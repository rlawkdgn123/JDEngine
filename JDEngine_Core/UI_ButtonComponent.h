#pragma once
#include "Component.h"
#include "InputManager.h"
#include "GameObjectBase.h"
#include "RectTransform.h"

namespace JDComponent {

    using RectTransform = JDComponent::D2DTM::RectTransform;
    using Vector2F = JDMath::Vector2F;

    enum class ButtonState
    {
        Idle,
        Hovered,
        Pressed
    };

    class UI_ButtonComponent : public Component
    {
    public:
        // Component을(를) 통해 상속됨
        void Update(float deltaTime) override;

        void OnEvent(const std::string& event) override {};
        void OnEnable() override {};
        void OnDisable() override {};

        void SetOnClick(std::function<void()> callback);
        void SetInteractable(bool value) { m_Interactable = value; }

        ButtonState GetState() const { return m_State; }

    private:
        ButtonState m_State = ButtonState::Idle;
        bool m_Interactable = true;
        bool m_WasMouseDown = true;

        std::function<void()> m_OnClick;

        void UpdateState();
        bool IsMouseOver();
    };
}

