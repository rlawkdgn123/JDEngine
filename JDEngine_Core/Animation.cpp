#include "pch.h"
#include "framework.h"
#include "Animation.h"
#include "D2DRenderer.h"

using namespace JDComponent;

void Animation::Update(float dt) {
    auto clip = AssetManager::Instance().GetAnimation(m_clipName);
    if (!clip) return;

    m_elapsed += dt * m_speed;

    const auto& frames = clip->frames;
    if (m_elapsed >= frames[m_currentFrame].duration) {
        m_elapsed -= frames[m_currentFrame].duration;
        m_currentFrame = (m_currentFrame + 1) % frames.size();
    }
}

void Animation::Render(ID2D1DeviceContext7* context, D2D1_MATRIX_3X2_F worldTransform)
{
    auto clip = AssetManager::Instance().GetAnimation(m_clipName);
    auto bitmap = AssetManager::Instance().GetTexture(m_clipName);
    if (!clip || !bitmap) return;

    const auto& frame = clip->frames[m_currentFrame];

    // 1) worldTransform 세팅
    auto ctx = D2DRenderer::Instance().GetD2DContext();
    ctx->SetTransform(worldTransform);

    // 2) destRect 계산 (원점 기준, 프레임 크기만큼)
    float w = frame.srcRect.right - frame.srcRect.left;
    float h = frame.srcRect.bottom - frame.srcRect.top;
    D2D1_RECT_F destRect = D2D1::RectF(0.0f, 0.0f, w, h);

    // 3) 래퍼 호출
    D2DRenderer::Instance().DrawBitmap(
        static_cast<ID2D1Bitmap1*>(bitmap),
        destRect,
        frame.srcRect,
        1.0f
    );
}