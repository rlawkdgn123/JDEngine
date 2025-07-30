#pragma once
#include "GameObjectBase.h"
#include "RectTransform.h"
#include "AssetManager.h"

namespace JDComponent {

    using RectTransform = D2DTM::RectTransform;
    using Vector2F = JDMath::Vector2F;

    template<typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    class UI_ImageComponent : public Component
    {
    public:
        UI_ImageComponent(std::string textureName)
            : m_textureName(std::move(textureName)) 
        {
            m_color = D2D1::ColorF(D2D1::ColorF::White);
        }

        void Render(ID2D1DeviceContext7* context, D2D1_MATRIX_3X2_F viewTransform);

        void            SetTextureName(std::string& name) { m_textureName = name; }
        std::string     GetTextureName() const { return m_textureName; }

        void            SetColor(const D2D1_COLOR_F& color);
        D2D1_COLOR_F    GetColor() const { return m_color; }

    private:
        std::string             m_textureName;
        D2D1_COLOR_F            m_color = D2D1::ColorF(D2D1::ColorF::White);

        // Component을(를) 통해 상속됨 [ 빈 함수 ]
        void Update(float deltaTime) override {};
        void OnEvent(const std::string& event) override {};
        void OnEnable() override {};
        void OnDisable() override {};
    };
}
