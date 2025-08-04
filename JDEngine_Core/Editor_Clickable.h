#pragma once
#include <algorithm>
#include "Component.h"
#include "D2DTransform.h"
#include "RectTransform.h"

#include "InputManager.h"
#include "GameObjectBase.h"

namespace JDComponent {

    using std::min;
    using std::max;
    using Vector2F = JDGlobal::Math::Vector2F;
    using Transform = JDComponent::D2DTM::Transform;
    using RectTransform = JDComponent::D2DTM::RectTransform;

    class Editor_Clickable : public Component
    {
    public:
        bool IsHit(Vector2F mousePos)
        {
            if (!m_Owner) return false;

            if (auto rectTransform = m_Owner->GetComponent<RectTransform>())
            {
                // 1. 마우스 좌표를 RectTransform의 로컬 좌표계로 변환
                D2D1_MATRIX_3X2_F worldMatrix = rectTransform->GetWorldMatrix();
                D2D1_MATRIX_3X2_F inverseMatrix = worldMatrix;
                D2D1InvertMatrix(&inverseMatrix);

                // 마우스를 로컬 좌표로 변환
                Vector2F localMouse = {
                    inverseMatrix._11 * mousePos.x + inverseMatrix._21 * mousePos.y + inverseMatrix._31,
                    inverseMatrix._12 * mousePos.x + inverseMatrix._22 * mousePos.y + inverseMatrix._32
                };

                // 2. 렌더링과 똑같은 방식으로 영역 계산 (하지만 로컬 좌표계에서)
                Vector2F pos = rectTransform->GetPosition();
                Vector2F size = rectTransform->GetSize();
                Vector2F pivot = rectTransform->GetPivot();
                Vector2F anchorOffset = { 0.f, 0.f };

                auto parent = rectTransform->GetParent();
                if (parent) {
                    Vector2F parentSize = parent->GetSize();
                    Vector2F anchor = rectTransform->GetAnchor();
                    anchorOffset = {
                        parentSize.x * anchor.x,
                        parentSize.y * anchor.y
                    };
                }

                Vector2F finalPos = {
                    anchorOffset.x + pos.x,
                    anchorOffset.y + pos.y
                };

                // 로컬 좌표계에서 영역 계산
                float left = finalPos.x - size.x * pivot.x;
                float top = finalPos.y - size.y * pivot.y;
                float right = finalPos.x + size.x * (1 - pivot.x);
                float bottom = finalPos.y + size.y * (1 - pivot.y);

                bool hit = (localMouse.x >= left && localMouse.x <= right &&
                    localMouse.y >= top && localMouse.y <= bottom);

                std::cout << "로컬 마우스: (" << localMouse.x << ", " << localMouse.y << ")" << std::endl;
                std::cout << "로컬 영역: (" << left << ", " << top << ") ~ (" << right << ", " << bottom << ")" << std::endl;
                std::cout << "결과: " << (hit ? "HIT" : "MISS") << std::endl;

                return hit;
            }
            else if (auto transform = m_Owner->GetComponent<Transform>())
            {
                Vector2F pos = transform->GetPosition();
                float radius = 50.0f;
                float dx = mousePos.x - pos.x;
                float dy = mousePos.y - pos.y;
                return (dx * dx + dy * dy <= radius * radius);
            }

            return false;
        }

        // Component을(를) 통해 상속됨 [ 빈 함수 ]
        void Update(float deltaTime) override {}            
        void OnEvent(const std::string& event) override {}
        void OnEnable() override {}
        void OnDisable() override {}
    };

}


