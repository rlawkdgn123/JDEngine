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
        ////////////////////////////////////////////////////////////////////////////////
        // RectTransform
        std::unique_ptr<UIObject> testUIObject = std::make_unique<UIObject>();

        auto rtf = testUIObject->GetComponent<RectTransform>();
        rtf->SetPosition({ 0.f, 0.f });
        rtf->SetScale({ 1.f, 1.f });
        rtf->SetSize({ 100.f, 100.f });

        ////////////////////////////////////////////////////////////////////////////////
        // UI_ImageComponent
        testUIObject->AddComponent<UI_ImageComponent>("Test");
        auto imageComponent = testUIObject->GetComponent<UI_ImageComponent>();

        ////////////////////////////////////////////////////////////////////////////////
        // UI_TextComponent
        testUIObject->AddComponent<UI_TextComponent>();
        auto textComponent = testUIObject->GetComponent<UI_TextComponent>();

        D2DRenderer& renderer = D2DRenderer::Instance();
        IDWriteTextFormat* textFormat = renderer.GetTextFormat();
        textComponent->SetTextFormat(textFormat);
        textComponent->SetText(L"Test");
        textComponent->SetColor(D2D1::ColorF(D2D1::ColorF::LightSkyBlue));

        ////////////////////////////////////////////////////////////////////////////////
        // UI_ButtonComponent
        testUIObject->AddComponent<UI_ButtonComponent>();
        auto buttonComponent = testUIObject->GetComponent<UI_ButtonComponent>();

        m_gameUiObjects.push_back(std::move(testUIObject));
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

    void TitleScene::Render(float dt) {
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
            D2DRenderer::Instance().RenderGameObject(*obj, dt);
        }

        for (auto& uiObj : m_gameUiObjects)
        {
            D2DRenderer::Instance().RenderUIObject(*uiObj);
        }
    }

}