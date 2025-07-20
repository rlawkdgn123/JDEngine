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
            // �������ȿ� ���� �������� Ȯ��
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

        // �̵� ���
        D2D1_MATRIX_3X2_F MakeTranslationMatrix(D2D1_SIZE_F size) {
            D2D1_MATRIX_3X2_F _translation;

            _translation._11 = 1.0f; _translation._12 = 0.0f;
            _translation._21 = 0.0f; _translation._22 = 1.0f;
            _translation._31 = size.width; _translation._32 = size.height;

            return _translation;
        }

        // ȸ�� ���
        D2D1_MATRIX_3X2_F MakeRotationMatrix_Origin(FLOAT angle)
        {
            float _rad = DegreeToRadian(angle);
            D2D1_MATRIX_3X2_F _rotation;

            //sin��Ÿ ��ŭ �ݽð� ���� ȸ��
            _rotation._11 = cos(_rad); _rotation._21 = -sin(_rad); _rotation._31 = 0;
            _rotation._12 = sin(_rad); _rotation._22 = cos(_rad); _rotation._32 = 0;

            return _rotation;
        }

        // ũ�� ���
        D2D1_MATRIX_3X2_F MakeScaleMatrix_Origin(D2D1_SIZE_F size)
        {
            D2D1_MATRIX_3X2_F _scale;

            //1.0f�� �����ִ� ���� : ����� ĳ���� �� �ǵ� ǥ��
            _scale._11 = size.width * 1.0f; _scale._12 = 0.0f;
            _scale._21 = 0.0f; _scale._22 = size.height * 1.0f;
            _scale._31 = 0.0f; _scale._32 = 0.0f;

            return _scale;
        }

        // ȸ�� ��� ��ȯ
        D2D1_MATRIX_3X2_F MakeRotationMatrix(FLOAT angle, D2D1_POINT_2F center /*= D2D1::Point2F()*/)
        {
            // 1.�߽��� center ��ŭ ��ǥ�踦 �������� �̵� 
            // 2.���� �������� ȸ�� (�⺻ ȸ�� ���)
            // 3.���� ��ġ(�߽���)�� ��ǥ�踦 �ٽ� �̵�
            D2D1_MATRIX_3X2_F _translateToOrigin = MakeTranslationMatrix(D2D1::Size(-center.x, -center.y));
            D2D1_MATRIX_3X2_F _rotateOnOrigin = MakeRotationMatrix_Origin(angle);
            D2D1_MATRIX_3X2_F _translateToCenter = MakeTranslationMatrix(D2D1::Size(center.x, center.y));

            D2D1_MATRIX_3X2_F _resultTM = _translateToOrigin * _rotateOnOrigin * _translateToCenter;

            return _resultTM;
        }

        // ũ�� ��� ��ȯ
        D2D1_MATRIX_3X2_F MakeScaleMatrix(D2D1_SIZE_F size, D2D1_POINT_2F center /*= D2D1::Point2F()*/)
        {
            // 1.�߽��� center ��ŭ ��ǥ�踦 �������� �̵� 
            // 2.���� �������� ũ�� ��ȯ (�⺻ ũ�� ���)
            // 3.���� ��ġ(�߽���)�� ��ǥ�踦 �ٽ� �̵�
            D2D1_MATRIX_3X2_F _translateToOrigin = MakeTranslationMatrix(D2D1::Size(-center.x, -center.y));
            D2D1_MATRIX_3X2_F _scaleOnOrigin = MakeScaleMatrix_Origin(size);
            D2D1_MATRIX_3X2_F _translateToCenter = MakeTranslationMatrix(D2D1::Size(center.x, center.y));

            D2D1_MATRIX_3X2_F _resultTM = _translateToOrigin * _scaleOnOrigin * _translateToCenter;

            return _resultTM;
        }

        /// <summary>
        /// ���� ���� ��ǥ���� ������ ��ũ�� ��ǥ��� ��ȯ�ϴ� ��ȯ ��� ����
        /// </summary>
        /// <param name="bUnityCoords">����Ƽ ��ǥ�� ��� ���� (Y�� ���� ���� ����)</param>
        /// <param name="bMirror">�¿� ���� ���� (X�� ���� ����)</param>
        /// <param name="offsetX">X�� ���� ��ǥ ������</param>
        /// <param name="offsetY">Y�� ���� ��ǥ ������</param>
        /// <returns>��ȯ ��� (�����ϰ� ���� �̵��� ������ ����)</returns>
        D2D1::Matrix3x2F MakeRenderMatrix(bool bUnityCoords, bool bMirror, float offsetX, float offsetY)
        {
            float scaleY = bUnityCoords ? -1.0f : 1.0f;     // ����Ƽ ��ǥ��� y�� ���� ����
            float scaleX = bMirror ? -1.0f : 1.0f;          // �̹��� �¿���� �̸� x�� ���� 

            offsetX = bMirror ? offsetX : -offsetX;         // �¿�����϶� �̹��� �̵� �� ó��
            offsetY = bUnityCoords ? offsetY : -offsetY;    // ����Ƽ ��ǥ���϶� �̹��� �̵� �� ó��

            return D2D1::Matrix3x2F::Scale(scaleX, scaleY) * D2D1::Matrix3x2F::Translation(offsetX, offsetY);
        }

        // ����� ����� ���� TM ���ڿ�
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

        // ��ȯ ��Ŀ� ���Ե� �ǹ�(�߽���) ������ ���ְ�, �ǹ� ������ ���� ��ȯ ����� ������
        D2D1::Matrix3x2F RemovePivot(const D2D1::Matrix3x2F& M_local, const D2D1_POINT_2F& pivot)
        {
            // 1) pivot ���� ��(��������) : T(-pivot)
            auto P0 = D2D1::Matrix3x2F::Translation(-pivot.x, -pivot.y);
            // 2) pivot ���� ��         : T(+pivot)
            auto P1 = D2D1::Matrix3x2F::Translation(pivot.x, pivot.y);
            // 3) ���ʿ� ���ؼ� �ǹ��� ����
            //    P1 * (P0 * M_local * P1) * P0 == M_local ������
            //    pivot ������ ������: M_noPivot = P1 * M_local * P0
            return P1 * M_local * P0;
        }

        bool IsPointInRect(const D2D1_POINT_2F& point, const D2D1_RECT_F& rect) noexcept
        {
            // ��ǥ ����ȭ: left <= right, top <= bottom
            float left = std::min(rect.left, rect.right);
            float right = std::max(rect.left, rect.right);
            float top = std::min(rect.top, rect.bottom);
            float bottom = std::max(rect.top, rect.bottom);

            return (point.x >= left && point.x <= right) && (point.y >= top && point.y <= bottom);
        }

    }
}