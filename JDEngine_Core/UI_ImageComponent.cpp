#include "pch.h"
#include "framework.h"
#include "UI_ImageComponent.h"


namespace JDComponent {

    void UI_ImageComponent::SetColor(const D2D1_COLOR_F& color)
    {
        m_color = color;
    }

    void UI_ImageComponent::Render(ID2D1DeviceContext7* context, D2D1_MATRIX_3X2_F viewTransform)
    {
        // 1. 렌더링 시점에 리소스 매니저에서 텍스처를 가져옵니다.
        ID2D1Bitmap* bitmap = AssetManager::Instance().GetTexture(m_textureName);

        // 2. 텍스처가 없으면 아무것도 하지 않고 종료 (안전장치)
        if (!bitmap) return;

        // 3. 외부에서 전달받은 변환 행렬을 설정합니다.
        context->SetTransform(viewTransform);

        // 4. RectTransform에서 크기를 가져와 그리기 영역을 설정합니다.
        RectTransform* rectTransform = m_Owner ? m_Owner->GetComponent<RectTransform>() : nullptr;
        if (!rectTransform) return;

        Vector2F pos = rectTransform->GetPosition();
        Vector2F size = rectTransform->GetSize();

        D2D1_RECT_F destRect = D2D1::RectF(
            pos.x,
            pos.y,
            pos.x + size.x,
            pos.y + size.y
        );

        // 5. 비트맵을 그립니다.
        context->DrawBitmap(bitmap, &destRect);
    }
}
