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
namespace JDScene {
    void SceneBase::CheckCollision()
    {
        m_currPairs.clear(); // 처음부터 현재 충돌 정보는 초기화하고 시작하기.

        // 현재 충돌한 쌍 저장
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
        if (m_drawCollider) {
            size_t n = m_gameObjects.size();
            for (size_t i = 0; i < n; ++i) {
                auto* obj = m_gameObjects[i].get();
                if (!obj) continue;

                auto* col = obj->GetComponent<JDComponent::ColliderBase>();
                if (!col) continue;

                auto* tm = obj->GetComponent<JDComponent::D2DTM::Transform>();
                auto pos = tm->GetPosition();

                UINT32 color = (col->GetColliding()) // 충돌중이면 초록색, 아니면 검정. 
                    ? 0xFF00FF00
                    : 0xFF000000;

                // 하이라이트된 인덱스일 때 빨강으로.
                //if (i == m_highlightedIndex) color = 0xFFFF0000;
                if (col->IsMouseOver(GetMouseWorldPos())) color = 0xFFFF0000;


                if (col->GetColliderType() == JDComponent::ColliderType::Box) {
                    auto* colB = static_cast<JDComponent::BoxCollider*>(col);
                    auto hsize = colB->GetHalfSize();
                    auto offset = colB->GetColliderOffset();

                    float left = pos.x + offset.x - hsize.x;
                    float top = pos.y + offset.y + hsize.y;
                    float right = pos.x + offset.x + hsize.x;
                    float bottom = pos.y + offset.y - hsize.y;

                    D2DRenderer::Instance().DrawRectangle(left, top, right, bottom, color);
                }
                else if (col->GetColliderType() == JDComponent::ColliderType::Circle) {
                    auto* colC = static_cast<JDComponent::CircleCollider*>(col);
                    auto radius = colC->GetRadius();
                    auto offset = colC->GetColliderOffset();

                    float x = pos.x + offset.x;
                    float y = pos.y + offset.y;

                    D2DRenderer::Instance().DrawCircle(x, y, radius, color);
                }
            }
        }
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
