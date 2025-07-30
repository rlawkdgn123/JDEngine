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
        using Vector2F = JDGlobal::Math::Vector2F;

        // UI
        ////////////////////////////////////////////////////////////////////////////////
        
        auto image = CreateUIObject<Image>(L"건물 제작 배경");
        image->SetPosition(Vector2F{ 100.0f, 100.0f });

        auto image01 = CreateUIObject<Image>(L"건물 제작 배경");
        image01->SetPosition(Vector2F{ 150.0f, 100.0f });

        auto image02 = CreateUIObject<Image>(L"건물 제작 배경");
        image02->SetPosition(Vector2F{ 100.0f, 150.0f });

        auto image03 = CreateUIObject<Image>(L"건물 제작 배경");
        image03->SetPosition(Vector2F{ 200.0f, 200.0f });
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