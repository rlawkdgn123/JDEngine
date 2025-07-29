#include "pch.h"
#include "framework.h"
#include "Sprite.h"

using namespace JDComponent;

void SpriteRenderer::Render(ID2D1DeviceContext* context, D2D1_MATRIX_3X2_F worldTransform)
{
    ID2D1Bitmap* bitmap = ResourceManager::Instance().GetTexture(m_textureName);
    
    if (!bitmap) return;

    context->SetTransform(worldTransform);

    D2D1_SIZE_F size = bitmap->GetSize();
    //D2D1_RECT_F destRect = D2D1::RectF(0.f, 0.f, size.width, size.height);
    D2D1_RECT_F destRect = D2D1::RectF(
        -size.width * 0.5f,  // 중심 기준
        -size.height * 0.5f,
        size.width * 0.5f,
        size.height * 0.5f
    );
    context->DrawBitmap(
        bitmap,
        &destRect,
        0.5f, // 
        D2D1_INTERPOLATION_MODE_LINEAR,
        nullptr // 전체 이미지 사용
    );
}