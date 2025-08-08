#include "pch.h"
#include "framework.h"
#include "Texture.h"
#include "D2DRenderer.h"

using namespace JDComponent;

void TextureRenderer::Render(ID2D1DeviceContext7* /*context*/, D2D1_MATRIX_3X2_F worldTransform)
{
    // AssetManager에서 ID2D1Bitmap1*으로 받아옵니다.
    auto bitmap = static_cast<ID2D1Bitmap1*>(
        AssetManager::Instance().GetTexture(m_textureName)
        );
    if (!bitmap) return;

    D2D1_RECT_F destRect = D2D1::RectF(
        -m_size.width * 0.5f,   // left
        m_size.height * 0.5f,  // top
        m_size.width * 0.5f,    // right
        -m_size.height * 0.5f   // bottom
    );

    D2D1_MATRIX_3X2_F flipMatrix = D2D1::Matrix3x2F::Identity();
    if (m_flipX) {
        // 중심 기준이므로 (0,0) 기준에서 X축 반전
        flipMatrix = D2D1::Matrix3x2F::Scale(-1.0f, 1.0f, D2D1::Point2F(0, 0));
    }

    auto ctx = D2DRenderer::Instance().GetD2DContext();
    ctx->SetTransform(flipMatrix * worldTransform);

    D2D1_SIZE_F originalSize = bitmap->GetSize();
    D2DRenderer::Instance().DrawBitmap(
        bitmap,
        destRect,
        /* srcRect */ D2D1::RectF(0, 0, originalSize.width, originalSize.height),
        /* opacity */ 1.0f
    );

    //// srcRect를 nullptr로 넘기면 전체 이미지를 사용합니다.
    //// opacity는 0.5f에서 1.0f 등 원하는 값으로 조정하세요.
    //D2DRenderer::Instance().DrawBitmap(
    //    bitmap,
    //    destRect,
    //    /* srcRect */ D2D1::RectF(0, 0, size.width, size.height),
    //    /* opacity */ 1.0f
    //);
}

// 텍스처의 원본 사이즈 반환
D2D1_SIZE_F TextureRenderer::GetOriginalTextureSize() const
{
    auto bitmap = static_cast<ID2D1Bitmap1*>(
        AssetManager::Instance().GetTexture(m_textureName)
        );
    if (bitmap)
    {
        return bitmap->GetSize();
    }
    return { 0, 0 }; // 텍스처가 없으면 0,0 반환
}
