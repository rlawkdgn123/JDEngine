#include "pch.h"
#include "framework.h"
#include "RectTransform.h"

namespace JDComponent {
    namespace D2DTM
    {
		using WindowSize = JDGlobal::Window::WindowSize;
		using Vector2F = JDMath::Vector2F;
        void RectTransform::UpdateMatrices()
		{
			// �θ� �ִٸ� �θ��� RectSize�� ������ ��
			// �θ� ���ٸ� ������ ����� ������ ��
			D2D1_SIZE_F baseSize = { 0, 0 };

			if (m_parent)
				baseSize = m_parent->GetD2D1Size();
			else
				baseSize = WindowSize::Instance().GetSize(); // ��ü ȭ��

			// 2. ��Ŀ ���� ��ġ�� ���� ��ǥ�迡�� ���
			Vector2F anchorOffset = SetAnchorOffset(baseSize); // �θ� ������ ���� ��Ŀ ��ġ

			// 3. ������ (Pivot ����)
			auto M_scale = D2D1::Matrix3x2F::Scale(
				m_scale.x, m_scale.y,
				m_pivot
			);
			// 4. ȸ�� (Pivot ����)
			auto M_rot = D2D1::Matrix3x2F::Rotation(
				m_rotation,
				m_pivot
			);

			// 5. ���� ��ġ = ��Ŀ ��ġ + ���� ������ (Position)
			auto finalPos = anchorOffset + m_position;

			auto M_trans = D2D1::Matrix3x2F::Translation(
				finalPos.x, finalPos.y
			);

			// 6. ���� ��� = Scale �� Rotate �� Translate
			m_matrixLocal = M_scale * M_rot * M_trans;

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
