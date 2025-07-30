#include "pch.h"
#include "framework.h"
#include "SceneList.h"
#include "TitleScene.h"

using namespace std;
using namespace JDGameObject::InGame;

namespace JDScene {

    // TitleScene
    void TitleScene::OnEnter() {
        using namespace JDGameObject;
        using namespace JDComponent;

        // UI
        std::shared_ptr<UIObject> testUIObject = std::make_shared<UIObject>();

        auto rtf = testUIObject->GetComponent<RectTransform>();
        rtf->SetPosition({ 0.f, 0.f });
        rtf->SetScale({ 1.f, 1.f });

        testUIObject->AddComponent<UI_ImageComponent>("Test");
        auto imageComponent = testUIObject->GetComponent<UI_ImageComponent>();

        m_UIObjects.push_back(testUIObject);
    }

    void TitleScene::OnLeave() {
        //cout << "[TestScene] OnLeave()\n";
    }

    void TitleScene::Update(float deltaTime) {
        SceneBase::Update(deltaTime);
        //cout << "[TestScene] Update() - dt: " << dt << "\n";
    }

    void TitleScene::FixedUpdate(float fixedDeltaTime) {
        SceneBase::FixedUpdate(fixedDeltaTime);
        //cout << "[TestScene] FixedUpdate() - dt: " << fixedDeltaTime << "\n";
    }

    void TitleScene::LateUpdate(float deltaTime) {
        SceneBase::LateUpdate(deltaTime);
        //cout << "[TestScene] LateUpdate() - dt: " << deltaTime << "\n";
    }

    void TitleScene::Render() {
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

        for (auto& uiObj : m_UIObjects)
        {
            D2DRenderer::Instance().RenderUIObject(*uiObj);
        }
    }

}