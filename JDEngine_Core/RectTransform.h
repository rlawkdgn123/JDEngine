#pragma once
#include "GameObjectBase.h"
#include "Component.h"
namespace JDMath = JDGlobal::Math;


namespace JDComponent {
    namespace D2DTM
    {

        class RectTransform : public JDComponent::Component {
        public:
            void Update(float dt) override {};

            void OnEvent(const std::string& ev) override {};

            void OnEnable() override {};

            void OnDisable() override {};

            using Vector2F = JDGlobal::Math::Vector2F;
            using Mat3x2 = D2D1::Matrix3x2F;
            using PivotPreset = JDGlobal::Core::PivotPreset;
            using AnchorPreset = JDGlobal::Core::AnchorPreset;
            RectTransform()
                : m_position{ 0, 0 }, m_rotation(0.0f), m_scale{ 1.0f, 1.0f },
                m_dirty(false), m_parent(nullptr)
            {
                m_matrixLocal = D2D1::Matrix3x2F::Identity();
                m_matrixWorld = D2D1::Matrix3x2F::Identity();
            }

            RectTransform(Vector2F tr)
                : m_position{ tr.x, tr.y }, m_rotation(0.0f), m_scale{ 1.0f, 1.0f },
                m_dirty(false), m_parent(nullptr)
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

            inline void AddChild(RectTransform* child)
            {
                // 자식의 로컬 좌표를 부모 좌표계로 변환
                // 자식의 로컬 트 랜스폼 * 부모의 월드 트랜스폼의 역행렬을 곱하고 원소 추출
                D2D1::Matrix3x2F chiledLocalTM = child->GetLocalMatrix();
                chiledLocalTM = chiledLocalTM * GetInverseWorldMatrix();

                auto M_noPivot = JDMath::RemovePivot(chiledLocalTM, child->GetPivot());
                JDGlobal::Math::DecomposeMatrix3x2(M_noPivot, child->m_position, child->m_rotation, child->m_scale);

                m_children.push_back(child);
            }

            inline void RemoveChild(RectTransform* child)
            {
                // 월드로 보낸다.
                D2D1::Matrix3x2F chiledLocalTM = child->GetLocalMatrix();
                chiledLocalTM = chiledLocalTM * GetWorldMatrix();

                auto M_noPivot = JDMath::RemovePivot(chiledLocalTM, child->GetPivot());
                JDMath::DecomposeMatrix3x2(M_noPivot, child->m_position, child->m_rotation, child->m_scale);

                m_children.erase(
                    std::remove(m_children.begin(), m_children.end(), child),
                    m_children.end()
                );
            }

            // ** Width, Height **
            void SetSize(const Vector2F& size) { m_size = size; SetDirty(); }
            const Vector2F& GetSize() const { return m_size; }

            // ** 트랜스폼 설정 **
            void SetPosition(const Vector2F& pos) { m_position = pos; SetDirty(); }
            void SetRotation(float degree) { m_rotation = degree; SetDirty(); }
            void SetScale(const Vector2F& scale) { m_scale = scale; SetDirty(); }

            const Vector2F& GetPosition() const { return m_position; }
            float GetRotation() const { return m_rotation; }
            const Vector2F& GetScale() const { return m_scale; }


            void Translate(const Vector2F& delta)
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
            Vector2F GetForward() const
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

            Mat3x2& GetWorldMatrix()
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

            // Unity와 동일한 피벗 적용을 위한 렌더링 매트릭스
            Mat3x2 GetRenderMatrix()
            {
                // 렌더링할 때 피벗을 적용한 최종 매트릭스
                Vector2F pivotOffset = GetPivotOffset();
                auto M_pivot = D2D1::Matrix3x2F::Translation(-pivotOffset.x, -pivotOffset.y);
                return M_pivot * GetWorldMatrix();
            }

            // ** 회전과 스케일을 위한 피봇, 앵커 설정 **
            void SetPivot(const Vector2F& relativePivot);
            void SetPivot(PivotPreset preset);

            // 상대 좌표를 반환하는 getter
            Vector2F GetPivot() const { return m_pivot; }

            // 실제 픽셀 오프셋을 계산해서 반환하는 헬퍼 함수
            Vector2F GetPivotOffset() const {
                Vector2F size = GetSize(); // 현재 사이즈를 가져옴
                return { m_pivot.x * size.x, m_pivot.y * size.y };
            }

            void SetAnchor(const Vector2F& anchor) {
                m_anchor = anchor;
                SetDirty();
            }

            Vector2F GetAnchor() const {
                return Vector2F{ m_anchor.x, m_anchor.y };
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


        private:

            bool m_dirty;

            Vector2F     m_size         = { 100.0f, 100.0f };
            Vector2F     m_position     = { 0.f, 0.f };
            float        m_rotation     = { 0.f };
            Vector2F     m_scale        = { 1.f, 1.f };

            Vector2F     m_pivot        = { 0.5f, 0.5f };
            Vector2F     m_anchor       = { 0.5f, 0.5f };

            RectTransform* m_parent;
            std::vector<RectTransform*> m_children;

            Mat3x2 m_matrixLocal;
            Mat3x2 m_matrixWorld;
        };
    }
}
