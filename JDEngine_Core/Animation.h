#pragma once
#include "Component.h"
#include "AssetManager.h"

using RenderLayerInfo = JDGlobal::Base::RenderLayerInfo;
using SortingLayer = JDGlobal::Base::SortingLayer;

namespace JDComponent
{
    class AnimationRender : public Component {
    public:
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

        // 클립 이름 설정
        void            SetClipName(const std::string& name) { m_clipName = name; }
        std::string     GetClipName() const { return m_clipName; }

        // 속도 제어
        void            SetSpeed(float speed) { m_speed = speed; }
        float           GetSpeed() const { return m_speed; }

        const RenderLayerInfo&  GetLayerInfo() const { return m_layerInfo; }
        void                    SetLayerInfo(const RenderLayerInfo& info) { m_layerInfo = info; }

    private:
        std::string m_clipName;
        float       m_speed;         // 재생 속도 배율
        size_t      m_currentFrame;
        float       m_elapsed;       // 누적 시간 (초)
        RenderLayerInfo m_layerInfo;
    };
}
