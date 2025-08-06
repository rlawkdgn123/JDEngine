#pragma once
#include "Component.h"
#include "AssetManager.h"

using RenderLayerInfo = JDGlobal::Base::RenderLayerInfo;
using SortingLayer = JDGlobal::Base::SortingLayer;

namespace JDComponent {
    class TextureRenderer : public Component {
    public:
        TextureRenderer(std::string textureName,
            const RenderLayerInfo& layerInfo = { SortingLayer::None, 0 })
            : m_textureName(std::move(textureName))
            , m_layerInfo(layerInfo)
        {
        }

        void Render(ID2D1DeviceContext7* context, D2D1_MATRIX_3X2_F worldTransform);

        void        SetTextureName(std::string& name) { m_textureName = name; }
        std::string GetTextureName() const { return m_textureName; }

        const RenderLayerInfo&  GetLayerInfo() const { return m_layerInfo; }
        void                    SetLayerInfo(const RenderLayerInfo& info) { m_layerInfo = info; }

        D2D1_SIZE_F GetOriginalTextureSize() const;

        void Update(float dt) override {}
        void OnEvent(const std::string& ev) override {}
        void OnEnable() override{}
        void OnDisable() override {}
    private:
        std::string m_textureName;
        RenderLayerInfo m_layerInfo;
    };
}