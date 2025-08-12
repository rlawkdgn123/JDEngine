#pragma once
#include "pch.h"
#include "framework.h"
#include "SceneBase.h"
#include "InputManager.h" 
#include <random>
#include "ParticleSystem.h"

namespace JDScene {
    class TestScene : public SceneBase
    {
    public:
        TestScene(SceneType type, std::string id) : SceneBase(type, id) {}

        ~TestScene() override {}

        void OnEnter() override;

        void OnLeave() override;

        void Update(float deltaTime) override;

        void FixedUpdate(float fixedDeltaTime) override;

        void LateUpdate(float deltaTime) override;

        void Render(float deltaTime) override;

        void ClickUpdate();
    private:
        int m_totalCols = 4;
        int m_totalRows = 6;
        std::vector<bool> m_isOpen;

        std::shared_ptr<Camera> m_camera;
        std::vector<std::shared_ptr<GameObject>> m_sceneObjects;
        std::vector<std::shared_ptr<UIObject>> m_UIObjects;

        //ÆÄÆ¼Å¬
        std::unique_ptr<ParticleSystem> m_mouseParticles;
        std::unique_ptr<ParticleSystem> m_leafParticles;
        std::unique_ptr<ParticleSystem> m_smokeParticles;
        std::unique_ptr<ParticleSystem> m_footdustParticles;
        std::unique_ptr<ParticleSystem> m_waveParticles;

        float m_torchSmokeAcc = 0.f;
        bool  m_emitTorchSmoke = false;
        float m_torchSmokePeriod = 0.2f;

        bool  m_emitleaf = false;
        bool  m_emitwave = false;
        //°³±¸¸®
        JDGameObject::GameObject* m_frog = nullptr;
        JDComponent::TextureRenderer* m_frogTex = nullptr;
        JDComponent::AnimationRender* m_frogAnim = nullptr;

        float m_frogTimer = 0.0f;
        bool m_frogChanged = false;
        bool  m_frogLocked = false;
        float m_frogLockTimer = 0.0f;
        std::mt19937 rng{ std::random_device{}() };

        //Áã
        JDGameObject::GameObject* m_rat = nullptr;
        JDComponent::TextureRenderer* m_ratTex = nullptr;
        JDComponent::AnimationRender* m_ratAnim = nullptr;

        float m_ratTimer = 0.0f;
        bool m_ratChanged = false;
        bool  m_ratLocked = false;
        float m_ratLockTimer = 0.0f;

        //È¶ºÒ
        JDGameObject::GameObject* m_torch = nullptr;
        JDComponent::TextureRenderer* m_torchTex = nullptr;
        JDComponent::AnimationRender* m_torchAnim = nullptr;

        JDComponent::SFX* m_torchSfx;
    };
}