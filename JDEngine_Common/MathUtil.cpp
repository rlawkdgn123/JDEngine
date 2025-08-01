#include "pch.h"
#include "framework.h"
#include "MathType.h"
#include "MathUtil.h"



namespace JDGlobal {
    namespace Math {
        // D2D1_POINT_2F

        int cn_PnPoly(D2D1_POINT_2F P, std::vector<D2D1_POINT_2F> V, int n)
        {
            int    cn = 0;    // the  crossing number counter

            // loop through all edges of the polygon
            for (int i = 0; i < n; i++) {    // edge from V[i]  to V[i+1]
                if (((V[i].y <= P.y) && (V[i + 1].y > P.y))     // an upward crossing
                    || ((V[i].y > P.y) && (V[i + 1].y <= P.y))) { // a downward crossing
                    // compute  the actual edge-ray intersect x-coordinate
                    float vt = (float)(P.y - V[i].y) / (V[i + 1].y - V[i].y);
                    if (P.x < V[i].x + vt * (V[i + 1].x - V[i].x)) // P.x < intersect
                        ++cn;   // a valid crossing of y=P.y right of P.x
                }
            }

            printf("cn = %d\n", cn);

            return (cn & 1);    // 0 if even (out), and 1 if  odd (in)
        }

        int wn_PnPoly(D2D1_POINT_2F P, std::vector<D2D1_POINT_2F> V, int n)
        {
            int    wn = 0;    // the  winding number counter

            // loop through all edges of the polygon
            for (int i = 0; i < n; i++) {   // edge from V[i] to  V[i+1]
                if (V[i].y <= P.y) {          // start y <= P.y
                    if (V[i + 1].y > P.y)      // an upward crossing
                        if (IsLeft(V[i], V[i + 1], P) > 0)  // P left of  edge
                            ++wn;            // have  a valid up intersect
                }
                else {                        // start y > P.y (no test needed)
                    if (V[i + 1].y <= P.y)     // a downward crossing
                        if (IsLeft(V[i], V[i + 1], P) < 0)  // P right of  edge
                            --wn;            // have  a valid down intersect
                }
            }
            return wn;
        }

        bool is_circum(Triangle cur, int i, std::vector<Vector2F>& point)
        {
            // 외접원안에 점이 들어오는지 확인
            float ccw = (point[cur.b] - point[cur.a]).Cross(point[cur.c] - point[cur.a]);

            float adx = point[cur.a].x - point[i].x;
            float ady = point[cur.a].y - point[i].y;
            float bdx = point[cur.b].x - point[i].x;
            float bdy = point[cur.b].y - point[i].y;
            float cdx = point[cur.c].x - point[i].x;
            float cdy = point[cur.c].y - point[i].y;
            float bdxcdy = bdx * cdy, cdxbdy = cdx * bdy;
            float cdxady = cdx * ady, adxcdy = adx * cdy;
            float adxbdy = adx * bdy, bdxady = bdx * ady;
            float alift = adx * adx + ady * ady;
            float blift = bdx * bdx + bdy * bdy;
            float clift = cdx * cdx + cdy * cdy;


            float det = alift * (bdxcdy - cdxbdy)
                + blift * (cdxady - adxcdy)
                + clift * (adxbdy - bdxady);

            if (ccw > 0) return det >= 0;
            else return det <= 0;
        }

        //D2D1_MATRIX_3X2_F

        // 이동 행렬
        D2D1_MATRIX_3X2_F MakeTranslationMatrix(D2D1_SIZE_F size) {
            D2D1_MATRIX_3X2_F _translation;

            _translation._11 = 1.0f; _translation._12 = 0.0f;
            _translation._21 = 0.0f; _translation._22 = 1.0f;
            _translation._31 = size.width; _translation._32 = size.height;

            return _translation;
        }

        // 회전 행렬
        D2D1_MATRIX_3X2_F MakeRotationMatrix_Origin(FLOAT angle)
        {
            float _rad = DegreeToRadian(angle);
            D2D1_MATRIX_3X2_F _rotation;

            //sin세타 만큼 반시계 방향 회전
            _rotation._11 = cos(_rad); _rotation._21 = -sin(_rad); _rotation._31 = 0;
            _rotation._12 = sin(_rad); _rotation._22 = cos(_rad); _rotation._32 = 0;

            return _rotation;
        }

        // 크기 행렬
        D2D1_MATRIX_3X2_F MakeScaleMatrix_Origin(D2D1_SIZE_F size)
        {
            D2D1_MATRIX_3X2_F _scale;

            //1.0f를 곱해주는 이유 : 명시적 캐스팅 및 의도 표현
            _scale._11 = size.width * 1.0f; _scale._12 = 0.0f;
            _scale._21 = 0.0f; _scale._22 = size.height * 1.0f;
            _scale._31 = 0.0f; _scale._32 = 0.0f;

            return _scale;
        }

        // 회전 행렬 변환
        D2D1_MATRIX_3X2_F MakeRotationMatrix(FLOAT angle, D2D1_POINT_2F center /*= D2D1::Point2F()*/)
        {
            // 1.중심점 center 만큼 좌표계를 원점으로 이동 
            // 2.원점 기준으로 회전 (기본 회전 행렬)
            // 3.원래 위치(중심점)로 좌표계를 다시 이동
            D2D1_MATRIX_3X2_F _translateToOrigin = MakeTranslationMatrix(D2D1::Size(-center.x, -center.y));
            D2D1_MATRIX_3X2_F _rotateOnOrigin = MakeRotationMatrix_Origin(angle);
            D2D1_MATRIX_3X2_F _translateToCenter = MakeTranslationMatrix(D2D1::Size(center.x, center.y));

            D2D1_MATRIX_3X2_F _resultTM = _translateToOrigin * _rotateOnOrigin * _translateToCenter;

            return _resultTM;
        }

        // 크기 행렬 변환
        D2D1_MATRIX_3X2_F MakeScaleMatrix(D2D1_SIZE_F size, D2D1_POINT_2F center /*= D2D1::Point2F()*/)
        {
            // 1.중심점 center 만큼 좌표계를 원점으로 이동 
            // 2.원점 기준으로 크기 변환 (기본 크기 행렬)
            // 3.원래 위치(중심점)로 좌표계를 다시 이동
            D2D1_MATRIX_3X2_F _translateToOrigin = MakeTranslationMatrix(D2D1::Size(-center.x, -center.y));
            D2D1_MATRIX_3X2_F _scaleOnOrigin = MakeScaleMatrix_Origin(size);
            D2D1_MATRIX_3X2_F _translateToCenter = MakeTranslationMatrix(D2D1::Size(center.x, center.y));

            D2D1_MATRIX_3X2_F _resultTM = _translateToOrigin * _scaleOnOrigin * _translateToCenter;

            return _resultTM;
        }

        /// <summary>
        /// 가상 월드 좌표계의 정점을 스크린 좌표계로 변환하는 변환 행렬 생성
        /// </summary>
        /// <param name="bUnityCoords">유니티 좌표계 사용 여부 (Y축 상하 반전 적용)</param>
        /// <param name="bMirror">좌우 반전 여부 (X축 반전 적용)</param>
        /// <param name="offsetX">X축 방향 좌표 보정값</param>
        /// <param name="offsetY">Y축 방향 좌표 보정값</param>
        /// <returns>변환 행렬 (스케일과 평행 이동이 합쳐진 형태)</returns>
        D2D1::Matrix3x2F MakeRenderMatrix(bool bUnityCoords, bool bMirror, float offsetX, float offsetY)
        {
            float scaleY = bUnityCoords ? -1.0f : 1.0f;     // 유니티 좌표계면 y축 상하 반전
            float scaleX = bMirror ? -1.0f : 1.0f;          // 이미지 좌우반전 이면 x축 반전 

            offsetX = bMirror ? offsetX : -offsetX;         // 좌우반전일때 이미지 이동 값 처리
            offsetY = bUnityCoords ? offsetY : -offsetY;    // 유니티 좌표계일때 이미지 이동 값 처리

            return D2D1::Matrix3x2F::Scale(scaleX, scaleY) * D2D1::Matrix3x2F::Translation(offsetX, offsetY);
        }

        // 디버그 출력을 위한 TM 문자열
        void MakeMatrixToString(
            const D2D1_MATRIX_3X2_F& matrix,
            __out_ecount(bufSize) wchar_t* buffer,
            size_t bufSize
        )
        {
            swprintf_s(
                buffer,
                bufSize,
                L"%.2f, %.2f\n%.2f, %.2f\n%.2f, %.2f\n",
                matrix._11, matrix._12,
                matrix._21, matrix._22,
                matrix._31, matrix._32
            );
        }

        // 변환 행렬에 포함된 피벗(중심점) 보정을 없애고, 피벗 기준점 없이 변환 행렬을 재조정
        D2D1::Matrix3x2F RemovePivot(const D2D1::Matrix3x2F& M_local, const D2D1_POINT_2F& pivot)
        {
            // 1) pivot 보정 전(원점으로) : T(-pivot)
            auto P0 = D2D1::Matrix3x2F::Translation(-pivot.x, -pivot.y);
            // 2) pivot 복원 후         : T(+pivot)
            auto P1 = D2D1::Matrix3x2F::Translation(pivot.x, pivot.y);
            // 3) 양쪽에 곱해서 피벗만 제거
            //    P1 * (P0 * M_local * P1) * P0 == M_local 이지만
            //    pivot 보정만 빼려면: M_noPivot = P1 * M_local * P0
            return P1 * M_local * P0;
        }

        bool IsPointInRect(const D2D1_POINT_2F& point, const D2D1_RECT_F& rect) noexcept
        {
            // 좌표 정규화: left <= right, top <= bottom
            float left = std::min(rect.left, rect.right);
            float right = std::max(rect.left, rect.right);
            float top = std::min(rect.top, rect.bottom);
            float bottom = std::max(rect.top, rect.bottom);

            return (point.x >= left && point.x <= right) && (point.y >= top && point.y <= bottom);
        }

    }
}