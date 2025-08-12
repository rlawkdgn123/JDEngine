#pragma once
#include "pch.h"
#include "framework.h"
#include "SceneBase.h"
#include "InputManager.h" 
#include <random>

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

        //∞≥±∏∏Æ
        JDGameObject::GameObject* m_frog = nullptr;
        JDComponent::TextureRenderer* m_frogTex = nullptr;
        JDComponent::AnimationRender* m_frogAnim = nullptr;

        float m_frogTimer = 0.0f;
        bool m_frogChanged = false;
        bool  m_frogLocked = false;
        float m_frogLockTimer = 0.0f;
        std::mt19937 rng{ std::random_device{}() };

        //¡„
        JDGameObject::GameObject* m_rat = nullptr;
        JDComponent::TextureRenderer* m_ratTex = nullptr;
        JDComponent::AnimationRender* m_ratAnim = nullptr;

        float m_ratTimer = 0.0f;
        bool m_ratChanged = false;
        bool  m_ratLocked = false;
        float m_ratLockTimer = 0.0f;

        //»∂∫“
        JDGameObject::GameObject* m_torch = nullptr;
        JDComponent::TextureRenderer* m_torchTex = nullptr;
        JDComponent::AnimationRender* m_torchAnim = nullptr;

        JDComponent::SFX* m_torchSfx;
    };
}