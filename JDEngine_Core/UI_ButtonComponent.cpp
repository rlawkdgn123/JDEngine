#include "pch.h"
#include "framework.h"
#include "UI_ButtonComponent.h"


namespace JDComponent {

    ////////////////////////////////////////////////////////////////////////////////
    // 
    // 이벤트 추가/제거 함수 구현
    //
    void UI_ButtonComponent::AddOnDown(const std::string& name, std::function<void()> callback) 
    { 
        m_OnDownCallbacks.push_back({ name, callback }); 
    }

    void UI_ButtonComponent::AddOnClick(const std::string& name, std::function<void()> callback) 
    {
        m_OnClickCallbacks.push_back({ name, callback }); 
    }

    void UI_ButtonComponent::AddOnEnter(const std::string& name, std::function<void()> callback) 
    { 
        m_OnEnterCallbacks.push_back({ name, callback }); 
    }

    void UI_ButtonComponent::AddOnExit(const std::string& name, std::function<void()> callback) 
    { 
        m_OnExitCallbacks.push_back({ name, callback }); 
    }


    void UI_ButtonComponent::ClearAllCallbacks()
    {
        m_OnDownCallbacks.clear();
        m_OnClickCallbacks.clear();
        m_OnEnterCallbacks.clear();
        m_OnExitCallbacks.clear();
    }

    ////////////////////////////////////////////////////////////////////////////////

    void UI_ButtonComponent::Update(float deltaTime)
    {
        if (!m_Interactable)
            return;

        UpdateState();
    }

    void UI_ButtonComponent::UpdateState()
    {
        InputManager& input = InputManager::Instance();
        MouseState mouse = input.GetMouseState();

        bool isMouseDown = mouse.leftPressed;
        bool isMouseOver = IsMouseOver();

        // 1. 마우스 진입/이탈 이벤트 (OnEnter / OnExit)
        if (isMouseOver && !m_WasMouseOver)
        {
            TriggerCallbacks(m_OnEnterCallbacks);
        }
        if (!isMouseOver && m_WasMouseOver)
        {
            TriggerCallbacks(m_OnExitCallbacks);
        }

        // 2. 상태 결정 및 이벤트 발생
        if (isMouseOver)
        {
            // 마우스 버튼을 방금 눌렀을 때 (OnDown)
            if (isMouseDown && !m_WasMouseDown)
            {
                m_State = ButtonState::Pressed;
                TriggerCallbacks(m_OnDownCallbacks);
            }
            // 마우스 버튼을 방금 뗐을 때 (OnClick)
            else if (!isMouseDown && m_WasMouseDown)
            {
                // Pressed 상태에서 떼는 경우에만 OnClick 이벤트 발생
                if (m_State == ButtonState::Pressed)
                {
                    TriggerCallbacks(m_OnClickCallbacks);
                }
                m_State = ButtonState::Hovered;
            }
            // 마우스가 위에 있고 아무것도 누르지 않은 상태
            else if (!isMouseDown)
            {
                // 누르고 있다가 영역 밖으로 나갔다 들어온 경우를 위해 Hovered 상태 유지
                if (m_State != ButtonState::Pressed)
                    m_State = ButtonState::Hovered;
            }
        }
        else
        {
            m_State = ButtonState::Idle;
        }

        // 현재 상태를 다음 프레임에서 사용하기 위해 저장
        m_WasMouseDown = isMouseDown;
        m_WasMouseOver = isMouseOver;
    }

    // 콜백 실행 헬퍼 함수
    void UI_ButtonComponent::TriggerCallbacks(const std::vector<CallbackInfo>& callbacks)
    {
        for (const auto& callbackInfo : callbacks)
        {
            if (callbackInfo.function)
            {
                callbackInfo.function();
            }
        }
    }

    bool UI_ButtonComponent::IsMouseOver()
    {
        InputManager& inputManager = InputManager::Instance();
        const MouseState& mouseState = inputManager.GetMouseState();
        POINT point = mouseState.pos;
        Vector2F mousePos = { static_cast<float>(point.x), static_cast<float>(point.y) };

        RectTransform* rectTransform = m_Owner ? m_Owner->GetComponent<RectTransform>() : nullptr;
        if (!rectTransform) return false;

        // 1. RectTransform의 최종 월드 행렬과 그 역행렬을 가져옵니다.
        // (GetWorldMatrix()가 스크린 중앙 변환까지 모두 포함하고 있어야 합니다.)
        D2D1_MATRIX_3X2_F worldMatrix = rectTransform->GetWorldMatrix();
        D2D1_MATRIX_3X2_F inverseMatrix = worldMatrix;
        D2D1InvertMatrix(&inverseMatrix); // 역행렬 계산

        // 2. 마우스 좌표를 UI의 로컬 좌표계로 변환합니다.
        Vector2F localMousePos = {
            mousePos.x * inverseMatrix._11 + mousePos.y * inverseMatrix._21 + inverseMatrix._31,
            mousePos.x * inverseMatrix._12 + mousePos.y * inverseMatrix._22 + inverseMatrix._32
        };

        // 3. UI의 크기와 피벗을 이용해 '로컬 공간'에서의 사각형을 정의합니다.
        Vector2F size = rectTransform->GetSize();
        Vector2F pivot = rectTransform->GetPivot();

        D2D1_RECT_F localRect = {
            -pivot.x * size.x,
            -pivot.y * size.y,
            (1.0f - pivot.x) * size.x,
            (1.0f - pivot.y) * size.y
        };

        // 4. 변환된 '로컬 마우스 좌표'가 '로컬 사각형' 안에 있는지 검사합니다.
        return (localMousePos.x >= localRect.left && localMousePos.x <= localRect.right &&
            localMousePos.y >= localRect.top && localMousePos.y <= localRect.bottom);
    }
}
