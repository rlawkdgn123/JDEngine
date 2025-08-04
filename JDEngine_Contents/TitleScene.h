#pragma once
#include "pch.h"
#include "framework.h"
#include "SceneBase.h"

namespace JDScene {
	class TitleScene : public SceneBase
	{
    public:

        TitleScene(SceneType type, std::string id) : SceneBase(type, id) {}

        ~TitleScene() override {}

        void OnEnter() override;

        void OnLeave() override;

        void Update(float deltaTime) override;

        void FixedUpdate(float fixedDeltaTime) override;

        void LateUpdate(float deltaTime) override;

        void Render(float deltaTime) override;

        ////////////////////////////////////////////////////////////////////////////////

        void ClickUpdate();
    private:
        FMOD::Channel* m_hoverSfxChannel = nullptr;

	};
}



