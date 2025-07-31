#pragma once
#include "Component.h"
#include "AssetManager.h"

using RenderLayerInfo = JDGlobal::Base::RenderLayerInfo;
using SortingLayer = JDGlobal::Base::SortingLayer;

namespace JDComponent
{
    class AnimationRenderer : public Component {
    public:
        // clipName: �ִϸ��̼� ���ҽ� Ű
        // speed: ��� �ӵ� ���� (1.0 = ���� �ӵ�, 2.0 = 2�� ���� ��)
        AnimationRenderer(const std::string& clipName, float speed = 1.0f, const JDGlobal::Base::RenderLayerInfo& layerInfo = { JDGlobal::Base::SortingLayer::None, 0 })
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

        // �ӵ� ����
        void SetSpeed(float speed) { m_speed = speed; }
        float GetSpeed() const { return m_speed; }

        const RenderLayerInfo& GetLayerInfo() const { return m_layerInfo; }
        void SetLayerInfo(const RenderLayerInfo& info) { m_layerInfo = info; }

    private:
        std::string m_clipName;
        float       m_speed;         // ��� �ӵ� ����
        size_t      m_currentFrame;
        float       m_elapsed;       // ���� �ð� (��)
        RenderLayerInfo m_layerInfo;
    };
}
