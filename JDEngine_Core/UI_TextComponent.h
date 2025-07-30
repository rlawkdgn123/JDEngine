#pragma once
#include "GameObjectBase.h"
#include "RectTransform.h"
#include <dwrite.h>

namespace JDComponent {

    using RectTransform = D2DTM::RectTransform;
    using Vector2F = JDMath::Vector2F;

    template<typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    class UI_TextComponent : public Component
    {
    public:
        void Render(ID2D1DeviceContext7* ctx);

        void                SetText(const std::wstring text) { m_text = text; }
        const std::wstring  GetText() const { return m_text; }

        void                SetColor(const D2D1_COLOR_F& color) { m_color = color; }
        D2D1_COLOR_F        GetColor() { return m_color; }

        void                SetTextFormat(IDWriteTextFormat* format) { m_textFormat = format; }

    private:
        std::wstring m_text = L"text";
        D2D1_COLOR_F m_color = D2D1::ColorF(D2D1::ColorF::Black);
        ComPtr<IDWriteTextFormat> m_textFormat;

        // Component을(를) 통해 상속됨 [ 빈 함수 ]
        void Update(float deltaTime) override {};
        void OnEvent(const std::string& event) override {};
        void OnEnable() override {};
        void OnDisable() override {};
    };

}

