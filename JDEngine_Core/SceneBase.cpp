#include "pch.h"
#include "Component.h"
#include "GameObjectBase.h"
#include "ColliderBase.h"
#include "SceneBase.h"
#include "BoxCollider.h"

using namespace std;
namespace JDScene {
    void SceneBase::CheckCollision()
    {
        m_currPairs.clear(); // ó������ ���� �浹 ������ �ʱ�ȭ�ϰ� �����ϱ�.

        // ���� �浹�� �� ����
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

        // ���� ó�� (Enter, Stay)
        for (auto& curr : m_currPairs) {
            auto* colA = curr.A->GetComponent<JDComponent::ColliderBase>();
            auto* colB = curr.B->GetComponent<JDComponent::ColliderBase>();

            bool wasColliding = false;
            for (auto& prev : m_prevPairs) {
                if ((prev.A == curr.A && prev.B == curr.B) || (prev.A == curr.B && prev.B == curr.A)) {
                    wasColliding = true; // ���� �浹�ְ� ���� ���� �浹���� �ִٸ� true.(Stay)
                    break;
                }
            }
            if (colA->IsTrigger() || colB->IsTrigger()) {
                if (wasColliding) { // �������� �浹�� ���¶�� Stay.
                    colA->OnTriggerStay(colB);
                    colB->OnTriggerStay(colA);
                }
                else { // ���Ӱ� �浹������ Enter.
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

        // �浹 �� ó�� (Exit)
        for (auto& prev : m_prevPairs) {
            auto* colA = prev.A->GetComponent<JDComponent::ColliderBase>();
            auto* colB = prev.B->GetComponent<JDComponent::ColliderBase>();

            bool stillColliding = false; // ���� �浹�ְ� ���� ���� �浹���� ���ٸ� false.(Exit)
            for (auto& curr : m_currPairs) {
                if ((prev.A == curr.A && prev.B == curr.B) || (prev.A == curr.B && prev.B == curr.A)) {
                    stillColliding = true;
                    break;
                }
            }
            if (!stillColliding) { // �浹�� �����ٸ� Exit.
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
        // ���� �� �������� ���� �浹 ������ ���� ������ �Ѱ��ֱ�.
        m_prevPairs.swap(m_currPairs);
    }

}
