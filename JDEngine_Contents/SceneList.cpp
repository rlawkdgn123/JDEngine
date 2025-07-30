#include "pch.h"
#include "framework.h"
#include "SceneList.h"
#include "BoxCollider.h"
#include "CircleCollider.h"

using namespace std;
using namespace JDGameObject::InGame;

namespace JDScene {


    void DefaultScene::OnEnter() {}

    void DefaultScene::OnLeave() {}

    void DefaultScene::Update(float deltaTime) { SceneBase::Update(deltaTime); }

    void DefaultScene::FixedUpdate(float fixedDeltaTime) { SceneBase::FixedUpdate(fixedDeltaTime); }

    void DefaultScene::LateUpdate(float deltaTime) { SceneBase::LateUpdate(deltaTime); }

    void DefaultScene::Render() {}



    // TestScene
    void TestScene::OnEnter() {
        using namespace JDGameObject;
        using namespace JDComponent;

        //cout << "[TestScene] OnEnter()\n";
        CreateGameObject<Player>(L"ㅁㄴㅇ");

        std::shared_ptr<GameObject> testObject = std::make_shared<GameObject>();

        auto tf = testObject->GetComponent<Transform>();
        tf->SetPosition({ 0.f, 0.f });
        tf->SetScale({ 1.f, 1.f });

        testObject->AddComponent<SpriteRenderer>("Test");

        m_sceneObjects.push_back(testObject);

        const float startX = -500.0f;
        const float startY = 300.0f;
        const float spacingX = 100.0f;
        const float spacingY = -100.0f;

        for (int col = 0; col < 5; ++col) {
            for (int row = 0; row < 7; ++row) {
                // 고유 이름 생성 (원하면 동일명으로 해도 됩니다)
                std::wstring name = L"Box_" + std::to_wstring(col) + L"_" + std::to_wstring(row);

                // GameObject 생성
                auto* box = CreateGameObject<Grid>(name.c_str());

                // 위치 설정
                float x = startX + spacingX * col;
                float y = startY + spacingY * row;
                box->GetComponent<JDComponent::D2DTM::Transform>()->SetPosition({ x, y });

                // 콜라이더 추가
                box->AddComponent<JDComponent::BoxCollider>(JDGlobal::Math::Vector2F{ 47.0f,47.0f });
            }
        }
    }

    void TestScene::OnLeave() {
        //cout << "[TestScene] OnLeave()\n";
    }

    void TestScene::Update(float deltaTime) {
        SceneBase::Update(deltaTime);

        // 1) 마우스 스크린 좌표 얻기
        auto& inputMgr = InputManager::Instance();
        auto  mouseState = inputMgr.GetMouseState().pos;    // {x, y} 픽셀
        JDGlobal::Math::Vector2F screenPos{ static_cast<float>(mouseState.x), static_cast<float>(mouseState.y) };

        // 2) 스크린 → 월드 변환
        JDGlobal::Math::Vector2F mouseWorld;
        auto camera = D2DRenderer::Instance().GetCamera();
        if (camera) {
            auto invView = camera->GetViewMatrix();
            D2D1InvertMatrix(&invView); 
            mouseWorld = m_camera->TransformPoint(invView, { screenPos.x,screenPos.y });
        }
        else {
            mouseWorld = screenPos;
        }

        //HandleMouseHover(mouseWorld);

        // 3) 하이라이트 인덱스 계산
        m_highlightedIndex = -1;
        for (int i = 0; i < (int)m_gameObjects.size(); ++i) {
            auto& obj = m_gameObjects[i];
            if (!obj) continue;

            auto* col = obj->GetComponent<JDComponent::BoxCollider>();
            if (!col) continue;

            auto* tm = obj->GetComponent<JDComponent::D2DTM::Transform>();
            auto  pos = tm->GetPosition();
            auto  hsize = col->GetHalfSize();
            auto  offset = col->GetColliderOffset();

            float left = pos.x + offset.x - hsize.x;
            float top = pos.y + offset.y + hsize.y;
            float right = pos.x + offset.x + hsize.x;
            float bottom = pos.y + offset.y - hsize.y;
            
            if (mouseWorld.x >= left && mouseWorld.x <= right &&
                mouseWorld.y <= top && mouseWorld.y >= bottom)
            {
                m_highlightedIndex = i;
                break;
            }
        }
    }


    void TestScene::FixedUpdate(float fixedDeltaTime) {
        SceneBase::FixedUpdate(fixedDeltaTime);
        //cout << "[TestScene] FixedUpdate() - dt: " << fixedDeltaTime << "\n";
    }

    void TestScene::LateUpdate(float deltaTime) {
        SceneBase::LateUpdate(deltaTime);
        //cout << "[TestScene] LateUpdate() - dt: " << deltaTime << "\n";
    }

    void TestScene::Render() {
        auto camera = D2DRenderer::Instance().GetCamera();

        if (camera)
            D2DRenderer::Instance().SetTransform(camera->GetViewMatrix());
        else
            D2DRenderer::Instance().SetTransform(D2D1::Matrix3x2F::Identity());

        // 콜라이더 그리기 (하이라이트 색 분기)
        for (int i = 0; i < (int)m_gameObjects.size(); ++i) {
            auto& obj = m_gameObjects[i];
            if (!obj) continue;

            auto* col = obj->GetComponent<JDComponent::BoxCollider>();
            if (!col) continue;

            auto* tm = obj->GetComponent<JDComponent::D2DTM::Transform>();
            auto pos = tm->GetPosition();
            auto hsize = col->GetHalfSize();
            auto offset = col->GetColliderOffset();

            float left = pos.x + offset.x - hsize.x;
            float top = pos.y + offset.y + hsize.y;
            float right = pos.x + offset.x + hsize.x;
            float bottom = pos.y + offset.y - hsize.y;

            // 하이라이트된 인덱스일 때 빨강, 아니면 기본 검정
            UINT32 color = (i == m_highlightedIndex)
                ? 0xFFFF0000  // 빨강
                : 0xFF000000; // 검정

            D2DRenderer::Instance().DrawRectangle(
                left, top, right, bottom,
                color
            );
        }

        // 게임 오브젝트 렌더
        for (auto& obj : m_sceneObjects) {
            D2DRenderer::Instance().RenderGameObject(*obj);
        }
    }
}
