#pragma once
#include "pch.h"
#include "framework.h"
#include "SceneBase.h"
 
namespace JDScene {

    class DefaultScene : public SceneBase
    {
    public:
        DefaultScene(SceneType type, std::string id) :SceneBase(type, id) {}

        ~DefaultScene() override {}

        void OnEnter() override;

        void OnLeave() override;

        void Update(float dt) override;

        void FixedUpdate(float fixedDeltaTime);

        void LateUpdate(float deltaTime);

        void Render() override;

        void GameObjCreate();
    };

    class TestScene : public SceneBase
    {
    public:
        TestScene(SceneType type, std::string id) : SceneBase(type, id) {}

        ~TestScene() override {}

        void OnEnter() override;

        void OnLeave() override;

        void Update(float dt) override;

        void FixedUpdate(float fixedDeltaTime);

        void LateUpdate(float deltaTime);

        void Render() override;

        void GameObjCreate();
    };

}
