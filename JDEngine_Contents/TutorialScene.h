#pragma once
#include "pch.h"
#include "framework.h"
#include "SceneBase.h"
#include "InputManager.h" 
#include "UIObject.h"
#include "UI_ImageComponent.h"
#include "UI_ButtonComponent.h"
#include "ColliderBase.h"
#include "CameraFader.h"
#include "ParticleSystem.h"
#include "BuildSystem.h"

using namespace JDGameObject::Content;
class GameTimer;

namespace JDScene {
    class TutorialScene : public SceneBase
    {
    public:
        TutorialScene(SceneType type, std::string id) : SceneBase(type, id) {}

        ~TutorialScene() override {}

        void OnEnter() override;

        void OnLeave() override;

        void Update(float deltaTime) override;

        void FixedUpdate(float fixedDeltaTime) override;

        void LateUpdate(float deltaTime) override;

        void Render(float deltaTime) override;

        void ClickUpdate();

        void ShowGridCreateMenu();

        void ShowGridSettingMenu();
    private:
        std::unique_ptr<ParticleSystem> m_lightParticles;
        Vector2F                        m_emitterPos;

        std::vector<bool> m_isOpen;

        CameraFader  m_fader;

        Image* m_Menu = nullptr;
        std::vector<Button*> m_menuButtons;
        Button* m_selectedTool = nullptr;
        JDComponent::ColliderBase* m_selectedCollider = nullptr;
        
        std::vector<Button*> m_gridSettingButtons;
        std::vector<Button*> m_gridCreateButtons;

        std::shared_ptr<Camera> m_camera;
        std::vector<std::shared_ptr<GameObject>> m_sceneObjects;
        std::vector<std::shared_ptr<UIObject>> m_UIObjects;
        std::vector<UIObject*> m_TutorialUIObjects;
        std::vector<GameObject*> m_TutorialObjects;

        std::unique_ptr<BuildSystem> m_BuildSystem;
    };
}