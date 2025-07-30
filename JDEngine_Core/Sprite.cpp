#include "pch.h"
#include "framework.h"
#include "Sprite.h"
#include "D2DRenderer.h"

using namespace JDComponent;

void SpriteRenderer::Render(ID2D1DeviceContext7* /*context*/, D2D1_MATRIX_3X2_F worldTransform)
{
    // ResourceManager에서 ID2D1Bitmap1*으로 받아옵니다.
    auto bitmap = static_cast<ID2D1Bitmap1*>(
        ResourceManager::Instance().GetTexture(m_textureName)
        );
    if (!bitmap) return;

    // 월드 변환 설정
    auto ctx = D2DRenderer::Instance().GetD2DContext();
    ctx->SetTransform(worldTransform);

    // 비트맵 크기 구해서 중심 기준 destRect 계산
    D2D1_SIZE_F size = bitmap->GetSize();
    D2D1_RECT_F destRect = D2D1::RectF(
        -size.width * 0.5f,
        -size.height * 0.5f,
        size.width * 0.5f,
        size.height * 0.5f
    );

    // srcRect를 nullptr로 넘기면 전체 이미지를 사용합니다.
    // opacity는 0.5f에서 1.0f 등 원하는 값으로 조정하세요.
    D2DRenderer::Instance().DrawBitmap(
        bitmap,
        destRect,
        /* srcRect */ D2D1::RectF(0, 0, size.width, size.height),
        /* opacity */ 0.5f
    );
}
