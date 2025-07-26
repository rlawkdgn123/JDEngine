#include "pch.h"
#include "framework.h"
#include "SceneList.h"
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
        cout << "[TestScene] OnEnter()\n";
        CreateGameObject<Player>(L"¤±¤¤¤·");
    }

    void TestScene::OnLeave() {
        cout << "[TestScene] OnLeave()\n";
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

    }

}
