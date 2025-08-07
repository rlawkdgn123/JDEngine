#pragma once
#include "pch.h"
#include "framework.h"
#include "SceneBase.h"
#include "ParticleSystem.h"

namespace JDScene {

    using Image = JDGameObject::Content::Image;
    using Text = JDGameObject::Content::Text;
    using Button = JDGameObject::Content::Button;

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

        std::unique_ptr<ParticleSystem> m_lightParticles;
        Vector2F                        m_emitterPos;

        // 옵션창
        Image* m_optionUI = nullptr;
        Image* m_optionVolume = nullptr;
        Image* m_optionControl = nullptr;
        Image* m_optionCredit = nullptr;

        Button* m_closeOption = nullptr;
        Button* m_selectVolume = nullptr;
        Button* m_selectControl = nullptr;
        Button* m_selectCredit = nullptr;
	};
}



