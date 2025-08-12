#include "pch.h"
#include "framework.h"
#include "Animation.h"
#include "D2DRenderer.h"

using namespace JDComponent;

void AnimationRender::Update(float dt)
{
    if (!m_playing || m_finished) return;

    auto clip = AssetManager::Instance().GetAnimationRender(m_clipName);
    if (!clip || clip->frames.empty()) return;

    // 루프 구간 보정
    const uint32_t lastIdx = (uint32_t)clip->frames.size() - 1;
    if (m_loopEnd > lastIdx) m_loopEnd = lastIdx;
    if (m_loopStart > m_loopEnd) m_loopStart = m_loopEnd;

    float frameDur = clip->frames[m_currentFrame].duration;
    m_elapsed += dt * m_speed;

    while (m_elapsed >= frameDur) {
        m_elapsed -= frameDur;

        auto advance = [&]() {
            // 공통: 다음 프레임
            ++m_currentFrame;
            // 모드별 래핑
            switch (m_playMode) {
            case PlayMode::Loop:
                if (m_currentFrame > lastIdx) m_currentFrame = 0;
                break;
            case PlayMode::LoopRange:
                if (m_currentFrame > m_loopEnd) m_currentFrame = m_loopStart;
                break;
            default: break; // Once, IntroThenLoop, IntroThenLoopRange는 아래에서 처리
            }
            };

        bool atLast = (m_currentFrame + 1 > lastIdx);

        if (atLast) {
            if (m_playMode == PlayMode::Once) {
                m_currentFrame = lastIdx;
                m_playing = false; m_finished = true;
                if (onCompleted) onCompleted();
                return;
            }
            else if (m_playMode == PlayMode::IntroThenLoop) {
                m_playMode = PlayMode::Loop;
                m_currentFrame = 0;
            }
            else if (m_playMode == PlayMode::IntroThenLoopRange) {
                m_playMode = PlayMode::LoopRange;
                m_currentFrame = m_loopStart;
            }
            else { // Loop / LoopRange면 advance에서 래핑됨
                // do nothing; 아래 advance
            }
        }
        else {
            // 일반 진행
        }

        advance();
        frameDur = clip->frames[m_currentFrame].duration;
    }
}


void AnimationRender::Render(ID2D1DeviceContext7* context, D2D1_MATRIX_3X2_F worldTransform)
{
    if (!GetEnabled()) return;

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