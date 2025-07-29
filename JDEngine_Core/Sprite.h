#pragma once
#include "Component.h"
#include "ResourceManager.h"

namespace JDComponent {
    class SpriteRenderer : public Component {
    public:
        SpriteRenderer(std::string textureName)
            : m_textureName(std::move(textureName)) {
        }

        void Render(ID2D1DeviceContext* context, D2D1_MATRIX_3X2_F worldTransform);

        std::string GetTextureName() const { return m_textureName; }

        void Update(float dt) override {}
        void OnEvent(const std::string& ev) override {}
        void OnEnable() override{}
        void OnDisable() override {}
    private:
        std::string m_textureName;
    };
}