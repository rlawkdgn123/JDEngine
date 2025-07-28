#include "pch.h"
#include "Framework.h"
///////////////////////////////////////////////////////////////////////////
#pragma comment(lib, "dxguid.lib")
////////////////////////////////////////////////////////////////////////////
/*
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
*/
/////////////////////////////////////////////////////////////////////////
#include <shobjidl.h>            // IFileOpenDialog
#include <filesystem>            // C++17 std::filesystem
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>//C++14
#include <assert.h>
/////////////////////////////////////////////////////////////////////////
#include "EngineCore.h"
/////////////////////////////////////////////////////////////////////////
// 추후 애니메이션 부분은 분리 예정
using namespace std;

// (일반적으로 UTF-8 또는 로케일별 멀티바이트 인코딩)과
// std::wstring (일반적으로 Windows에서 UTF-16) 간의 변환을 위한 두 가지 유틸리티 함수

std::wstring ConvertToWString(const std::string& str)
{
    size_t len = 0;
    mbstowcs_s(&len, nullptr, 0, str.c_str(), _TRUNCATE);
    if (len == 0)
        return L"";

    std::wstring wstr(len, L'\0');
    mbstowcs_s(&len, &wstr[0], len, str.c_str(), _TRUNCATE);
    wstr.resize(len - 1); // Remove the null terminator added by mbstowcs_s
    return wstr;
}

std::string WStringToString(const std::wstring& wstr)
{
    size_t len = 0;
    wcstombs_s(&len, nullptr, 0, wstr.c_str(), _TRUNCATE);
    if (len == 0)
        return "";
    std::string str(len, '\0');
    wcstombs_s(&len, &str[0], len, wstr.c_str(), _TRUNCATE);
    str.resize(len - 1); // Remove the null terminator added by wcstombs_s
    return str;
}

/////////////////////////////////////////////////////////////////////////


// EngineCore 클래스의 멤버 함수 정의는 전역 네임스페이스에 있어야 합니다.
// 파일 끝에 있던 익명 네임스페이스를 제거했습니다.

bool EngineCore::Initialize()
{
    // 유니코드 기반의 윈도우 클래스 이름과 윈도우 이름 설정
    const wchar_t* className = L"JDEngine";
    const wchar_t* windowName = L"JDEngine";

    // 이 클래스의 부모 클래스(__super)에서 제공하는 Create 함수 호출
    // 전달된 클래스명 / 윈도우명 / 크기로 윈도우 생성 시도.
    if (false == __super::Create(className, windowName, 1204, 800)) {
        return false;
    }

    //if (false == InputManager::Instance().Initialize(m_hWnd))
    //{
    //    return false;
    //}
    
    m_EngineTimer = make_unique<GameTimer>(); // 팩토리에서 타이머 unique 형태로 할당


    //m_Renderer = std::make_shared<D2DRenderer>(); // D2DRenderer 객체를 shared_ptr로 생성 뒤 m_Renderer에 저장

    m_Renderer = make_shared<D2DRenderer>(); // 팩토리에서 렌더러 shared 형태로 할당
    m_Renderer->Initialize(m_hWnd);//Direct2D 초기화 작업(디바이스 생성) 수행

    //m_SceneManager = make_unique<SceneManager>(); // 팩토리에서 SceneManager unique 형태로 할당

    JDGlobal::Window::GlobalWindowSize::Instance().Set(this);
    
    SceneManager::Instance().RegisterScene(make_unique< JDScene::TestScene>(JDGlobal::Core::SceneType::SCENE_TEST, "TestScene01"));
    SceneManager::Instance().ChangeScene("TestScene01");

    m_ResourceManager = make_shared<ResourceManager>();

    ID2D1RenderTarget* renderTarget = m_Renderer->GetRenderTarget();

    if (!m_ResourceManager->Initialize(renderTarget)) {
        return false;
    }
    //파일 위치 확인용(디버그용)
    if (!std::experimental::filesystem::exists("../Resource/Test.png"))
        std::cout << "[ERROR] 파일이 존재하지 않음!" << std::endl;

    if (!m_ResourceManager->LoadTexture("Test", L"../Resource/Test.png")) {
        std::cout << "[ERROR] 텍스처 로드 실패" << std::endl;
    }
    if (!m_ResourceManager->LoadTexture("cat", L"../Resource/cat.png")) {
        std::cout << "[ERROR] 텍스처 로드 실패" << std::endl;
    }

    if (!m_ResourceManager->LoadTexture("graybirdsheet", L"../Resource/graybirdsheet.png")) {
        std::cout << "[ERROR] graybirdsheet 텍스처 로드 실패" << std::endl;
    }
    if (!m_ResourceManager->LoadAnimation("GrayBird", L"../Resource/graybirdsheet.json")) {
        std::cout << "[ERROR] 애니메이션 로드 실패!" << std::endl;
    }
    /*
    // [ImGUI] 컨텍스트 & 백엔드 초기화
    // ImGui 컨텍스트 생성
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGui_ImplWin32_Init(m_hWnd);
    */

    //D3D11Device* pd3dDevice = m_Renderer->GetD3DDevice(); // 렌더러에서 생성한 디바이스 연결
    //m_Renderer->Initialize(m_hWnd);

    // 이어서 렌더러에게 컨텍스트 받기
    ID3D11DeviceContext* pd3dDeviceContext = nullptr;
    pd3dDeviceContext = m_Renderer->GetD3DContext();

    // [ImGUI] DirectX 11 백엔드 초기화
    //ImGui_ImplDX11_Init(pd3dDevice, pd3dDeviceContext);

    // 타이머 초기화
    m_EngineTimer->Reset();

    return true;
}

void EngineCore::Run()
{
    MSG msg = { 0 };

    while (WM_QUIT != msg.message) { // 종료 메세지 이전까지 무한 반복
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) // 키 입력 메세지를 받았을 때
        {
            // PeekMessage가 검색한 메시지(msg)를 분석하여
            // 가상 키 코드 메시지(예: WM_KEYDOWN, WM_KEYUP)를 문자 메시지(예: WM_CHAR)로 변환
            if (false == InputManager::Instance().OnHandleMessage(msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else {
            // 키 메세지 입력이 없으면 기본 업데이트(반복) 로직 실행
            UpdateTime();
            UpdateLogic();
            Render();
        }
    }

}

void EngineCore::Finalize()
{
    // [ImGUI] DirectX 11 백엔드 정리
    //ImGui_ImplDX11_Shutdown();
    //ImGui_ImplWin32_Shutdown();
    //ImGui::DestroyContext();
    
    JDGlobal::Window::GlobalWindowSize::Instance().Set(nullptr); // 종료 시 해제 권장

    if (m_Renderer != nullptr) // 렌더러 종료
    {
        m_Renderer->Uninitialize();
        m_Renderer.reset();
    }
}

bool EngineCore::OnWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    /*
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
    {
        return true; // ImGui가 메시지를 처리했으면 true 반환
    }
    */
    return false;
}

void EngineCore::UpdateTime()
{
    assert(m_EngineTimer != nullptr);

    m_EngineTimer->Tick();
    float deltaTime = m_EngineTimer->DeltaTime();
    float fixedDeltaTime = 1.0f/60.0f;

    SceneManager::Instance().Update(deltaTime); // 0.016f : fixedUpdate
    // FixedUpdate 누적 처리
    m_FixedTimeAccumulator += deltaTime;
    while (m_FixedTimeAccumulator >= fixedDeltaTime)
    {
        SceneManager::Instance().FixedUpdate(fixedDeltaTime);
        m_FixedTimeAccumulator -= fixedDeltaTime;
    }
    SceneManager::Instance().LateUpdate(deltaTime);
    SceneManager::Instance().Render();
}

void EngineCore::UpdateLogic()
{
}

void EngineCore::Render()
{
    if (m_Renderer == nullptr) return;

    static float elapsed = 0.0f;
    static size_t frameIndex = 0;

    float deltaTime = m_EngineTimer->DeltaTime();

    m_Renderer->RenderBegin();

    ID2D1Bitmap* bitmap = m_ResourceManager->GetTexture("graybirdsheet"); // PNG 이름
    const AnimationClip* clip = m_ResourceManager->GetAnimation("GrayBird"); // JSON 애니메이션 이름

    if (bitmap && clip && !clip->frames.empty())
    {
        elapsed += deltaTime;

        if (elapsed >= clip->frames[frameIndex].duration)
        {
            elapsed = 0.0f;
            frameIndex++;
            if (frameIndex >= clip->frames.size())
            {
                frameIndex = clip->loop ? 0 : clip->frames.size() - 1;
            }
        }

        const D2D1_RECT_F& src = clip->frames[frameIndex].srcRect;
        D2D1_RECT_F dest = D2D1::RectF(100, 100,
            100 + (src.right - src.left),
            100 + (src.bottom - src.top));

        auto context = m_Renderer->GetD2DContext();

        // 1. 현재 프레임만 잘라낸 임시 비트맵 생성
        ComPtr<ID2D1Bitmap1> cropped = m_Renderer->CreateCroppedBitmap(static_cast<ID2D1Bitmap1*>(bitmap), src);
        if (cropped) {
            auto effect = m_Renderer->CreateGaussianBlurEffect(cropped.Get(), 5.0f);
            if (effect) {
                m_Renderer->GetD2DContext()->DrawImage(effect.Get(), D2D1::Point2F(dest.left, dest.top));
            }
            else {
                m_Renderer->DrawBitmap(cropped.Get(), dest);
            }
        }
        else {
            m_Renderer->DrawBitmap(static_cast<ID2D1Bitmap1*>(bitmap), dest, src, 1.0f);
        }

        // 2. 블러 이펙트 생성
        auto effect = m_Renderer->CreateGaussianBlurEffect(cropped.Get(), 5.0f);

        // 3. 출력
        if (effect)
            context->DrawImage(effect.Get(), D2D1::Point2F(dest.left, dest.top));
        else
            m_Renderer->DrawBitmap(cropped.Get(), dest);
    }

    ID2D1Bitmap* bitmap1 = m_ResourceManager->GetTexture("Test");
    if (bitmap1)
    {
        D2D1_RECT_F dest1 = D2D1::RectF(400, 100, 400 + bitmap1->GetSize().width, 100 + bitmap1->GetSize().height);
        m_Renderer->DrawBitmap(static_cast<ID2D1Bitmap1*>(bitmap1), dest1);
    }

    ID2D1Bitmap* bitmap2 = m_ResourceManager->GetTexture("cat");
    if (bitmap2)
    {
        auto context = m_Renderer->GetD2DContext();

        // 1. 원본 비트맵에서 그레이스케일 효과 생성
        ComPtr<ID2D1Effect> grayEffect;
        HRESULT hr = context->CreateEffect(CLSID_D2D1Grayscale, &grayEffect);
        if (FAILED(hr)) {
            std::cout << "그레이스케일 효과 생성 실패" << std::endl;
            return;
        }

        grayEffect->SetInput(0, bitmap2);

        // 2. 출력 위치 지정
        D2D1_POINT_2F destPos = { 600.f, 200.f };

        // 3. 그레이스케일 효과 출력
        context->DrawImage(grayEffect.Get(), destPos);
    }

    m_Renderer->RenderEnd(false);


    m_Renderer->Present();


}


void EngineCore::OnResize(int width, int height) // 창 크기 재조정
{
    __super::OnResize(width, height);

    if (m_Renderer != nullptr) m_Renderer->Resize(width, height);
}

void EngineCore::OnClose()
{
    std::cout << "OnClose" << std::endl;
}

