#pragma once
#include "Component.h"

using RenderLayerInfo = JDGlobal::Base::RenderLayerInfo;

namespace JDComponent {

    class RenderLayer : public Component {
    public:
        RenderLayer(const RenderLayerInfo& info)
            : m_info(info) {
        }

        const RenderLayerInfo& GetInfo() const { return m_info; }
        void SetInfo(const RenderLayerInfo& info) { m_info = info; }

        void Update(float dt) override {}
        void OnEvent(const std::string& ev) override {}
        void OnEnable() override {}
        void OnDisable() override {}
    private:
        RenderLayerInfo m_info;
    };

}