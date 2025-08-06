#pragma once
#include "pch.h"
#include "framework.h"
#include "SceneBase.h"
#include "ParticleSystem.h"

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

        std::unique_ptr<ParticleSystem> m_mouseParticles;
        std::unique_ptr<ParticleSystem> m_sakuraParticles;
        Vector2F                        m_emitterPos;

        Vector2F sakuraMin{ 900.0f, 0.0f };
        Vector2F sakuraMax{ 1920.0f, 500.0f };

        float   m_sakuraEmissionRate = 5.0f;
        float   m_sakuraEmitAccumulator = 0.0f;
	};
}



