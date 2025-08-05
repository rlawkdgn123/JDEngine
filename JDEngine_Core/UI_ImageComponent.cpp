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

    void UI_ImageComponent::Render(ID2D1DeviceContext7* context, D2D1_MATRIX_3X2_F renderTransform)
    {
        if (!m_Owner || !m_Owner->IsActive())
            return;

        RectTransform* rectTransform = m_Owner->GetComponent<RectTransform>();
        if (!rectTransform) return;

        ID2D1Bitmap* bitmap = AssetManager::Instance().GetTexture(m_textureName);
        if (!bitmap) return;

        // 1. 외부에서 전달받은 최종 변환 행렬을 설정합니다. (가장 중요!)
        // 이 행렬에는 위치, 회전, 크기, 피벗, 부모, 앵커 모든 정보가 들어있습니다.
        context->SetTransform(renderTransform);

        // 2. '로컬 공간' 기준의 사각형을 정의합니다.
        // 피벗은 이미 renderTransform에 적용되었으므로, (0,0)부터 그리면 됩니다.
        Vector2F size = rectTransform->GetSize();
        D2D1_RECT_F localRect = D2D1::RectF(0.f, 0.f, size.x, size.y);

        // 3. 비트맵을 로컬 사각형에 그립니다.
        // D2D가 설정된 변환 행렬(renderTransform)을 통해 알아서 올바른 위치에 그려줍니다.
        context->DrawBitmap(
            bitmap,
            &localRect,
            m_color.a,
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
            nullptr
        );
    }
}
