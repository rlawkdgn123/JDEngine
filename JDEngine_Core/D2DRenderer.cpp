#include "pch.h"
#include "D2DRenderer.h"

void D2DRenderer::Initialize(HWND hwnd)
{
    m_hwnd = hwnd;

    CreateDeviceAndSwapChain(hwnd);
    CreateRenderTargets();
    CreateWriteResource();

    ComPtr<IWICImagingFactory> wicFactory;

    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&wicFactory));

    DX::ThrowIfFailed(hr);

    m_wicFactory = wicFactory;

    m_camera = std::make_unique<Camera>();
}

void D2DRenderer::Uninitialize()
{
    ReleaseRenderTargets();

    m_wicFactory = nullptr;

    m_targetBitmap = nullptr;
    m_brush = nullptr;

    m_d2dContext = nullptr;
    m_d2dDevice = nullptr;

    m_swapChain = nullptr;
    m_d3dDevice = nullptr;
}

void D2DRenderer::Resize(UINT width, UINT height)
{
    if (nullptr == m_swapChain) return; // 초기화 전에 호출이 될 수 있음.
    ReleaseRenderTargets();

    // 스왑체인 크기 조정 후 렌더 타겟 재생성
    DX::ThrowIfFailed(m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0));

    CreateRenderTargets();
}

void D2DRenderer::DrawLine(float x1, float y1, float x2, float y2, const D2D1::ColorF& color)
{
    m_brush->SetColor(color);
    m_d2dContext->DrawLine(D2D1::Point2F(x1, y1), D2D1::Point2F(x2, y2), m_brush.Get());
}

void D2DRenderer::DrawCircle(float x, float y, float radius, const D2D1::ColorF& color)
{
    m_brush->SetColor(color);
    m_d2dContext->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius), m_brush.Get());
}

void D2DRenderer::DrawRectangle(float left, float top, float right, float bottom, const D2D1::ColorF& color)
{
    m_brush->SetColor(color);
    m_d2dContext->DrawRectangle(D2D1::Rect(left, top, right, bottom), m_brush.Get());
}

void D2DRenderer::DrawBitmap(ID2D1Bitmap1* bitmap, D2D1_RECT_F dest)
{
    D2D1_MATRIX_3X2_F currentTransform;
    m_d2dContext->GetTransform(&currentTransform);

    D2D1_MATRIX_3X2_F flipY = D2D1::Matrix3x2F::Scale(1.f, -1.f);
    //D2D1_MATRIX_3X2_F translateBack = D2D1::Matrix3x2F::Translation(0.f, -2 * dest.top - (dest.bottom - dest.top));

    D2D1_MATRIX_3X2_F corrected = flipY * currentTransform;
    m_d2dContext->SetTransform(corrected);

    m_d2dContext->DrawBitmap(bitmap, dest);

    m_d2dContext->SetTransform(currentTransform);
}

void D2DRenderer::DrawBitmap(ID2D1Bitmap1* bitmap, D2D1_RECT_F destRect, D2D1_RECT_F srcRect, float opacity)
{
    D2D1_MATRIX_3X2_F currentTransform;
    m_d2dContext->GetTransform(&currentTransform);

    D2D1_MATRIX_3X2_F flipY = D2D1::Matrix3x2F::Scale(1.f, -1.f);
    D2D1_MATRIX_3X2_F translateBack = D2D1::Matrix3x2F::Translation(0.f, -2 * destRect.top - (destRect.bottom - destRect.top));

    D2D1_MATRIX_3X2_F corrected = flipY * translateBack * currentTransform;
    m_d2dContext->SetTransform(corrected);

    m_d2dContext->DrawBitmap(
        bitmap,
        destRect,
        opacity,
        D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
        srcRect
    );

    m_d2dContext->SetTransform(currentTransform);
}

ComPtr<ID2D1Effect> D2DRenderer::CreateGaussianBlurEffect(ID2D1Bitmap1* srcBitmap, float blurAmount)
{
    ComPtr<ID2D1Effect> blurEffect;
    HRESULT hr = m_d2dContext->CreateEffect(CLSID_D2D1GaussianBlur, &blurEffect);
    if (FAILED(hr)) return nullptr;

    blurEffect->SetInput(0, srcBitmap);
    blurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, blurAmount);
    return blurEffect;
}

ComPtr<ID2D1Bitmap1> D2DRenderer::CreateCroppedBitmap(ID2D1Bitmap1* src, D2D1_RECT_F cropRect)
{
    if (!src) return nullptr;

    D2D1_SIZE_U size = {
        static_cast<UINT32>(cropRect.right - cropRect.left),
        static_cast<UINT32>(cropRect.bottom - cropRect.top)
    };

    if (size.width == 0 || size.height == 0) return nullptr;

    D2D1_SIZE_F srcSize = src->GetSize();
    if (cropRect.left < 0 || cropRect.top < 0 || cropRect.right > srcSize.width || cropRect.bottom > srcSize.height)
        return nullptr;

    D2D1_BITMAP_PROPERTIES1 props = {};
    props.pixelFormat = src->GetPixelFormat();

    props.bitmapOptions = D2D1_BITMAP_OPTIONS_NONE;

    src->GetDpi(&props.dpiX, &props.dpiY);

    auto context = GetD2DContext();

    ComPtr<ID2D1Bitmap1> cropped;
    HRESULT hr = context->CreateBitmap(size, nullptr, 0, &props, &cropped);
    if (FAILED(hr)) {
        std::cout << "[CreateCroppedBitmap] CreateBitmap 실패. HRESULT: 0x" << std::hex << hr << std::endl;
        return nullptr;
    }

    D2D1_POINT_2U destPoint = { 0, 0 };
    D2D1_RECT_U srcRect = {
        static_cast<UINT32>(cropRect.left),
        static_cast<UINT32>(cropRect.top),
        static_cast<UINT32>(cropRect.right),
        static_cast<UINT32>(cropRect.bottom)
    };

    hr = cropped->CopyFromBitmap(&destPoint, src, &srcRect);
    if (FAILED(hr)) return nullptr;

    return cropped;
}



void D2DRenderer::DrawMessage(const wchar_t* text, float left, float top, float width, float height, const D2D1::ColorF& color)
{
    if (nullptr == m_textBrush)
    {
        m_d2dContext->CreateSolidColorBrush(D2D1::ColorF(color), &m_textBrush);
    }

    m_textBrush->SetColor(color);
    D2D1_RECT_F layoutRect = D2D1::RectF(left, top, left + width, top + height);

    m_d2dContext->DrawTextW(
        text,
        static_cast<UINT32>(wcslen(text)),
        m_textFormat.Get(),
        layoutRect,
        m_textBrush.Get(),
        D2D1_DRAW_TEXT_OPTIONS_NONE,
        DWRITE_MEASURING_MODE_NATURAL);
}

void D2DRenderer::SetTransform(const D2D1_MATRIX_3X2_F tm)
{
    if (m_d2dContext) m_d2dContext->SetTransform(tm);
}

void D2DRenderer::RenderBegin()
{
    m_d2dContext->BeginDraw();
    m_d2dContext->Clear(D2D1::ColorF(D2D1::ColorF::White)); // 배경을 흰색으로 초기화
}

void D2DRenderer::RenderEnd(bool bPresent)
{
    m_d2dContext->EndDraw();

    if (bPresent)
    {
        Present();
    }
}

void D2DRenderer::RenderGameObject(const GameObject& obj)
{
    using namespace JDGameObject;
    using namespace JDComponent;
    using Transform = JDComponent::D2DTM::Transform;
    using SpriteRenderer = JDComponent::SpriteRenderer;

    auto tf = obj.GetComponent<Transform>();
    auto renderer = obj.GetComponent<SpriteRenderer>();

    if (tf && renderer)
    {
        D2D1_MATRIX_3X2_F worldMatrix = tf->GetWorldMatrix();
        if (m_camera)
        {
            worldMatrix = worldMatrix * m_camera->GetViewMatrix();
        }
        ID2D1DeviceContext7* context = D2DRenderer::Instance().GetD2DContext();
        renderer->Render(context, worldMatrix);
    }
}

void D2DRenderer::RenderUIObject(const UIObject& uiObj)
{
    using namespace JDGameObject;
    using namespace JDComponent;
    using RectTransform = JDComponent::D2DTM::RectTransform;

    auto rtf = uiObj.GetComponent<RectTransform>();
    auto imageComponent = uiObj.GetComponent<UI_ImageComponent>();

    if (rtf && imageComponent)
    {
        ID2D1DeviceContext7* context = D2DRenderer::Instance().GetD2DContext();

        D2D1_MATRIX_3X2_F originalTransform;
        context->GetTransform(&originalTransform);

        D2D1_MATRIX_3X2_F worldMatrix = rtf->GetWorldMatrix();

        // context->SetTransform(worldMatrix * originalTransform);

        // 카메라 영향을 빼고, 월드 행렬만 사용
        imageComponent->Render(context, worldMatrix);

        context->SetTransform(originalTransform);
    }
}

void D2DRenderer::Present()
{
    // 렌더링 작업이 끝나면 스왑체인에 프레임을 표시
    HRESULT hr = m_swapChain->Present(1, 0);

    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        Uninitialize();     // 디바이스가 제거되거나 리셋된 경우, 재초기화 필요
        Initialize(m_hwnd);
    }
    else
    {
        DX::ThrowIfFailed(hr);
    }
}

void D2DRenderer::CreateDeviceAndSwapChain(HWND hwnd)
{
    //1. D3D11 디바이스 생성
    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    ComPtr<ID3D11Device> d3dDevice;
    ComPtr<ID3D11DeviceContext> d3dContext;

    HRESULT hr = D3D11CreateDevice(
        nullptr,                            //[in, optional]  IDXGIAdapter* pAdapter
        D3D_DRIVER_TYPE_HARDWARE,           //D3D_DRIVER_TYPE DriverType
        nullptr,                            //HJDEngine         Software
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,   //UINT            Flags
        featureLevels,                      //[in, optional]  const D3D_FEATURE_LEVEL* pFeatureLevels
        ARRAYSIZE(featureLevels),           //UINT            FeatureLevels
        D3D11_SDK_VERSION,                  //UINT            SDKVersion
        &d3dDevice,                         //[out, optional] ID3D11Device** ppDevice
        nullptr,                            //[out, optional] D3D_FEATURE_LEVEL* pFeatureLevel
        &d3dContext);                       //[out, optional] ID3D11DeviceContext** ppImmediateContext

    DX::ThrowIfFailed(hr);

    // 2. DXGI 스왑체인 생성
    ComPtr<IDXGIDevice> dxgiDevice;
    hr = d3dDevice.As(&dxgiDevice);

    DX::ThrowIfFailed(hr);

    ComPtr<IDXGIAdapter> dxgiAdapter;
    hr = dxgiDevice->GetAdapter(&dxgiAdapter);

    DX::ThrowIfFailed(hr);

    ComPtr<IDXGIFactory2> dxgiFactory;
    hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));

    DX::ThrowIfFailed(hr);

    DXGI_SWAP_CHAIN_DESC1 scDesc = {};
    scDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    scDesc.SampleDesc.Count = 1;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.BufferCount = 2;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scDesc.Scaling = DXGI_SCALING_STRETCH;

    ComPtr<IDXGISwapChain1> swapChain;
    hr = dxgiFactory->CreateSwapChainForHwnd(
        d3dDevice.Get(), hwnd, &scDesc, nullptr, nullptr, &swapChain);

    DX::ThrowIfFailed(hr);

    // 3. ID2D1Factory4 생성
    D2D1_FACTORY_OPTIONS opts = {};
    ComPtr<ID2D1Factory8> d2dFactory;

#if defined(_DEBUG)
    opts.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

    hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        __uuidof(ID2D1Factory8),
        &opts,
        reinterpret_cast<void**>(d2dFactory.GetAddressOf()));

    DX::ThrowIfFailed(hr);

    // 4. ID2D1Device4 생성
    ComPtr<ID2D1Device> baseDevice;
    hr = d2dFactory->CreateDevice(dxgiDevice.Get(), &baseDevice);

    DX::ThrowIfFailed(hr);

    ComPtr<ID2D1Device7> d2dDevice;
    hr = baseDevice.As(&d2dDevice);

    DX::ThrowIfFailed(hr);

    // 5. ID2D1DeviceContext7 생성
    ComPtr<ID2D1DeviceContext7> d2dContext;//
    hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dContext);

    DX::ThrowIfFailed(hr);

    m_d3dDevice = d3dDevice;
    m_d3dContext = d3dContext;
    m_swapChain = swapChain;

    m_d2dDevice = d2dDevice;
    m_d2dContext = d2dContext;
}

void D2DRenderer::CreateRenderTargets()
{
    // 6. SwapChain 백버퍼 -> D2D Bitmap1 을 생성하여 렌더 타겟으로 설정

    ComPtr<IDXGISurface> dxgiSurface;
    HRESULT hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiSurface));

    DX::ThrowIfFailed(hr);

    D2D1_BITMAP_PROPERTIES1 bitmapProps = {};
    bitmapProps.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    bitmapProps.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bitmapProps.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    bitmapProps.dpiX = bitmapProps.dpiY = 96.0f;

    ComPtr<ID2D1Bitmap1> targetBitmap;
    hr = m_d2dContext->CreateBitmapFromDxgiSurface(dxgiSurface.Get(), &bitmapProps, targetBitmap.GetAddressOf());

    DX::ThrowIfFailed(hr);

    // 렌더 타겟 설정
    m_d2dContext->SetTarget(targetBitmap.Get());

    m_targetBitmap = targetBitmap;

    hr = m_d2dContext->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Blue),
        &m_brush);

    DX::ThrowIfFailed(hr);

    // ImGUI 은 D3D11 렌더 타겟 뷰가 필요!!! 
    ComPtr<ID3D11Texture2D> backBuffer;
    hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

    // RenderTargetView 생성
    ComPtr<ID3D11RenderTargetView> mainRTV;
    hr = m_d3dDevice->CreateRenderTargetView(
        backBuffer.Get(),      // 텍스처
        nullptr,               // 기본 뷰 설명
        &mainRTV               // 출력 RTV 포인터
    );

    DX::ThrowIfFailed(hr);

    m_d3dRenderTV = mainRTV;
}

void D2DRenderer::CreateWriteResource()
{
    ComPtr<IDWriteFactory> writeFactory = nullptr;
    HRESULT hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(writeFactory.GetAddressOf()));

    DX::ThrowIfFailed(hr);

    writeFactory->CreateTextFormat(
        L"", // FontName    제어판-모든제어판-항목-글꼴-클릭 으로 글꼴이름 확인가능
        NULL,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        15.0f,   // Font Size
        L"", //locale
        &m_textFormat);

    DX::ThrowIfFailed(hr);

    m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING); // 왼쪽 정렬
    m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR); // 위쪽 정렬
    m_textFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP); // 줄바꿈 
}

void D2DRenderer::ReleaseRenderTargets()
{
    if (m_d2dContext)
    {
        m_d2dContext->SetTarget(nullptr);
    }

    m_d3dRenderTV.Reset();

    m_targetBitmap.Reset();
    m_brush.Reset();
    m_textBrush.Reset();
}

void D2DRenderer::CreateBitmapFromFile(const wchar_t* path, ID2D1Bitmap1*& outBitmap)
{
    ComPtr<IWICBitmapDecoder>     decoder;
    ComPtr<IWICBitmapFrameDecode> frame;
    ComPtr<IWICFormatConverter>   converter;


    HRESULT hr = m_wicFactory->CreateDecoderFromFilename(
        path, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);

    DX::ThrowIfFailed(hr);


    hr = decoder->GetFrame(0, &frame);

    DX::ThrowIfFailed(hr);

    hr = m_wicFactory->CreateFormatConverter(&converter);

    DX::ThrowIfFailed(hr);


    hr = converter->Initialize(
        frame.Get(),
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.0f,
        WICBitmapPaletteTypeCustom
    );

    DX::ThrowIfFailed(hr);

    // Direct2D 비트맵 속성 (premultiplied alpha, B8G8R8A8_UNORM)
    D2D1_BITMAP_PROPERTIES1 bmpProps = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_NONE,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
    );

    // DeviceContext에서 WIC 비트맵으로부터 D2D1Bitmap1 생성
    hr = m_d2dContext->CreateBitmapFromWicBitmap(converter.Get(), &bmpProps, &outBitmap);
}


