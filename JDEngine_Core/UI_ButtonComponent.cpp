#include "pch.h"
#include "framework.h"
#include "UI_ButtonComponent.h"


namespace JDComponent {
    void UI_ButtonComponent::SetOnClick(std::function<void()> callback)
    {
        m_OnClick = callback;
    }

    void UI_ButtonComponent::Update(float deltaTime)
    {
        std::cout << "버튼 업데이트" << std::endl;

        if (!m_Interactable)
            return;

        UpdateState();
    }

    void UI_ButtonComponent::UpdateState()
    {
        InputManager& input = InputManager::Instance();
        MouseState mouse = input.GetMouseState();

        bool isMouseDown = mouse.leftPressed;       // 마우스 왼쪽 버튼이 눌려있는지 여부
        bool isMouseOver = IsMouseOver();           // 마우스가 버튼 위에 있는지 여부

        if (isMouseOver)
        {
            // Hover된 상태에서 누른 상태
            if (isMouseDown && !m_WasMouseDown)
            {
                m_State = ButtonState::Pressed;
                std::cout << "클릭" << std::endl;
            }

            // 눌렀다가 뗀 상태 ( 클릭 완료 )
            else if (!isMouseDown && m_WasMouseDown)
            {
                if (m_State == ButtonState::Pressed && m_OnClick)
                    m_OnClick();

                m_State = ButtonState::Hovered;
            }

            // 마우스를 올렸지만 클릭은 안 한 상태
            else if (!isMouseDown)
            {
                m_State = ButtonState::Hovered;
                std::cout << "호버" << std::endl;
            }
        }

        // 마우스가 버튼 위에 없을 때
        else
        {
            m_State = ButtonState::Idle;
            std::cout << "기본" << std::endl;
        }

        m_WasMouseDown = isMouseDown;
    }

    bool UI_ButtonComponent::IsMouseOver()
    {
        InputManager& inputManager = InputManager::Instance();
        const MouseState& mouseState = inputManager.GetMouseState();
        POINT point = mouseState.pos;
        Vector2F mousePos = { static_cast<float>(point.x), static_cast<float>(point.y) };

        RectTransform* rectTransform = m_Owner ? m_Owner->GetComponent<RectTransform>() : nullptr;
        if (!rectTransform) return false;

        Vector2F pos = rectTransform->GetPosition();
        Vector2F size = rectTransform->GetSize();

        D2D1_RECT_F rect = {
            pos.x,
            pos.y,
            pos.x + size.x,
            pos.y + size.y
        };

        return (mousePos.x >= rect.left && mousePos.x <= rect.right &&
            mousePos.y >= rect.top && mousePos.y <= rect.bottom);
    }
}
