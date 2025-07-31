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
			// Unity 좌표계 : 좌측 하단이 원점, Y축 위쪽이 양수

			// 1) 피벗 기준 스케일
			auto M_scale = D2D1::Matrix3x2F::Scale(
				m_scale.x, m_scale.y,
				D2D1::Point2F(m_pivot.x, m_pivot.y)
			);

			// 2) 피벗 기준 회전	( 반시계 방향이 양수 )
			auto M_rot = D2D1::Matrix3x2F::Rotation(
				m_rotation,
				D2D1::Point2F(m_pivot.x, m_pivot.y)
			);

			// 3) 최종 위치 이동
			auto M_trans = D2D1::Matrix3x2F::Translation(
				m_position.x, m_position.y
			);

			// 합쳐서 로컬 변환 매트릭스 완성
			m_matrixLocal = M_scale * M_rot * M_trans;

			if (m_parent)
				m_matrixWorld = m_matrixLocal * m_parent->GetWorldMatrix();
			else
				m_matrixWorld = m_matrixLocal;

			m_dirty = false;

		}

		void RectTransform::SetPivotPreset(PivotPreset preset, const D2D1_SIZE_F& size)
		{
			// Unity 좌표계에 맞게 피벗 설정 (Y축 위쪽이 양수)

			switch (preset)
			{
			case PivotPreset::TopLeft:
				m_pivot = { 0.0f, size.height };  // 위쪽이 양수
				break;
			case PivotPreset::TopRight:
				m_pivot = { size.width, size.height };
				break;
			case PivotPreset::BottomLeft:
				m_pivot = { 0.0f, 0.0f };  // 하단 기준점
				break;
			case PivotPreset::BottomRight:
				m_pivot = { size.width, 0.0f };
				break;
			case PivotPreset::Center:
				m_pivot = { size.width * 0.5f, size.height * 0.5f };
				break;
			}
		}

		

    }
}
