#pragma once
#include "Component.h"
#include "D2DTransform.h"
#include "RectTransform.h"

#include "InputManager.h"
#include "GameObjectBase.h"

namespace JDComponent {

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
                Vector2F centerPos = rectTransform->GetPosition();
                Vector2F size = rectTransform->GetSize();
                Vector2F scale = rectTransform->GetScale();
                Vector2F scaledSize = size * scale;

                // D2D 렌더링 방식에 맞춰 좌상단 좌표로 변환
                Vector2F topLeft = centerPos; // D2D는 이미 좌상단 기준이므로
                Vector2F bottomRight = topLeft + scaledSize;

                bool hit = (mousePos.x >= topLeft.x && mousePos.x <= bottomRight.x &&
                    mousePos.y >= topLeft.y && mousePos.y <= bottomRight.y);

                // Debug 출력문
                /*
                std::cout << "중심 위치: (" << centerPos.x << ", " << centerPos.y << ")" << std::endl;
                std::cout << "좌상단: (" << topLeft.x << ", " << topLeft.y << ")" << std::endl;
                std::cout << "우하단: (" << bottomRight.x << ", " << bottomRight.y << ")" << std::endl;
                std::cout << "마우스: (" << mousePos.x << ", " << mousePos.y << ")" << std::endl;
                std::cout << "결과: " << (hit ? "HIT" : "MISS") << std::endl;
                */

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


