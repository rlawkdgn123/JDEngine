#include "pch.h"
#include "Component.h"
#include "GameObjectBase.h"
#include "ColliderBase.h"
#include "SceneBase.h"
#include "BoxCollider.h"

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


}
