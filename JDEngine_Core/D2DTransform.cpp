#include "pch.h"
#include "Framework.h"
#include "D2DTransform.h"
#include "RectTransform.h"

namespace JDComponent {
	namespace D2DTM
	{
		void Transform::SetPivotPreset(PivotPreset preset, const D2D1_SIZE_F& size)
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

		
		/*void Transform::UpdateMatrices()
		{
			const auto P = D2D1::Matrix3x2F::Translation(-m_pivot.x, -m_pivot.y);

			const auto S = D2D1::Matrix3x2F::Scale(m_scale.x, m_scale.y);

			const auto R = D2D1::Matrix3x2F::Rotation(m_rotation);

			const auto T1 = D2D1::Matrix3x2F::Translation(m_pivot.x, m_pivot.y);

			const auto T2 = D2D1::Matrix3x2F::Translation(m_position.x, m_position.y);

			m_matrixLocal = P * S * R * T1 * T2;

			if (m_parent)
				m_matrixWorld = m_matrixLocal * m_parent->GetWorldMatrix();
			else
				m_matrixWorld = m_matrixLocal;

			m_dirty = false;
		}*/

		// �ڵ� ����ȭ - �ǹ� �̵��� ��Ʈ������ ���Խ�Ŵ
		void Transform::UpdateMatrices()
		{
			// 1) �ǹ� ���� ������
			auto M_scale = D2D1::Matrix3x2F::Scale(
				m_scale.x, m_scale.y,
				D2D1::Point2F(m_pivot.x, m_pivot.y)
			);

			// 2) �ǹ� ���� ȸ��
			auto M_rot = D2D1::Matrix3x2F::Rotation(
				m_rotation,
				D2D1::Point2F(m_pivot.x, m_pivot.y)
			);

			// 3) ���� ��ġ �̵�
			auto M_trans = D2D1::Matrix3x2F::Translation(
				m_position.x, m_position.y
			);

			// ���ļ� ���� ��ȯ ��Ʈ���� �ϼ�
			m_matrixLocal = M_scale * M_rot * M_trans;

			if (m_parent)
				m_matrixWorld = m_matrixLocal * m_parent->GetWorldMatrix();
			else
				m_matrixWorld = m_matrixLocal;

			m_dirty = false;
		}
	}
}

