#pragma once
#include "pch.h"
#include "Renderframework.h"


namespace JDInterface {
    class D2DRenderer {
    public:
        virtual ~D2DRenderer() = default;

        virtual void Initialize(HWND hwnd) = 0;
        virtual void Uninitialize() = 0;
        virtual void Resize(UINT width, UINT height) = 0;

        virtual void DrawLine(float x1, float y1, float x2, float y2, const D2D1::ColorF& color) = 0;
        virtual void DrawCircle(float x, float y, float radius, const D2D1::ColorF& color) = 0;
        virtual void DrawRectangle(float left, float top, float right, float bottom, const D2D1::ColorF& color) = 0;

        virtual void DrawBitmap(ID2D1Bitmap1* bitmap, D2D1_RECT_F dest) = 0;
        virtual void DrawBitmap(ID2D1Bitmap1* bitmap, D2D1_RECT_F destRect, D2D1_RECT_F srcRect, float opacity = 1.0f) = 0;

        virtual void DrawMessage(const wchar_t* text, float left, float top, float width, float height, const D2D1::ColorF& color) = 0;

        virtual void SetTransform(const D2D1_MATRIX_3X2_F tm) = 0;

        virtual void RenderBegin() = 0;
        virtual void RenderEnd(bool bPresent = true) = 0;
        virtual void Present() = 0;

        virtual void CreateBitmapFromFile(const wchar_t* path, ID2D1Bitmap1*& outBitmap) = 0;

        virtual ID3D11Device* GetD3DDevice() const = 0;
        virtual ID3D11DeviceContext* GetD3DContext() const = 0;
        virtual ID3D11RenderTargetView* GetD3DRenderTargetView() const = 0;
    };
}