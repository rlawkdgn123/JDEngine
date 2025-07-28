#include "pch.h"
#include "framework.h"
#include "RectTransform.h"

namespace JDComponent {
    namespace D2DTM
    {
		using WindowSize = JDGlobal::Window::WindowSize;

        void RectTransform::UpdateMatrices()
		{
			// 부모가 있다면 부모의 RectSize가 기준이 됨
			// 부모가 없다면 윈도우 사이즈가 기준이 됨
			D2D1_SIZE_F parentSize =
				m_parent ? D2D1_SIZE_F{ m_parent->GetSize().x, m_parent->GetSize().y }
			: WindowSize::Instance().GetSize();

			auto anchorOffset = SetAnchorOffset(parentSize);

			auto finalPosition = anchorOffset + m_position; // 앵커 오프셋 + 위치값

			// 1) 피벗 기준 스케일
			auto M_scale = D2D1::Matrix3x2F::Scale(
				m_scale.x, m_scale.y,
				D2D1::Point2F(m_pivot.x, m_pivot.y)
			);

			// 2) 피벗 기준 회전
			auto M_rot = D2D1::Matrix3x2F::Rotation(
				m_rotation,
				D2D1::Point2F(m_pivot.x, m_pivot.y)
			);

			// 3) 위치 이동
			auto M_trans = D2D1::Matrix3x2F::Translation(
				finalPosition.x, finalPosition.y
			);

			// 4) 로컬 행렬 = 스케일 * 회전 * 이동
			m_matrixLocal = M_scale * M_rot * M_trans;

			// 5) 부모가 있으면 월드 매트릭스 계산
			if (m_parent)
				m_matrixWorld = m_matrixLocal * m_parent->GetWorldMatrix();
			else
				m_matrixWorld = m_matrixLocal;

			m_dirty = false;
		}

		void RectTransform::SetPivotPreset(PivotPreset preset, const D2D1_SIZE_F& size)
		{
			switch (preset)
			{
			case PivotPreset::TopLeft:
				m_pivot = { 0.0f, 0.0f };
				break;
			case PivotPreset::TopRight:
				m_pivot = { size.width, 0.0f };
				break;
			case PivotPreset::BottomLeft:
				m_pivot = { 0.0f, -size.height };
				break;
			case PivotPreset::BottomRight:
				m_pivot = { size.width, -size.height };
				break;
			case PivotPreset::Center:
				m_pivot = { size.width * 0.5f, -(size.height * 0.5f) };

				break;
			}
		}

		JDMath::Vector2F RectTransform::SetAnchorOffset(const D2D1_SIZE_F& parentSize)
		{
			{
				switch (m_anchor)
				{
				case AnchorPreset::TopLeft:
					return { 0.f, 0.f };
				case AnchorPreset::TopCenter:
					return { parentSize.width * 0.5f, 0.f };
				case AnchorPreset::TopRight:
					return { parentSize.width, 0.f };
				case AnchorPreset::CenterLeft:
					return { 0.f, parentSize.height * 0.5f };
				case AnchorPreset::Center:
					return { parentSize.width * 0.5f, parentSize.height * 0.5f };
				case AnchorPreset::CenterRight:
					return { parentSize.width, parentSize.height * 0.5f };
				case AnchorPreset::BottomLeft:
					return { 0.f, parentSize.height };
				case AnchorPreset::BottomCenter:
					return { parentSize.width * 0.5f, parentSize.height };
				case AnchorPreset::BottomRight:
					return { parentSize.width, parentSize.height };
				case AnchorPreset::XStretch:
					return { parentSize.width * 0.5f, 0.f };
				case AnchorPreset::YStretch:
					return { 0.f, parentSize.height * 0.5f };
				default:
					return { 0.f, 0.f };
				}
			}
		}
    }
}
