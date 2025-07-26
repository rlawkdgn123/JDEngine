#include "pch.h"
#include "framework.h"
#include "SceneList.h"
using namespace std;
using namespace JDGameObject::InGame;

namespace JDScene {


    void DefaultScene::OnEnter() {}

    void DefaultScene::OnLeave() {}

    void DefaultScene::Update(float dt) {}

    void DefaultScene::FixedUpdate(float fixedDeltaTime) {}

    void DefaultScene::LateUpdate(float deltaTime) {}

    void DefaultScene::Render() {}



    // TestScene
    void TestScene::OnEnter() {
        cout << "[TestScene] OnEnter()\n";
        CreateGameObject<Player>(L"¤±¤¤¤·");
    }

    void TestScene::OnLeave() {
        cout << "[TestScene] OnLeave()\n";
    }

    void TestScene::Update(float dt) {
        //cout << "[TestScene] Update() - dt: " << dt << "\n";
    }

    void TestScene::FixedUpdate(float fixedDeltaTime) {
        //cout << "[TestScene] FixedUpdate() - dt: " << fixedDeltaTime << "\n";
    }

    void TestScene::LateUpdate(float deltaTime) {
        //cout << "[TestScene] LateUpdate() - dt: " << deltaTime << "\n";
    }

    void TestScene::Render() {
       //cout << "[TestScene] Render()\n";

    }

}
