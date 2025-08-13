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
            if (!objA || !objA->IsActive()) continue;

            auto* colA = objA->GetComponent<JDComponent::ColliderBase>();
            if (!colA) continue;

            for (size_t j = i + 1; j < n; ++j) {
                auto* objB = m_gameObjects[j].get();
                if (!objB || !objB->IsActive()) continue;

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
            if (!colA || !colB) continue;

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
        auto isAlive = [&](JDGameObject::GameObjectBase* obj) -> bool {
            if (!obj) return false;
            for (auto& up : m_gameObjects) {
                if (up.get() == obj) return obj->IsActive(); // Active도 확인
            }
            return false;
            };

        for (auto& prev : m_prevPairs) {
            const bool aliveA = isAlive(prev.A);
            const bool aliveB = isAlive(prev.B);

            // 아직도 충돌 중인가?
            bool stillColliding = false;
            for (auto& curr : m_currPairs) {
                if ((prev.A == curr.A && prev.B == curr.B) ||
                    (prev.A == curr.B && prev.B == curr.A)) {
                    stillColliding = true;
                    break;
                }
            }
            if (stillColliding) continue;

            // Exit 이벤트: 살아있는 쪽에만 보낸다 (죽은 쪽 역참조 금지)
            JDComponent::ColliderBase* colA = (aliveA ? prev.A->GetComponent<JDComponent::ColliderBase>() : nullptr);
            JDComponent::ColliderBase* colB = (aliveB ? prev.B->GetComponent<JDComponent::ColliderBase>() : nullptr);

            if (colA && colB) {
                if (colA->IsTrigger() || colB->IsTrigger()) {
                    colA->OnTriggerExit(colB);
                    colB->OnTriggerExit(colA);
                }
                else {
                    colA->OnCollisionExit(colB);
                    colB->OnCollisionExit(colA);
                }
            }
            else if (colA) {
                // 반대쪽이 이미 사라졌다면, 필요하면 단독 Exit만 호출 (게임 규칙에 맞게 유지/삭제)
                colA->OnTriggerExit(nullptr); // 또는 스킵
            }
            else if (colB) {
                colB->OnTriggerExit(nullptr); // 또는 스킵
            }
        }
        // 다음 프레임을 위해 prev <- curr
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

            if (!col->IsRenderCollider())continue;

            auto* tm = obj->GetComponent<JDComponent::D2DTM::Transform>();
            if (!tm) continue;
            D2D1_MATRIX_3X2_F world = tm->GetWorldMatrix();

            // RenderGameObject와 동일하게 수정
            D2D1_MATRIX_3X2_F finalTransform = world;
            if (camera)
            {
                finalTransform = finalTransform * view; // view는 camera->GetViewMatrix()
            }
            finalTransform = finalTransform * tm->GetScreenCenterTranslation();
            D2DRenderer::Instance().SetTransform(finalTransform);

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

        // 6) 그리기 끝난 뒤, 렌더러의 Transform을 초기화합니다.
        D2DRenderer::Instance().SetTransform(D2D1::Matrix3x2F::Identity());
    }

    Vec2 SceneBase::GetMouseWorldPos()
    {
        // 1) 마우스 스크린 좌표 얻기
        auto& inputMgr = InputManager::Instance();
        auto  mousePos = inputMgr.GetMouseState().pos;
        D2D1_POINT_2F screenPoint = { static_cast<float>(mousePos.x), static_cast<float>(mousePos.y) };

        // 2) 최종 변환 행렬의 역행렬 계산
        D2D1_MATRIX_3X2_F finalTransform = D2D1::Matrix3x2F::Identity();
        auto camera = D2DRenderer::Instance().GetCamera();
        if (camera)
        {
            finalTransform = camera->GetViewMatrix();
        }

        // 임시 Transform 객체를 사용해 GetScreenCenterTranslation을 가져옵니다.
        JDComponent::D2DTM::Transform tempTransform;
        finalTransform = finalTransform * tempTransform.GetScreenCenterTranslation();

        // 최종 행렬의 역행렬을 구합니다. 이것이 화면->월드 변환 행렬입니다.
        D2D1InvertMatrix(&finalTransform);

        // 3) 스크린 좌표에 역행렬을 곱해 월드 좌표를 얻습니다.
        //    TransformPoint 멤버 함수 대신 직접 행렬 연산을 수행합니다.
        D2D1_POINT_2F worldPoint;
        worldPoint.x = screenPoint.x * finalTransform._11 + screenPoint.y * finalTransform._21 + finalTransform._31;
        worldPoint.y = screenPoint.x * finalTransform._12 + screenPoint.y * finalTransform._22 + finalTransform._32;


        // 4) 유니티 Y-up 좌표계에 맞게 Y 부호를 뒤집어줍니다.
        return { worldPoint.x, -worldPoint.y };

        // LEGACY : 과거 코드
        /*
        //// 1) 마우스 스크린 좌표 얻기
        //auto& inputMgr = InputManager::Instance();
        //auto  mousePos = inputMgr.GetMouseState().pos;    // {x, y}
        //Vec2 screenPos{ static_cast<float>(mousePos.x), static_cast<float>(mousePos.y) };

        //// 2) 스크린 → 월드 변환
        //Vec2 mouseWorld = screenPos;
        //auto camera = D2DRenderer::Instance().GetCamera();
        //if (camera) {
        //    auto invView = camera->GetViewMatrix();
        //    D2D1InvertMatrix(&invView);
        //    mouseWorld = camera->TransformPoint(invView, { screenPos.x,screenPos.y });
        //}

        //return mouseWorld;
        */

    }
}