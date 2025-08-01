#pragma once
#include "pch.h"
#include "framework.h"
#include "SceneBase.h"
#include "InputManager.h" 

namespace JDScene {

    class DefaultScene : public SceneBase
    {
    public:
        DefaultScene(SceneType type, std::string id) :SceneBase(type, id) {}

        ~DefaultScene() override {}

        void OnEnter() override;

        void OnLeave() override;

        void Update(float deltaTime) override;

        void FixedUpdate(float fixedDeltaTime) override;

        void LateUpdate(float deltaTime) override;

        void Render(float deltaTime) override;
    };

}
