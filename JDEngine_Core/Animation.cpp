#include "pch.h"
#include "framework.h"
#include "Animation.h"
#include "D2DRenderer.h"

using namespace JDComponent;

void AnimationRender::Update(float dt) {
    auto clip = AssetManager::Instance().GetAnimationRender(m_clipName);
    if (!clip) return;

    m_elapsed += dt * m_speed;

    const auto& frames = clip->frames;
    if (m_elapsed >= frames[m_currentFrame].duration) {
        m_elapsed -= frames[m_currentFrame].duration;
        m_currentFrame = (m_currentFrame + 1) % frames.size();
    }
}

void AnimationRender::Render(ID2D1DeviceContext7* context, D2D1_MATRIX_3X2_F worldTransform)
{
    auto clip = AssetManager::Instance().GetAnimationRender(m_clipName);
    auto bitmap = AssetManager::Instance().GetTexture(m_clipName);
    if (!clip || !bitmap) return;

    const auto& frame = clip->frames[m_currentFrame];

    float w = frame.srcRect.right - frame.srcRect.left;
    float h = frame.srcRect.bottom - frame.srcRect.top;
    D2D1_RECT_F destRect = D2D1::RectF(-w * 0.5f, h * 0.5f, w * 0.5f, -h * 0.5f);

    D2D1_MATRIX_3X2_F flipMatrix = D2D1::Matrix3x2F::Identity();
    if (m_flipX) {
        // 중심 기준이므로 (0,0) 기준에서 X축 반전
        flipMatrix = D2D1::Matrix3x2F::Scale(-1.0f, 1.0f, D2D1::Point2F(0, 0));
    }

    auto ctx = D2DRenderer::Instance().GetD2DContext();
    ctx->SetTransform(flipMatrix * worldTransform);

    D2DRenderer::Instance().DrawBitmap(
        static_cast<ID2D1Bitmap1*>(bitmap),
        destRect,
        frame.srcRect,
        1.0f
    );
}

D2D1_SIZE_F JDComponent::AnimationRender::GetFrameSize() const
{
    auto clip = AssetManager::Instance().GetAnimationRender(m_clipName);
    if (!clip || clip->frames.empty())
    {
        return { 0, 0 };
    }

    // 보통 첫 번째 프레임을 대표 크기로 사용합니다.
    const auto& frame = clip->frames[0];
    float w = frame.srcRect.right - frame.srcRect.left;
    float h = frame.srcRect.bottom - frame.srcRect.top;
    return { w, h };
}