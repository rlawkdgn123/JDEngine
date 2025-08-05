#pragma once
#include <cassert>
#include "Component.h"

namespace JDMath = JDGlobal::Math;

 
namespace JDComponent {
    namespace D2DTM
    {

        class Transform : public  JDComponent::Component {
        public:
            using Vec2 = JDGlobal::Math::Vector2F;
            using Mat3x2 = D2D1::Matrix3x2F;
            using PivotPreset = JDGlobal::Core::PivotPreset;
            Transform()
                : m_position{ 0, 0 }, m_rotation(0.0f), m_scale{ 1.0f, 1.0f },
                m_dirty(false), m_parent(nullptr)
            {
                m_matrixLocal = D2D1::Matrix3x2F::Identity();
                m_matrixWorld = D2D1::Matrix3x2F::Identity();
            }

            Transform(Vec2 tr)
                : m_position{ tr.x, tr.y }, m_rotation(0.0f), m_scale{ 1.0f, 1.0f },
                m_dirty(false), m_parent(nullptr)
            {
                m_matrixLocal = D2D1::Matrix3x2F::Identity();
                m_matrixWorld = D2D1::Matrix3x2F::Identity();
            }
            ~Transform()
            {
                for (auto* child : m_children)
                    child->m_parent = nullptr;
            }

            Transform* GetParent() const { return m_parent; }

            void SetParent(Transform* newParent)
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

            void AddChild(Transform* child)
            {
                m_children.push_back(child);

                // LEGACY : 과거 코드
                /*
                //// 자식의 로컬 좌표를 부모 좌표계로 변환
                //// 자식의 로컬 트 랜스폼 * 부모의 월드 트랜스폼의 역행렬을 곱하고 원소 추출
                //D2D1::Matrix3x2F chiledLocalTM = child->GetLocalMatrix();
                //chiledLocalTM = chiledLocalTM * GetInverseWorldMatrix();

                //auto M_noPivot = JDMath::RemovePivot(chiledLocalTM, child->GetPivotPoint());
                //JDGlobal::Math::DecomposeMatrix3x2(M_noPivot, child->m_position, child->m_rotation, child->m_scale);

                //m_children.push_back(child);
                */
                
            }

            void RemoveChild(Transform* child)
            {
                m_children.erase(
                    std::remove(m_children.begin(), m_children.end(), child),
                    m_children.end()
                );

                /*
                //// 월드로 보낸다.
                //D2D1::Matrix3x2F chiledLocalTM = child->GetLocalMatrix();
                //chiledLocalTM = chiledLocalTM * GetWorldMatrix();

                //auto M_noPivot = JDMath::RemovePivot(chiledLocalTM, child->GetPivotPoint());
                //JDMath::DecomposeMatrix3x2(M_noPivot, child->m_position, child->m_rotation, child->m_scale);

                //m_children.erase(
                //    std::remove(m_children.begin(), m_children.end(), child),
                //    m_children.end()
                //);
                */
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

            Vec2 GetWorldPosition()
            {
                if (m_parent)
                {
                    // 부모의 월드 좌표에 나의 상대 좌표를 더한 것이 나의 월드 좌표
                    // (부모의 회전/스케일이 자식의 위치에 영향을 주는 경우 더 복잡한 계산이 필요하지만,
                    //  간단한 시스템에서는 이 방식으로 충분히 동작합니다)
                    return m_parent->GetWorldPosition() + m_position;
                }
                // 부모가 없으면 나의 상대 좌표가 곧 월드 좌표
                return m_position;
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

            Mat3x2 GetScreenCenterTranslation()
            {
                float width = static_cast<float>(JDGlobal::Window::WindowSize::Instance().GetWidth());
                float height = static_cast<float>(JDGlobal::Window::WindowSize::Instance().GetHeight());

                return D2D1::Matrix3x2F::Translation(
                    width * 0.5f,
                    height * 0.5f
                );
            }

            // ** 회전과 스케일을 위한 피봇 설정 **
            void SetPivotPreset(PivotPreset preset, const D2D1_SIZE_F& size);

            D2D1_POINT_2F GetPivotPoint() const
            {
                return m_pivot;
            }

            void SetPivot(const Vec2& pivot)
            {
                m_pivot = { pivot.x, pivot.y };
                SetDirty();
            }

            void SetPivot(const D2D1_POINT_2F& pivot)
            {
                m_pivot = pivot;
                SetDirty();
            }

            Vec2 GetPivot() const
            {
                return { m_pivot.x, m_pivot.y };
            }

            // 픽셀 단위인 m_pivot을 0~1 비율의 상대 좌표로 변환해서 반환합니다.
            Vec2 GetRelativePivot(const D2D1_SIZE_F& size) const
            {
                // size가 0일 경우 0으로 나누는 것을 방지
                if (size.width == 0.0f || size.height == 0.0f)
                {
                    return { 0.5f, 0.5f }; // 기본값 반환
                }
                // 유니티 좌표계(Y 위가 양수)에 맞춰서 반환
                return { m_pivot.x / size.width, m_pivot.y / size.height };
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

            void OnDisable() override {}


        private:

            bool m_dirty;

            Vec2     m_position = { 0.f, 0.f }; // translation position
            float    m_rotation = 0.f;          // in degrees
            Vec2     m_scale = { 1.f, 1.f };

            D2D1_POINT_2F m_pivot{ 0.5f, 0.5f };

            Transform* m_parent;
            std::vector<Transform*> m_children;

            Mat3x2 m_matrixLocal;
            Mat3x2 m_matrixWorld;
        };
    }
}

