#pragma once
#include <cassert>
#include "Component.h"

namespace JDMath = JDGlobal::Math;


namespace JDComponent {
    namespace D2DTM
    {
        

        class RectTransform : public  JDComponent::Component {
        public:
            using Vec2 = JDGlobal::Math::Vector2F;
            using Mat3x2 = D2D1::Matrix3x2F;
            using AnchorPreset = JDGlobal::Core::AnchorPreset;
            using PivotPreset = JDGlobal::Core::PivotPreset;
            using Anchor = JDGlobal::Core::Anchor;

            RectTransform()
                : m_position{ 0, 0 }, m_rotation(0.0f), m_scale{ 1.0f, 1.0f },
                m_dirty(false), m_parent(nullptr), m_anchor()
            {
                m_matrixLocal = D2D1::Matrix3x2F::Identity();
                m_matrixWorld = D2D1::Matrix3x2F::Identity();
            }

            RectTransform(Anchor anchor)
                : m_position{ 0, 0 }, m_rotation(0.0f), m_scale{ 1.0f, 1.0f },
                m_dirty(false), m_parent(nullptr), m_anchor(anchor)
            {
                m_matrixLocal = D2D1::Matrix3x2F::Identity();
                m_matrixWorld = D2D1::Matrix3x2F::Identity();
            }
            ~RectTransform()
            {
                for (auto* child : m_children)
                    child->m_parent = nullptr;
            }

            RectTransform* GetParent() const { return m_parent; }

            void SetParent(RectTransform* newParent)
            {
                assert(newParent != this); // 자기 자신을 부모로 설정할 수 없음
                assert(m_parent == nullptr); // DetachFromParent 를 먼저 호출하고 다시 SetParent를 호출해야 함
                if (newParent == m_parent) return;
                if (m_parent) DetachFromParent();

                m_parent = newParent;
                m_parent->AddChild(this);

                SetDirty();
            }

            void DetachFromParent()
            {
                if (m_parent == nullptr) return;

                m_parent->RemoveChild(this);

                m_parent = nullptr;

                SetDirty();
            }

            void AddChild(RectTransform* child)
            {
                // 자식의 로컬 좌표를 부모 좌표계로 변환
                // 자식의 로컬 트 랜스폼 * 부모의 월드 트랜스폼의 역행렬을 곱하고 원소 추출
                D2D1::Matrix3x2F chiledLocalTM = child->GetLocalMatrix();
                chiledLocalTM = chiledLocalTM * GetInverseWorldMatrix();

                auto M_noPivot = JDMath::RemovePivot(chiledLocalTM, child->GetPivotPoint());
                JDGlobal::Math::DecomposeMatrix3x2(M_noPivot, child->m_position, child->m_rotation, child->m_scale);

                m_children.push_back(child);
            }

            void RemoveChild(RectTransform* child)
            {
                // 월드로 보낸다.
                D2D1::Matrix3x2F chiledLocalTM = child->GetLocalMatrix();
                chiledLocalTM = chiledLocalTM * GetWorldMatrix();

                auto M_noPivot = JDMath::RemovePivot(chiledLocalTM, child->GetPivotPoint());
                JDMath::DecomposeMatrix3x2(M_noPivot, child->m_position, child->m_rotation, child->m_scale);

                m_children.erase(
                    std::remove(m_children.begin(), m_children.end(), child),
                    m_children.end()
                );
            }

            // ** 트랜스폼 설정 **
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

            // ** 방향 벡터 **
            Vec2 GetForward() const
            {
                float radian = JDMath::DegreeToRadian(m_rotation);
                return { std::cosf(radian), std::sinf(radian) };
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

            D2D1_POINT_2F GetPivotPoint() const
            {
                return m_pivot;
            }

        private:
            void SetDirty()
            {
                m_dirty = true;
                for (auto* child : m_children)
                {
                    child->SetDirty();
                }
            }

            void UpdateMatrices();

            void Update(float dt) override {}

            void OnEvent(const std::string& ev) override {}

            void OnEnable() override {}


        private:

            bool m_dirty;

            Vec2     m_position = { 0.f, 0.f }; // 앵커를 기준으로 한 사각형의 피벗 포인트 포지션.
            float    m_rotation = 0.f;          // in degrees
            Vec2     m_scale = { 1.f, 1.f };

            Anchor m_anchor;
            RectTransform* m_parent;
            std::vector<RectTransform*> m_children;

            Mat3x2 m_matrixLocal;
            Mat3x2 m_matrixWorld;

            D2D1_POINT_2F m_pivot{ 0.0f, 0.0f };
        };
    }
}

