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

using namespace JDGameObject::Content;

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

        void ShowEmptyMenu();

        void ShowFilledMenu();
    private:
        std::unique_ptr<ParticleSystem> m_lightParticles;
        Vector2F                        m_emitterPos;

        int m_totalCols = 4;
        int m_totalRows = 6;
        std::vector<bool> m_isOpen;

        CameraFader  m_fader;

        Image* m_Menu = nullptr;
        std::vector<Button*> m_menuButtons;
        Button* m_selectedTool = nullptr;
        JDComponent::ColliderBase* m_selectedCollider = nullptr;
        
        std::vector<Button*> m_emptyButtons;
        std::vector<Button*> m_filledButtons;

        std::shared_ptr<Camera> m_camera;
        std::vector<std::shared_ptr<GameObject>> m_sceneObjects;
        std::vector<std::shared_ptr<UIObject>> m_UIObjects;
        std::vector<UIObject*> m_TutorialUIObjects;
        std::vector<GameObject*> m_TutorialObjects;
    };
}