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
        
        CreateUIObject<Button>(L"타이틀 버튼");

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