#include "pch.h"
#include "framework.h"
#include "UI_TextComponent.h"

namespace JDComponent {

    void UI_TextComponent::Render(ID2D1DeviceContext7* context, D2D1_MATRIX_3X2_F renderTransform)
    {
        if (m_text.empty()) return;

        RectTransform* rectTransform = m_Owner ? m_Owner->GetComponent<RectTransform>() : nullptr;
        if (!rectTransform) return;

        if (!m_Owner || !m_Owner->IsActive())
            return;

        IDWriteTextFormat* textFormat = GetTextFormat();
        if (!textFormat) return;

        // 1. 외부에서 전달받은 최종 변환 행렬을 설정합니다.
        // 이 행렬에 위치, 회전, 스케일, 피벗 등 모든 정보가 담겨있습니다.
        context->SetTransform(renderTransform);

        // 2. '로컬 공간' 기준의 텍스트 레이아웃 사각형을 정의합니다.
        // 피벗은 이미 renderTransform에 적용되었으므로, (0,0)부터 그리면 됩니다.
        Vector2F size = rectTransform->GetSize();
        D2D1_RECT_F localLayoutRect = D2D1::RectF(0.f, 0.f, size.x, size.y);

        // 3. 텍스트 브러시를 생성하고 텍스트를 로컬 사각형에 그립니다.
        ComPtr<ID2D1SolidColorBrush> brush;
        HRESULT hr = context->CreateSolidColorBrush(m_color, &brush);
        if (SUCCEEDED(hr))
        {
            context->DrawTextW(
                m_text.c_str(),
                static_cast<UINT32>(m_text.length()),
                textFormat,
                localLayoutRect, // 로컬 레이아웃 사각형 사용
                brush.Get()
            );
        }
    }
}