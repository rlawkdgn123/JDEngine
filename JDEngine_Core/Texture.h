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
            m_size = { 100.f, 100.f };
        };
    private:
        TextureRenderer() = delete;

    public:
        void Render(ID2D1DeviceContext7* context, D2D1_MATRIX_3X2_F worldTransform);

        void        SetTextureName(const std::string& name) { m_textureName = name; }
        std::string GetTextureName() const { return m_textureName; }

        const RenderLayerInfo& GetLayerInfo() const { return m_layerInfo; }
        void                    SetLayerInfo(const RenderLayerInfo& info) { m_layerInfo = info; }

        D2D1_SIZE_F GetSize() const { return m_size; }
        void        SetSize(const D2D1_SIZE_F& newSize) { m_size = newSize; }

        void SetFlipX(bool flip) { m_flipX = flip; }
        bool GetFlipX() const { return m_flipX; }

        D2D1_SIZE_F GetOriginalTextureSize() const;

        void Update(float dt) override {}
        void OnEvent(const std::string& ev) override {}
        void OnEnable() override {}
        void OnDisable() override {}

    private:
        std::string m_textureName;
        RenderLayerInfo m_layerInfo;

        // 사용자 정의 크기
        D2D1_SIZE_F m_size{ 100.0f, 100.0f };

        bool m_flipX = false;
    };
}