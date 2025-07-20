#pragma once
#include <cassert>

namespace Compontnts {
    namespace D2DTM
    {
        enum class PivotPreset
        {
            TopLeft,
            TopRight,
            BottomLeft,
            BottomRight,
            Center
        };

        class Transform : Interface::Comonent::Transform{
        public:
            using Vec2 = JDGlobal::Math::Vector2F;
            using Mat3x2 = D2D1::Matrix3x2F;

            Transform()
                : m_position{ 0, 0 }, m_rotation(0.0f), m_scale{ 1.0f, 1.0f },
                m_dirty(true), m_parentTM(nullptr) {
                m_matrixLocal = D2D1::Matrix3x2F::Identity();
                m_matrixWorld = D2D1::Matrix3x2F::Identity();
            }

            ~Transform() = default;

        public:
            // 부모 Transform 참조 (계층 관리 X, 소유권 없음)
            void SetParentTransform(Transform* parentTM) {
                // GameObject가 관계를 설정해주므로, 여기서는 단순 참조만 갱신
                m_parentTM = parentTM;
                MarkAsDirty(); // 부모가 바뀌었으니 월드 행렬 재계산 필요
            }

            // 트랜스폼 설정 
            void SetPosition(const Vec2& pos) { m_position = pos; SetDirty(); }
            void SetRotation(float degree) { m_rotation = degree; SetDirty(); }
            void SetScale(const Vec2& scale) { m_scale = scale; SetDirty(); }

            const Vec2& GetPosition() const { return m_position; }
            float GetRotation() const { return m_rotation; }
            const Vec2& GetScale() const { return m_scale; }

            void SetPosition(const Vec2& pos) { m_position = pos; SetDirty(); }
            void SetRotation(float degree) { m_rotation = degree; SetDirty(); }
            void SetScale(const Vec2& scale) { m_scale = scale; SetDirty(); }

            const Vec2& GetPosition() const { return m_position; }
            float GetRotation() const { return m_rotation; }
            const Vec2& GetScale() const { return m_scale; }

            void Translate(const Vec2& delta)
            {
                m_position.x += delta.x;
                m_position.y += delta.y;
                SetDirty();
            }
            void Translate(const float x, const float y)
            {
                m_position.x += x;
                m_position.y += y;
                SetDirty();
            }

            void Rotate(float degree)
            {
                m_rotation += degree;
                SetDirty();
            }

            // ** 변환 행렬 **
            const Mat3x2& GetLocalMatrix()
            {
                if (m_dirty) UpdateMatrices();
                return m_matrixLocal;
            }

            const Mat3x2& GetWorldMatrix()
            {
                if (m_dirty) UpdateMatrices();
                return m_matrixWorld;
            }

            Mat3x2 GetInverseWorldMatrix()
            {
                Mat3x2 inv = GetWorldMatrix();
                inv.Invert();
                return inv;
            }

            // ** 회전과 스케일을 위한 피봇 설정 **
            void SetPivotPreset(PivotPreset preset, const D2D1_SIZE_F& size);
            D2D1_POINT_2F GetPivotPoint() const { return m_pivot; }

        private:
            void SetDirty()
            {
                m_dirty = true;
                // 자식 전파는 GameObject 계층에서 처리해야 함
            }

            void UpdateMatrices(); // 정의는 외부에 있음

        private:
            Vec2    m_position = { 0.f, 0.f };  // 위치값 (Position)
            float   m_rotation = 0.f;           // 회전값 (Rotation)
            Vec2    m_scale = { 1.f, 1.f };     // 크기값 (Scale) 

            Transform* m_parentTM = nullptr;    // 부모 Transform 참조 (계층 X)

            Mat3x2 m_matrixLocal;
            Mat3x2 m_matrixWorld;
            D2D1_POINT_2F m_pivot;
            bool m_dirty;
        };
    }


}
