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
            // �θ� Transform ���� (���� ���� X, ������ ����)
            void SetParentTransform(Transform* parentTM) {
                // GameObject�� ���踦 �������ֹǷ�, ���⼭�� �ܼ� ������ ����
                m_parentTM = parentTM;
                MarkAsDirty(); // �θ� �ٲ������ ���� ��� ���� �ʿ�
            }

            // Ʈ������ ���� 
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

            // ** ��ȯ ��� **
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

            // ** ȸ���� �������� ���� �Ǻ� ���� **
            void SetPivotPreset(PivotPreset preset, const D2D1_SIZE_F& size);
            D2D1_POINT_2F GetPivotPoint() const { return m_pivot; }

        private:
            void SetDirty()
            {
                m_dirty = true;
                // �ڽ� ���Ĵ� GameObject �������� ó���ؾ� ��
            }

            void UpdateMatrices(); // ���Ǵ� �ܺο� ����

        private:
            Vec2    m_position = { 0.f, 0.f };  // ��ġ�� (Position)
            float   m_rotation = 0.f;           // ȸ���� (Rotation)
            Vec2    m_scale = { 1.f, 1.f };     // ũ�Ⱚ (Scale) 

            Transform* m_parentTM = nullptr;    // �θ� Transform ���� (���� X)

            Mat3x2 m_matrixLocal;
            Mat3x2 m_matrixWorld;
            D2D1_POINT_2F m_pivot;
            bool m_dirty;
        };
    }


}
