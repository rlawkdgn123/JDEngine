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
        //if (!m_image) return;

        //RectTransform* rectTransform = m_Owner ? m_Owner->GetComponent<RectTransform>() : nullptr;
        //if (!rectTransform) return;

        //////////////////////////////////////////////////////////////////////////////////

        //// 1. 현재 Transform (카메라 + 렌더TM) 저장
        //D2D1::Matrix3x2F currentTransform;
        //ctx->GetTransform(&currentTransform);

        //// 2. 이 오브젝트 전용 rectTransform 계산
        //Vector2F pos = rectTransform->GetPosition();
        //Vector2F scale = rectTransform->GetScale();
        //float rot = rectTransform->GetRotation();

        //D2D1::Matrix3x2F objectTM =
        //    D2D1::Matrix3x2F::Scale(scale.x, scale.y) *
        //    D2D1::Matrix3x2F::Rotation(rot) *
        //    D2D1::Matrix3x2F::Translation(pos.x, pos.y);

        //// 3. 전체 변환 = 현재 설정된 
        //D2D1::Matrix3x2F finalTM = objectTM * currentTransform;

        //// 4. 설정
        //ctx->SetTransform(finalTM);

        //////////////////////////////////////////////////////////////////////////////////

        //// 이미지 크기
        //D2D1_SIZE_F imageSize = m_image->GetSize();

        //// 출력 사각형 계산
        //D2D1_RECT_F destRect = {
        //    0.f, 0.f,
        //    imageSize.width,
        //    imageSize.height
        //};

        //ctx->DrawBitmap(m_image.Get(), destRect);

        // 1. 렌더링 시점에 리소스 매니저에서 텍스처를 가져옵니다.
        ID2D1Bitmap* bitmap = ResourceManager::Instance().GetTexture(m_textureName);

        // 2. 텍스처가 없으면 아무것도 하지 않고 종료 (안전장치)
        if (!bitmap) return;

        // 3. 외부에서 전달받은 변환 행렬을 설정합니다.
        context->SetTransform(viewTransform);

        // 4. RectTransform에서 크기를 가져와 그리기 영역을 설정합니다.
        RectTransform* rectTransform = m_Owner ? m_Owner->GetComponent<RectTransform>() : nullptr;
        if (!rectTransform) return;
        Vector2F size = rectTransform->GetSize();

        D2D1_RECT_F destRect = D2D1::RectF(0.f, 0.f, size.x, size.y);

        // 5. 비트맵을 그립니다.
        context->DrawBitmap(bitmap, &destRect);
    }
}
