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

            auto* colA = objA->GetComponent<ColliderBase>();
            if (!colA) continue;
            // --- 디버그용 콜라이더면 스킵 ---
            if (auto* boxA = dynamic_cast<BoxCollider*>(colA)) {
                if (boxA->Purpose() == ColliderPurpose::Debug)
                    continue;
            }

            for (size_t j = i + 1; j < n; ++j) {
                auto* objB = m_gameObjects[j].get();
                if (!objB) continue;

                auto* colB = objB->GetComponent<ColliderBase>();
                if (!colB) continue;
                // --- 디버그용 콜라이더면 스킵 ---
                if (auto* boxB = dynamic_cast<BoxCollider*>(colB)) {
                    if (boxB->Purpose() == ColliderPurpose::Debug)
                        continue;
                }

                if (colA->Intersect(colB)) {
                    m_currPairs.push_back({ objA, objB });
                }
            }
        }

        // ────────────── Enter/Stay 처리 ──────────────
        for (auto& curr : m_currPairs) {
            auto* colA = curr.A->GetComponent<ColliderBase>();
            auto* colB = curr.B->GetComponent<ColliderBase>();
            // (위와 동일하게, 디버그용이면 건너뛰기)
            if (auto* boxA = dynamic_cast<BoxCollider*>(colA))
                if (boxA->Purpose() == ColliderPurpose::Debug) continue;
            if (auto* boxB = dynamic_cast<BoxCollider*>(colB))
                if (boxB->Purpose() == ColliderPurpose::Debug) continue;

            bool wasColliding = false;
            for (auto& prev : m_prevPairs) {
                if ((prev.A == curr.A && prev.B == curr.B) || (prev.A == curr.B && prev.B == curr.A)) {
                    wasColliding = true; break;
                }
            }

            if (colA->IsTrigger() || colB->IsTrigger()) {
                if (wasColliding) {
                    colA->OnTriggerStay(colB);
                    colB->OnTriggerStay(colA);
                }
                else {
                    colA->OnTriggerEnter(colB);
                    colB->OnTriggerEnter(colA);
                }
            }
            else {
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

        // ────────────── Exit 처리 ──────────────
        for (auto& prev : m_prevPairs) {
            auto* colA = prev.A->GetComponent<ColliderBase>();
            auto* colB = prev.B->GetComponent<ColliderBase>();
            // (디버그용이면 Exit 콜백도 생략)
            if (auto* boxA = dynamic_cast<BoxCollider*>(colA))
                if (boxA->Purpose() == ColliderPurpose::Debug) continue;
            if (auto* boxB = dynamic_cast<BoxCollider*>(colB))
                if (boxB->Purpose() == ColliderPurpose::Debug) continue;

            bool still = false;
            for (auto& curr : m_currPairs) {
                if ((prev.A == curr.A && prev.B == curr.B) || (prev.A == curr.B && prev.B == curr.A)) {
                    still = true; break;
                }
            }
            if (!still) {
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
