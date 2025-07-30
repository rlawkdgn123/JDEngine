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
        CreateGameObject<Player>(L"¤±¤¤¤·");

        std::shared_ptr<GameObject> testObject = std::make_shared<GameObject>();

        auto tf = testObject->GetComponent<Transform>();
        tf->SetPosition({ 0.f, 0.f });
        tf->SetScale({ 1.f, 1.f });

        testObject->AddComponent<SpriteRenderer>("Test");

        m_gameObjects.push_back(testObject);


    }

    void TestScene::OnLeave() {
        //cout << "[TestScene] OnLeave()\n";
    }

    void TestScene::Update(float deltaTime) {
        SceneBase::Update(deltaTime);
        //cout << "[TestScene] Update() - dt: " << dt << "\n";
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
       //cout << "[TestScene] Render()\n";
        auto camera = D2DRenderer::Instance().GetCamera();

        if (camera)
        {
            D2DRenderer::Instance().SetTransform(camera->GetViewMatrix());
        }
        else
        {
            D2DRenderer::Instance().SetTransform(D2D1::Matrix3x2F::Identity());
        }

        for (auto& obj : m_gameObjects)
        {
            D2DRenderer::Instance().RenderGameObject(*obj);
        }
    }

}
