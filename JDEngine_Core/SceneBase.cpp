#include "pch.h"
#include "Component.h"
#include "GameObjectBase.h"
#include "ColliderBase.h"
#include "SceneBase.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "D2DRenderer.h"
#include "InputManager.h"

using namespace std;
using namespace JDComponent;
namespace JDScene {
    void SceneBase::CheckCollision()
    {
        m_currPairs.clear();

        size_t n = m_gameObjects.size();
        for (size_t i = 0; i < n; ++i) {
            auto* objA = m_gameObjects[i].get();
            if (!objA) continue;

            auto* colA = objA->GetComponent<JDComponent::ColliderBase>();
            if (!colA) continue;

            for (size_t j = i + 1; j < n; ++j) {
                auto* objB = m_gameObjects[j].get();
                if (!objB) continue;

                auto* colB = objB->GetComponent<JDComponent::ColliderBase>();
                if (!colB) continue;

                if (colA->Intersect(colB)) {
                    CollisionPair cp;
                    cp.A = objA;
                    cp.B = objB;

                    m_currPairs.push_back(std::move(cp));
                }
            }
        }

        // 층돌 처리 (Enter, Stay)
        for (auto& curr : m_currPairs) {
            auto* colA = curr.A->GetComponent<JDComponent::ColliderBase>();
            auto* colB = curr.B->GetComponent<JDComponent::ColliderBase>();

            bool wasColliding = false;
            for (auto& prev : m_prevPairs) {
                if ((prev.A == curr.A && prev.B == curr.B) || (prev.A == curr.B && prev.B == curr.A)) {
                    wasColliding = true; // 현재 충돌쌍과 같은 과거 충돌쌍이 있다면 true.(Stay)
                    break;
                }
            }
            if (colA->IsTrigger() || colB->IsTrigger()) {
                if (wasColliding) { // 이전부터 충돌한 상태라면 Stay.
                    colA->OnTriggerStay(colB);
                    colB->OnTriggerStay(colA);
                }
                else { // 새롭게 충돌했으면 Enter.
                    colA->OnTriggerEnter(colB);
                    colB->OnTriggerEnter(colA);
                }
            }
            else
            {
                if (wasColliding) {
                    colA->OnCollisionStay(colB);
                    colB->OnCollisionStay(colA);
                }
                else {
                    colA->OnCollisionEnter(colB);
                    colB->OnCollisionEnter(colA);
                }
            }
        }

        // 충돌 끝 처리 (Exit)
        for (auto& prev : m_prevPairs) {
            auto* colA = prev.A->GetComponent<JDComponent::ColliderBase>();
            auto* colB = prev.B->GetComponent<JDComponent::ColliderBase>();

            bool stillColliding = false; // 현재 충돌쌍과 같은 과거 충돌쌍이 없다면 false.(Exit)
            for (auto& curr : m_currPairs) {
                if ((prev.A == curr.A && prev.B == curr.B) || (prev.A == curr.B && prev.B == curr.A)) {
                    stillColliding = true;
                    break;
                }
            }
            if (!stillColliding) { // 충돌이 끝났다면 Exit.
                if (colA->IsTrigger() || colB->IsTrigger()) {
                    colA->OnTriggerExit(colB);
                    colB->OnTriggerExit(colA);
                }
                else {
                    colA->OnCollisionExit(colB);
                    colB->OnCollisionExit(colA);
                }
            }
        }
        // 이제 다 끝났으니 현재 충돌 정보를 과거 정보로 넘겨주기.
        m_prevPairs.swap(m_currPairs);
    }

    void SceneBase::DrawColider()
    {
        if (!m_drawCollider)
            return;

        auto camera = D2DRenderer::Instance().GetCamera();
        D2D1_MATRIX_3X2_F view =
            camera
            ? camera->GetViewMatrix()
            : D2D1::Matrix3x2F::Identity();

        for (auto& objPtr : m_gameObjects)
        {
            auto* obj = objPtr.get();
            if (!obj) continue;

            auto* col = obj->GetComponent<JDComponent::ColliderBase>();
            if (!col) continue;

            auto* tm = obj->GetComponent<JDComponent::D2DTM::Transform>();
            if (!tm) continue;
            D2D1_MATRIX_3X2_F world = tm->GetWorldMatrix();

            D2D1_MATRIX_3X2_F worldView = world * view;
            D2DRenderer::Instance().SetTransform(worldView);

            // 이제 강조 색 결정 (열린 셀만 이 로직을 타게 됩니다)

            UINT32 color;
            if (col->IsOpen())
            {
                if (col->IsMouseOver(GetMouseWorldPos()))
                    color = 0xFFFF0000;      // 마우스 오버 빨강
                else if (col->GetColliding())
                    color = 0xFF00FF00;      // 충돌 초록
                else
                    color = 0xFF000000;      // 기본 검정
            }
            else
            {
                // 닫힌 콜라이더는 어두운 회색
                color = 0xFF555555;
            }

            if (col->GetColliderType() == JDComponent::ColliderType::Box) // 박스 콜라이더
            {
                auto* colb = static_cast<JDComponent::BoxCollider*>(col);
                auto  halfSize = colb->GetHalfSize();
                auto  offset = colb->GetColliderOffset();

                D2DRenderer::Instance().DrawRectangle(
                    offset.x - halfSize.x, offset.y + halfSize.y,
                    offset.x + halfSize.x, offset.y - halfSize.y,
                    color
                );
            }
            else if (col->GetColliderType() == JDComponent::ColliderType::Circle) // 원형 콜라이더
            {
                auto* colc = static_cast<JDComponent::CircleCollider*>(col);
                float radius = colc->GetRadius();
                auto  offset = colc->GetColliderOffset();

                D2DRenderer::Instance().DrawCircle(
                    offset.x, offset.y, radius,
                    color
                );
            }
        }

        // 6) 그리기 끝난 뒤, 원래 매트릭스(뷰만) 복원
        D2DRenderer::Instance().SetTransform(view);
    }

    Vec2 SceneBase::GetMouseWorldPos()
    {
        // 1) 마우스 스크린 좌표 얻기
        auto& inputMgr = InputManager::Instance();
        auto  mousePos = inputMgr.GetMouseState().pos;    // {x, y}
        Vec2 screenPos{ static_cast<float>(mousePos.x), static_cast<float>(mousePos.y) };

        // 2) 스크린 → 월드 변환
        Vec2 mouseWorld = screenPos;
        auto camera = D2DRenderer::Instance().GetCamera();
        if (camera) {
            auto invView = camera->GetViewMatrix();
            D2D1InvertMatrix(&invView);
            mouseWorld = camera->TransformPoint(invView, { screenPos.x,screenPos.y });
        }

        return mouseWorld;
    }
}
