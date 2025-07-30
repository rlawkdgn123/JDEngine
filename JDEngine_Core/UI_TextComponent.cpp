#include "pch.h"
#include "framework.h"
#include "UI_TextComponent.h"

namespace JDComponent {

    void UI_TextComponent::Render(ID2D1DeviceContext7* ctx)
    {
        if (m_text.empty() || !m_textFormat) return;

        RectTransform* rectTransform = m_Owner ? m_Owner->GetComponent<RectTransform>() : nullptr;
        if (!rectTransform) return;

        ////////////////////////////////////////////////////////////////////////////////

        // 1. 현재 Transform (카메라 + 렌더TM) 저장
        D2D1::Matrix3x2F currentTransform;
        ctx->GetTransform(&currentTransform);

        // 2. 이 오브젝트 전용 rectTransform 계산
        Vector2F pos = rectTransform->GetPosition();
        Vector2F scale = rectTransform->GetScale();
        float rot = rectTransform->GetRotation();

        D2D1::Matrix3x2F objectTM =
            D2D1::Matrix3x2F::Scale(scale.x, scale.y) *
            D2D1::Matrix3x2F::Rotation(rot) *
            D2D1::Matrix3x2F::Translation(pos.x, pos.y);

        // 3. 전체 변환 = 현재 설정된 
        D2D1::Matrix3x2F finalTM = objectTM * currentTransform;

        // 4. 설정
        ctx->SetTransform(finalTM);

        ////////////////////////////////////////////////////////////////////////////////

        // 텍스트 출력 영역 ( 임시 )
        D2D1_RECT_F layoutRect = {
            0, 0,
            300, 100
        };

        // 텍스트 브러시 생성 (임시)
        ComPtr<ID2D1SolidColorBrush> brush;
        ctx->CreateSolidColorBrush(m_color, &brush);

        ctx->DrawTextW(
            m_text.c_str(),
            static_cast<UINT32>(m_text.length()),
            m_textFormat.Get(),
            layoutRect,
            brush.Get()
        );
    }
}