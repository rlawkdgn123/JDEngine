#pragma once
#include "Component.h"
#include "AssetManager.h"
#include <functional>

using RenderLayerInfo = JDGlobal::Base::RenderLayerInfo;
using SortingLayer = JDGlobal::Base::SortingLayer;

namespace JDComponent
{
    class AnimationRender : public Component {
    public:
        enum class PlayMode { Loop, Once, IntroThenLoop, LoopRange, IntroThenLoopRange};
        // clipName: 애니메이션 리소스 키
        // speed: 재생 속도 배율 (1.0 = 원래 속도, 2.0 = 2배 빠름 등)
        AnimationRender(const std::string& clipName, float speed = 1.0f, const JDGlobal::Base::RenderLayerInfo& layerInfo = { JDGlobal::Base::SortingLayer::None, 0 })
            : m_clipName(clipName)
            , m_speed(speed)
            , m_currentFrame(0)
            , m_elapsed(0.f)
            , m_layerInfo(layerInfo)
        {}

        void Render(ID2D1DeviceContext7* context, D2D1_MATRIX_3X2_F worldTransform);

        void Update(float dt) override;
        void OnEvent(const std::string& ev) override {}
        void OnEnable() override {}
        void OnDisable() override {}

        // 이미지 사이즈
        D2D1_SIZE_F GetFrameSize() const;

        // 클립 이름 설정
        void            SetClipName(const std::string& name) { m_clipName = name; }
        std::string     GetClipName() const { return m_clipName; }

        // 속도 제어
        void            SetSpeed(float speed) { m_speed = speed; }
        float           GetSpeed() const { return m_speed; }

        void SetFlipX(bool flip) { m_flipX = flip; }
        bool GetFlipX() const { return m_flipX; }

        const RenderLayerInfo&  GetLayerInfo() const { return m_layerInfo; }
        void                    SetLayerInfo(const RenderLayerInfo& info) { m_layerInfo = info; }

        void     SetPlayMode(PlayMode m) { m_playMode = m; }
        PlayMode GetPlayMode() const { return m_playMode; }

        void Play() { m_playing = true;  m_finished = false; }
        void Stop() { m_playing = false; }
        void Reset() { m_currentFrame = 0; m_elapsed = 0.f; m_finished = false; }
        void PlayOnceFromStart() { SetPlayMode(PlayMode::Once); Reset(); Play(); }

        void SetCurrentFrame(size_t idx) { m_currentFrame = (uint32_t)idx; m_elapsed = 0.f; }
        void SetLoopStartFrame(size_t idx) { m_loopStart = (uint32_t)idx; }
        void PlayOnceThenLoopFrom(size_t loopStart) {
            m_playMode = PlayMode::IntroThenLoop;
            m_loopStart = (uint32_t)loopStart;
            Reset();
            Play();
        }
        void PlayOnceThenLoopRange(size_t start, size_t end /*포함*/)
        {
            m_loopStart = (uint32_t)start;
            m_loopEnd = (uint32_t)end;
            m_playMode = PlayMode::IntroThenLoopRange;
            Reset();
            Play();
        }

        bool IsPlaying()  const { return m_playing; }
        bool IsFinished() const { return m_finished; }

        std::function<void()> onCompleted;

    private:
        std::string m_clipName;
        float       m_speed;         // 재생 속도 배율
        size_t      m_currentFrame;
        float       m_elapsed;       // 누적 시간 (초)
        RenderLayerInfo m_layerInfo;

        bool m_flipX = false;

        PlayMode      m_playMode = PlayMode::Loop;
        bool          m_playing = true;
        bool          m_finished = false;

        uint32_t m_loopStart = 0;
        uint32_t m_loopEnd = 0;
    };
}
