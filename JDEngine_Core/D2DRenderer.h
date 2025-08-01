#pragma once
#include <wrl/client.h>
#include <d2d1helper.h>
#include "Camera.h"
#include "GameObjectBase.h"
#include "UIObject.h"
#include "Texture.h"
#include "Animation.h"

using GameObject = JDGameObject::GameObject;
using UIObject = JDGameObject::UIObject;
using Microsoft::WRL::ComPtr;

class D2DRenderer
{
public:
    static D2DRenderer& Instance()
    {
        static D2DRenderer instance;
        return instance;
    }
    // 복사/이동 금지
    D2DRenderer(const D2DRenderer&) = delete;
    D2DRenderer& operator=(const D2DRenderer&) = delete;

    // 기존 public 함수들 그대로 유지
    ~D2DRenderer() { Uninitialize(); }

    void Initialize(HWND hwnd);

    void Uninitialize();

    void Resize(UINT width, UINT height);

    void DrawLine(float x1, float y1, float x2, float y2, const D2D1::ColorF& color);

    void DrawCircle(float x, float y, float radius, const D2D1::ColorF& color);

    void DrawRectangle(float left, float top, float right, float bottom, const D2D1::ColorF& color);

    void DrawBitmap(ID2D1Bitmap1* bitmap, D2D1_RECT_F dest);

    void DrawBitmap(ID2D1Bitmap1* bitmap, D2D1_RECT_F destRect, D2D1_RECT_F srcRect, float opacity = 1.0f);

    void DrawMessage(const wchar_t* text, float left, float top, float width, float height, const D2D1::ColorF& color, IDWriteTextFormat* textFormat);

    void SetTransform(const D2D1_MATRIX_3X2_F tm);

    void SetCamera(std::shared_ptr<Camera> camera) { m_camera = std::move(camera); }

    std::shared_ptr<Camera> GetCamera() const { return m_camera; }

    void RenderGameObject(const GameObject& obj , float dt);

    void RenderUIObject(const UIObject& uiObj);

    void RenderBegin();

    void RenderEnd(bool bPresent = true);

    void Present();

    void CreateBitmapFromFile(const wchar_t* path, ID2D1Bitmap1*& outBitmap);

    ID3D11Device* GetD3DDevice() const { return m_d3dDevice.Get(); }

    ID3D11DeviceContext* GetD3DContext() const { return m_d3dContext.Get(); }

    ID3D11RenderTargetView* GetD3DRenderTargetView() const { return m_d3dRenderTV.Get(); }

    ID2D1RenderTarget* GetRenderTarget() const { return m_d2dContext.Get(); }

    Microsoft::WRL::ComPtr<ID2D1Effect> CreateGaussianBlurEffect(ID2D1Bitmap1* srcBitmap, float blurAmount);// 이펙트

    ID2D1DeviceContext7* GetD2DContext() const { return m_d2dContext.Get(); }

    const std::unordered_map<std::string, ComPtr<IDWriteTextFormat>>& GetTextFormats() const {
        return m_textFormats;
    }

    Microsoft::WRL::ComPtr<ID2D1Bitmap1> CreateCroppedBitmap(ID2D1Bitmap1* src, D2D1_RECT_F cropRect);
private:
    D2DRenderer() = default;

    void CreateDeviceAndSwapChain(HWND hwnd);

    void CreateRenderTargets();

    void CreateWriteResource();

    void ReleaseRenderTargets();

private:
    HWND m_hwnd = nullptr;

    std::shared_ptr<Camera> m_camera;

    Microsoft::WRL::ComPtr<ID3D11Device>           m_d3dDevice;
    Microsoft::WRL::ComPtr<IDXGISwapChain1>        m_swapChain;

    Microsoft::WRL::ComPtr<ID3D11DeviceContext>    m_d3dContext;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_d3dRenderTV; // D3D render target view

    Microsoft::WRL::ComPtr<ID2D1Bitmap1>           m_targetBitmap; // D2D render target bitmap
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>   m_brush;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>   m_textBrush;
    // ComPtr<IDWriteTextFormat>      m_textFormat;
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<IDWriteTextFormat>> m_textFormats;

    Microsoft::WRL::ComPtr<ID2D1Device7>           m_d2dDevice;
    Microsoft::WRL::ComPtr<ID2D1DeviceContext7>    m_d2dContext;

    Microsoft::WRL::ComPtr<IWICImagingFactory>     m_wicFactory;
};

