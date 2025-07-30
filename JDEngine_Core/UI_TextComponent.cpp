#include "pch.h"
#include "framework.h"
#include "UI_TextComponent.h"

namespace JDComponent {

    void UI_TextComponent::Render(ID2D1DeviceContext7* context, D2D1_MATRIX_3X2_F viewTransform)
    {
        if (m_text.empty() || !m_textFormat) return;

        RectTransform* rectTransform = m_Owner ? m_Owner->GetComponent<RectTransform>() : nullptr;
        if (!rectTransform) return;

        context->SetTransform(viewTransform);

        Vector2F size = rectTransform->GetSize();

        D2D1_RECT_F layoutRect = {
            0, 0,
            size.x, size.y
        };

        // 텍스트 브러시 생성 (임시)
        ComPtr<ID2D1SolidColorBrush> brush;
        context->CreateSolidColorBrush(m_color, &brush);

        context->DrawTextW(
            m_text.c_str(),
            static_cast<UINT32>(m_text.length()),
            m_textFormat.Get(),
            layoutRect,
            brush.Get()
        );
    }
}