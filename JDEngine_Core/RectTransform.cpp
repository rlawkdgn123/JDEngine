#include "pch.h"
#include "Framework.h"
#include "D2DTransform.h"
#include "RectTransform.h"

namespace JDComponent {
    namespace D2DTM
    {
		using WindowSize = JDGlobal::Window::WindowSize;
		using Vector2F = JDMath::Vector2F;
        void RectTransform::UpdateMatrices()
        {

            std::cout << "m_scale = (" << m_scale.x << ", " << m_scale.y << ")\n";
            std::cout << "m_rotation = " << m_rotation << " degrees\n";
            std::cout << "m_position = (" << m_position.x << ", " << m_position.y << ")\n";
            std::cout << "m_pivot = (" << m_pivot.x << ", " << m_pivot.y << ")\n";

            // Unity 좌표계 : 좌측 하단이 원점, Y축 위쪽이 양수
            // 1) 피벗 기준 스케일
            auto M_scale = D2D1::Matrix3x2F::Scale(
                m_scale.x, m_scale.y,
                D2D1::Point2F(m_pivot.x, m_pivot.y)
            );
            // 2) 피벗 기준 회전 (반시계 방향이 양수)
            float centerX = (m_pivot.x - 0.5f) * m_size.x;
            float centerY = (0.5f - m_pivot.y) * m_size.y;
            std::cout << "center = (" << centerX << ", " << centerY << ")\n";

            auto M_rot = D2D1::Matrix3x2F::Rotation(
                m_rotation,
                D2D1::Point2F(centerX, centerY)
            );
            // 3) 최종 위치 이동
            auto M_trans = D2D1::Matrix3x2F::Translation(
                m_position.x, m_position.y
            );

            // 4) 로컬 매트릭스
            m_matrixLocal = M_scale * M_rot * M_trans;

            // 5) 월드 매트릭스
            if (m_parent)
            {
                m_matrixWorld = m_matrixLocal * m_parent->GetWorldMatrix();
            }
            else
            {
                // 여기서 화면 중심 기준 적용!
                m_matrixWorld = m_matrixLocal * GetScreenCenterTranslation();
            }
            m_dirty = false;
        }


        void RectTransform::SetPivot(const Vector2F& relativePivot)
        {
            // 값의 범위를 0~1로 제한 (선택사항)
            m_pivot.x = std::max(0.0f, std::min(1.0f, relativePivot.x));
            m_pivot.y = std::max(0.0f, std::min(1.0f, relativePivot.y));
        }

        void RectTransform::SetPivot(PivotPreset preset)
		{
            // 이 함수는 m_pivot을 0~1 사이의 상대 좌표로 설정합니다.
            switch (preset)
            {
            case PivotPreset::TopLeft:      m_pivot = { 0.0f, 1.0f }; break;
            case PivotPreset::TopCenter:    m_pivot = { 0.5f, 1.0f }; break;
            case PivotPreset::TopRight:     m_pivot = { 1.0f, 1.0f }; break;
            case PivotPreset::CenterLeft:   m_pivot = { 0.0f, 0.5f }; break;
            case PivotPreset::Center:       m_pivot = { 0.5f, 0.5f }; break;
            case PivotPreset::CenterRight:  m_pivot = { 1.0f, 0.5f }; break;
            case PivotPreset::BottomLeft:   m_pivot = { 0.0f, 0.0f }; break;
            case PivotPreset::BottomCenter: m_pivot = { 0.5f, 0.0f }; break;
            case PivotPreset::BottomRight:  m_pivot = { 1.0f, 0.0f }; break;
            }
		}

		

    }
}
