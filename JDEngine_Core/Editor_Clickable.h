#pragma once
#include <algorithm>
#include "Component.h"
#include "D2DTransform.h"
#include "RectTransform.h"

#include "InputManager.h"
#include "GameObjectBase.h"
#include "ColliderBase.h"

namespace JDComponent {

    using std::min;
    using std::max;
    using Vector2F = JDGlobal::Math::Vector2F;
    using Transform = JDComponent::D2DTM::Transform;
    using RectTransform = JDComponent::D2DTM::RectTransform;
    using ColliderBase = JDComponent::ColliderBase;

    class Editor_Clickable : public Component
    {
    public:
        bool IsHit(Vector2F mousePos)
        {
            if (!m_Owner || !m_Owner->IsActive()) return false;

            // =====================================================================
            // 1. UI 객체 (RectTransform) 클릭 판정
            // =====================================================================
            if (auto rectTransform = m_Owner->GetComponent<RectTransform>())
            {
                // UI의 IsHit 함수에 전달되는 mousePos는 '스크린 좌표'입니다.

                // 1-1. 스크린 좌표인 마우스 위치를 이 UI 객체의 '로컬 좌표'로 변환합니다.
                D2D1_MATRIX_3X2_F inverseMatrix = rectTransform->GetWorldMatrix();
                D2D1InvertMatrix(&inverseMatrix);

                D2D1_POINT_2F localMousePoint;
                localMousePoint.x = mousePos.x * inverseMatrix._11 + mousePos.y * inverseMatrix._21 + inverseMatrix._31;
                localMousePoint.y = mousePos.x * inverseMatrix._12 + mousePos.y * inverseMatrix._22 + inverseMatrix._32;

                // 1-2. 이 객체의 로컬 공간에서 경계 상자(Bounding Box)를 계산합니다.
                //      복잡한 앵커 계산은 필요 없습니다. 로컬 공간에서는 피벗과 크기만 알면 됩니다.
                Vector2F size = rectTransform->GetSize();
                Vector2F pivot = rectTransform->GetPivot(); // 0~1 사이의 상대 피벗

                // D2D는 Y축이 아래로 향하므로, 로컬 좌표계의 Y도 아래를 양수로 간주합니다.
                float localLeft = -pivot.x * size.x;
                float localRight = (1.0f - pivot.x) * size.x;
                float localTop = -pivot.y * size.y;
                float localBottom = (1.0f - pivot.y) * size.y;

                // 1-3. 변환된 로컬 마우스 좌표가 로컬 경계 상자 내에 있는지 확인합니다.
                return (localMousePoint.x >= localLeft && localMousePoint.x <= localRight &&
                    localMousePoint.y >= localTop && localMousePoint.y <= localBottom);
            }
            // =====================================================================
            // 2. 게임 오브젝트 (Transform) 클릭 판정
            // =====================================================================
            else if (auto transform = m_Owner->GetComponent<Transform>())
            {
                // 게임오브젝트의 IsHit 함수에 전달되는 mousePos는 '월드 좌표'입니다.

                // 2-1. 클릭 판정의 책임을 Collider에게 위임합니다.
                auto collider = m_Owner->GetComponent<ColliderBase>();
                if (collider)
                {
                    // 2-2. Collider의 IsMouseOver 함수를 호출하고 그 결과를 반환합니다.
                    //      IsMouseOver 함수는 월드 좌표를 기준으로 올바르게 판정하도록 이미 구현되어 있습니다.
                    return collider->IsMouseOver(mousePos);
                }
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


