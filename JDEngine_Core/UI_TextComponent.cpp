#include "pch.h"
#include "framework.h"
#include "UI_TextComponent.h"

namespace JDComponent {

    void UI_TextComponent::Render(ID2D1DeviceContext7* context, D2D1_MATRIX_3X2_F viewTransform)
    {
        if (m_text.empty()) return;

        RectTransform* rectTransform = m_Owner ? m_Owner->GetComponent<RectTransform>() : nullptr;
        if (!rectTransform) return;

        if (!m_Owner || !m_Owner->IsActive())
            return;

        IDWriteTextFormat* textFormat = GetTextFormat();
        if (!textFormat) return;

        context->SetTransform(viewTransform);

        Vector2F pos = rectTransform->GetPosition();
        Vector2F size = rectTransform->GetSize();
        Vector2F pivot = rectTransform->GetPivot();
        Vector2F anchorOffset = { 0.f, 0.f };

        auto parent = rectTransform->GetParent();
        if (parent) {
            Vector2F parentSize = parent->GetSize();
            Vector2F anchor = rectTransform->GetAnchor();
            anchorOffset = {
                parentSize.x * anchor.x,
                parentSize.y * anchor.y
            };
        }

        Vector2F finalPos = {
            anchorOffset.x + pos.x,
            anchorOffset.y + pos.y
        };

        // 화면 중심 변환 제거! (RectTransform에서 이미 처리됨)
        // 단순히 finalPos 사용
        D2D1_RECT_F layoutRect = D2D1::RectF(
            finalPos.x - size.x * pivot.x,
            finalPos.y - size.y * pivot.y,
            finalPos.x + size.x * (1 - pivot.x),
            finalPos.y + size.y * (1 - pivot.y)
        );

        // 텍스트 브러시 생성
        ComPtr<ID2D1SolidColorBrush> brush;
        context->CreateSolidColorBrush(m_color, &brush);

        // 텍스트 렌더링
        context->DrawTextW(
            m_text.c_str(),
            static_cast<UINT32>(m_text.length()),
            textFormat,
            layoutRect,
            brush.Get()
        );
    }
}