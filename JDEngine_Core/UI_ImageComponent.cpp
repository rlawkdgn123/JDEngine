#include "pch.h"
#include "framework.h"
#include "UI_ImageComponent.h"


namespace JDComponent {

    void UI_ImageComponent::SetColor(const D2D1_COLOR_F& color)
    {
        m_color = color;
    }

    void UI_ImageComponent::SetSizeToOriginal()
    {
        if (!m_textureName.empty()) {
            ID2D1Bitmap* bitmap = AssetManager::Instance().GetTexture(m_textureName);
            if (!bitmap)
                return;

            auto size = bitmap->GetSize(); // D2D1_SIZE_F

            auto rectTransform = GetRectTransform();
            if (rectTransform)
                rectTransform->SetSize(Vector2F{ size.width, size.height });
        }
    }

    Vector2F UI_ImageComponent::GetOriginalSize() const
    {
        if (!m_textureName.empty()) {
            ID2D1Bitmap* bitmap = AssetManager::Instance().GetTexture(m_textureName);
            if (!bitmap)
                return Vector2F{};

            auto size = bitmap->GetSize(); // D2D1_SIZE_F
            return Vector2F{ size.width, size.height };
        }
        return Vector2F{};
    }

    void UI_ImageComponent::Render(ID2D1DeviceContext7* context, D2D1_MATRIX_3X2_F viewTransform)
    {
        // RectTransform에서 크기를 가져와 그리기 영역을 설정합니다.
        RectTransform* rectTransform = m_Owner ? m_Owner->GetComponent<RectTransform>() : nullptr;
        if (!rectTransform) return;

        if (!m_Owner || !m_Owner->IsActive())
            return;

        // 렌더링 시점에 리소스 매니저에서 텍스처를 가져옵니다.
        ID2D1Bitmap* bitmap = AssetManager::Instance().GetTexture(m_textureName);

        // 텍스처가 없으면 아무것도 하지 않고 종료 (안전장치)
        if (!bitmap) return;

        // 외부에서 전달받은 변환 행렬을 설정합니다.
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

        // 좌표 계산
        D2D1_RECT_F destRect = D2D1::RectF(
            finalPos.x - size.x * pivot.x,
            finalPos.y - size.y * pivot.y,
            finalPos.x + size.x * (1 - pivot.x),
            finalPos.y + size.y * (1 - pivot.y)
        );

        // 비트맵을 그립니다.
        context->DrawBitmap(
            bitmap,
            &destRect,
            m_color.a, // 불투명도 (0.0f ~ 1.0f)
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
            nullptr    // 원본 bitmap 영역 (전체 사용 시 nullptr)
        );
    }
}
