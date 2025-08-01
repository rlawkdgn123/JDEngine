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

		template <typename T>
		inline T Clamp(const T& value, const T min, const T max)
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


		// D2D1_MATRIX_3X2_F : D2D 기능을 사용해도 되지만, 연습 겸 다시 한 번 만들어 보았다.
		D2D1_MATRIX_3X2_F MakeTranslationMatrix(D2D1_SIZE_F size);		// 이동
		D2D1_MATRIX_3X2_F MakeRotationMatrix_Origin(FLOAT angle);		// 원점에서의 회전, 스케일
		D2D1_MATRIX_3X2_F MakeScaleMatrix_Origin(D2D1_SIZE_F size);

		// 특정 점을 기준으로 한 회전
		D2D1_MATRIX_3X2_F MakeRotationMatrix(FLOAT angle, D2D1_POINT_2F center = D2D1::Point2F());

		// 특정 점을 기준으로 한 크기 변환
		D2D1_MATRIX_3X2_F MakeScaleMatrix(D2D1_SIZE_F size, D2D1_POINT_2F center = D2D1::Point2F());

		D2D1::Matrix3x2F MakeRenderMatrix(bool bUnityCoords = false, bool bMirror = false, float offsetX = 0, float offsetY = 0);


		// 디버그 출력을 위한 TM 문자열
		// 코드 형태로 입력하며 딱 짚어 말할 순 없으나, 수학적으로 행 우선 행렬곱으로 이해하면 쉽다
		static void MakeMatrixToString(
			const D2D1_MATRIX_3X2_F& matrix,
			__out_ecount(bufSize) wchar_t* buffer,
			size_t bufSize
		);

		void DecomposeMatrix3x2
		(const D2D1::Matrix3x2F& M, Vector2F& outTranslation, float& outRotation, Vector2F& outScale);

		D2D1::Matrix3x2F RemovePivot(const D2D1::Matrix3x2F& M_local, const D2D1_POINT_2F& pivot);

		/// 사각형 rect 안에 point 가 포함되어 있으면 true 반환
		/// (경계 포함 검사)
		bool IsPointInRect(const D2D1_POINT_2F& point, const D2D1_RECT_F& rect) noexcept;
	}

}