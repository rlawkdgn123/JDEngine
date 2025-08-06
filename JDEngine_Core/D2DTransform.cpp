#include "pch.h"
#include "Framework.h"
#include "D2DTransform.h"
#include "RectTransform.h"

namespace JDComponent {
	namespace D2DTM
	{
		// size를 받아 픽셀 단위 피벗을 설정하는 함수
		void Transform::SetPivotPreset(PivotPreset preset, const D2D1_SIZE_F& size)
		{
			// 유니티는 좌측 '하단'이 (0,0) 기준이므로 Y축 계산에 주의해야 합니다.
			switch (preset)
			{
			case PivotPreset::TopLeft:      m_pivot = { 0.0f, size.height }; break;
			case PivotPreset::TopCenter:    m_pivot = { size.width * 0.5f, size.height }; break;
			case PivotPreset::TopRight:     m_pivot = { size.width, size.height }; break;
			case PivotPreset::CenterLeft:   m_pivot = { 0.0f, size.height * 0.5f }; break;
			case PivotPreset::Center:       m_pivot = { size.width * 0.5f, size.height * 0.5f }; break;
			case PivotPreset::CenterRight:  m_pivot = { size.width, size.height * 0.5f }; break;
			case PivotPreset::BottomLeft:   m_pivot = { 0.0f, 0.0f }; break;
			case PivotPreset::BottomCenter: m_pivot = { size.width * 0.5f, 0.0f }; break;
			case PivotPreset::BottomRight:  m_pivot = { size.width, 0.0f }; break;
			}
		}

		// LEGACY : 과거코드 위의 함수로 대체
		/*
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
		 
		void Transform::SetPivotPreset(PivotPreset preset)
		{
			switch (preset)
			{
			case PivotPreset::TopLeft:
				m_pivot = { 0.0f, 1.0f };
				break;
			case PivotPreset::TopCenter:
				m_pivot = { 0.5f, 1.0f };
				break;
			case PivotPreset::TopRight:
				m_pivot = { 1.0f, 1.0f };
				break;
			case PivotPreset::CenterLeft:
				m_pivot = { 0.0f, 0.5f };
				break;
			case PivotPreset::Center:
				m_pivot = { 0.5f, 0.5f };
				break;
			case PivotPreset::CenterRight:
				m_pivot = { 1.0f, 0.5f };
				break;
			case PivotPreset::BottomLeft:
				m_pivot = { 0.0f, 0.0f };
				break;
			case PivotPreset::BottomCenter:
				m_pivot = { 0.5f, 0.0f };
				break;
			case PivotPreset::BottomRight:
				m_pivot = { 1.0f, 0.0f };
				break;
			}

			SetDirty();
		}
		*/

		
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

		// 코드 간소화 - 피벗 이동을 매트릭스에 포함시킴
		void Transform::UpdateMatrices()
		{
			// 1. 피벗의 위치를 원점으로 옮기는 행렬
			//    (Scale과 Rotation을 피벗 기준으로 적용하기 위함)
			const auto P = D2D1::Matrix3x2F::Translation(-m_pivot.x, -m_pivot.y);

			// 2. 크기 및 회전 행렬
			const auto S = D2D1::Matrix3x2F::Scale(m_scale.x, m_scale.y);
			const auto R = D2D1::Matrix3x2F::Rotation(m_rotation);

			// 3. 최종 위치 이동 행렬
			//    'Y 위쪽이 양수'인 유니티 좌표계를 위해 m_position.y 부호를 뒤집습니다.
			const auto T = D2D1::Matrix3x2F::Translation(m_position.x, -m_position.y);

			// 4. 로컬 행렬 계산: P-SR-T 순서 (더 직관적인 방식)
			//    연산 순서: 피벗을 원점으로 -> 스케일 -> 회전 -> '피벗'을 최종 위치로 이동
			//    P_inv(피벗 원위치)를 제거하고 T(최종 위치)를 마지막에 곱합니다.
			m_matrixLocal = P * S * R * T;

			if (m_parent)
			{
				m_matrixWorld = m_matrixLocal * m_parent->GetWorldMatrix();
			}
			else
			{
				m_matrixWorld = m_matrixLocal;
			}
			m_dirty = false;
		}
	}
}

