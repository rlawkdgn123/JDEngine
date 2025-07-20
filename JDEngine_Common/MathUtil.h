#pragma once
#include <D2D1Helper.h>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

namespace JDGlobal {
	namespace Math {

		// D2D1_POINT_2F
		constexpr float DegreeToRadian(float deg)
		{
			return static_cast<float>(deg * (M_PI / 180.0f));
		}
		constexpr float RadianToDegree(float rad)
		{
			return static_cast<float>(rad * (180.0f / M_PI));
		}

		inline float Clamp(float value, float min, float max)
		{
			if (value < min) return min;
			if (value > max) return max;
			return value;
		}

		inline int IsLeft(D2D1_POINT_2F P0, D2D1_POINT_2F P1, D2D1_POINT_2F P2)
		{
			return static_cast<int>(((P1.x - P0.x) * (P2.y - P0.y) - (P2.x - P0.x) * (P1.y - P0.y)));
		}

		int cn_PnPoly(D2D1_POINT_2F P, std::vector<D2D1_POINT_2F> V, int n);

		int wn_PnPoly(D2D1_POINT_2F P, std::vector<D2D1_POINT_2F> V, int n);


		// D2D1_MATRIX_3X2_F : D2D ����� ����ص� ������, ���� �� �ٽ� �� �� ����� ���Ҵ�.
		D2D1_MATRIX_3X2_F MakeTranslationMatrix(D2D1_SIZE_F size);		// �̵�
		D2D1_MATRIX_3X2_F MakeRotationMatrix_Origin(FLOAT angle);		// ���������� ȸ��, ������
		D2D1_MATRIX_3X2_F MakeScaleMatrix_Origin(D2D1_SIZE_F size);

		// Ư�� ���� �������� �� ȸ��
		D2D1_MATRIX_3X2_F MakeRotationMatrix(FLOAT angle, D2D1_POINT_2F center = D2D1::Point2F());

		// Ư�� ���� �������� �� ũ�� ��ȯ
		D2D1_MATRIX_3X2_F MakeScaleMatrix(D2D1_SIZE_F size, D2D1_POINT_2F center = D2D1::Point2F());

		D2D1::Matrix3x2F MakeRenderMatrix(bool bUnityCoords = false, bool bMirror = false, float offsetX = 0, float offsetY = 0);


		// ����� ����� ���� TM ���ڿ�
		// �ڵ� ���·� �Է��ϸ� �� ¤�� ���� �� ������, ���������� �� �켱 ��İ����� �����ϸ� ����
		static void MakeMatrixToString(
			const D2D1_MATRIX_3X2_F& matrix,
			__out_ecount(bufSize) wchar_t* buffer,
			size_t bufSize
		);

		void DecomposeMatrix3x2
		(const D2D1::Matrix3x2F& M, Vector2F& outTranslation, float& outRotation, Vector2F& outScale);

		D2D1::Matrix3x2F RemovePivot(const D2D1::Matrix3x2F& M_local, const D2D1_POINT_2F& pivot);

		/// �簢�� rect �ȿ� point �� ���ԵǾ� ������ true ��ȯ
		/// (��� ���� �˻�)
		bool IsPointInRect(const D2D1_POINT_2F& point, const D2D1_RECT_F& rect) noexcept;
	}

}