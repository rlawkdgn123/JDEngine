#include "pch.h"
#include "framework.h"
#include "UI_SliderComponent.h"
#include "GameObjectBase.h"

using namespace JDComponent;

////////////////////////////////////////////////////////////////////////////////
// 
// 이벤트 콜백 추가/제거 함수 구현

void UI_SliderComponent::AddOnValueChanged(const std::string& name, std::function<void(float)> callback) 
{ m_OnValueChangedCallbacks.push_back({ name, callback }); }

void UI_SliderComponent::AddOnEnter(const std::string& name, std::function<void()> callback) 
{ m_OnEnterCallbacks.push_back({ name, callback }); }

void UI_SliderComponent::AddOnExit(const std::string& name, std::function<void()> callback) 
{ m_OnExitCallbacks.push_back({ name, callback }); }

void UI_SliderComponent::AddOnDown(const std::string& name, std::function<void()> callback) 
{ m_OnDownCallbacks.push_back({ name, callback }); }

void UI_SliderComponent::AddOnUp(const std::string& name, std::function<void()> callback) 
{ m_OnUpCallbacks.push_back({ name, callback }); }

void UI_SliderComponent::ClearAllCallbacks() {
    m_OnValueChangedCallbacks.clear();
    m_OnEnterCallbacks.clear();
    m_OnExitCallbacks.clear();
    m_OnDownCallbacks.clear();
    m_OnUpCallbacks.clear();
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// 
// 핵심 로직

void UI_SliderComponent::Update(float deltaTime) {
    if (!m_Interactable) return;
    UpdateState();
}

void UI_SliderComponent::UpdateState() {
    InputManager& input = InputManager::Instance();
    MouseState mouse = input.GetMouseState();
    bool isMouseDown = mouse.leftPressed;
    bool isMouseOver = IsMouseOver();

    // 1. 마우스 진입/이탈 이벤트
    if (isMouseOver && !m_WasMouseOver) 
    {
        m_State = SliderState::Hovered;
        TriggerSimpleCallbacks(m_OnEnterCallbacks);
    }
    if (!isMouseOver && !m_WasMouseOver) {
        if (m_State == SliderState::Hovered) 
        {
            // 드래그 중이 아닐 때만 Idle로 전환
            m_State = SliderState::Idle;
        }

        TriggerSimpleCallbacks(m_OnExitCallbacks);
    }

    // 2. 누르기/떼기/드래그 이벤트
    if (isMouseDown && !m_WasMouseDown && isMouseOver) 
    {
        m_State = SliderState::Dragging;
        TriggerSimpleCallbacks(m_OnDownCallbacks);
        UpdateValueByMousePos({ (float)mouse.pos.x, (float)mouse.pos.y });
    }
    else if (!isMouseDown && m_WasMouseDown) 
    {
        if (m_State == SliderState::Dragging) 
        {
            m_State = isMouseOver ? SliderState::Hovered : SliderState::Idle;
            TriggerSimpleCallbacks(m_OnUpCallbacks);
        }
    }
    else if (isMouseDown && m_State == SliderState::Dragging) 
    {
        UpdateValueByMousePos({ (float)mouse.pos.x, (float)mouse.pos.y });
    }

    // 3. 현재 상태를 다음 프레임에서 사용하기 위해 저장
    m_WasMouseDown = isMouseDown;
    m_WasMouseOver = isMouseOver;
}

bool UI_SliderComponent::IsMouseOver() {
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

void UI_SliderComponent::UpdateValueByMousePos(const Vector2F& mousePos) {
    auto ownerRect = GetOwner()->GetComponent<RectTransform>();
    if (!ownerRect) return;

    D2D1_RECT_F worldRect = ownerRect->GetWorldRect();

    // 분모가 0이 되는 것을 방지
    float trackWidth = worldRect.right - worldRect.left;
    if (abs(trackWidth) < 1e-6) return;

    // 마우스 위치를 기반으로 0.0 ~ 1.0 사이의 정규화된 값을 계산합니다.
    float normalizedValue = (mousePos.x - worldRect.left) / trackWidth;

    // 값을 0.0과 1.0 사이로 제한합니다. (슬라이더 밖으로 드래그 시)
    normalizedValue = std::max(0.0f, std::min(1.0f, normalizedValue));

    SetValue(m_minValue + (m_maxValue - m_minValue) * normalizedValue);
}

void UI_SliderComponent::SetValue(float value, bool broadcast) {
    float newValue = std::max(m_minValue, std::min(m_maxValue, value));
    if (abs(m_currentValue - newValue) > 1e-6) { // 부동소수점 비교
        m_currentValue = newValue;
        UpdateVisuals();
        if (broadcast) {
            TriggerValueCallbacks(m_OnValueChangedCallbacks, m_currentValue);
        }
    }
}

void JDComponent::UI_SliderComponent::SetMinValue(float value)
{
    m_minValue = value;
    // 최대값이 최소값보다 작아지지 않도록 보정
    if (m_maxValue < m_minValue) {
        m_maxValue = m_minValue;
    }
    // 현재 값을 새로운 범위에 맞게 다시 조정
    SetValue(m_currentValue);
}

void JDComponent::UI_SliderComponent::SetMaxValue(float value)
{
    m_maxValue = value;
    // 최소값이 최대값보다 커지지 않도록 보정
    if (m_minValue > m_maxValue) {
        m_minValue = m_maxValue;
    }
    // 현재 값을 새로운 범위에 맞게 다시 조정
    SetValue(m_currentValue);
}

float UI_SliderComponent::GetNormalizedValue() const {
    if (abs(m_maxValue - m_minValue) < 1e-6) return 0.f;
    return (m_currentValue - m_minValue) / (m_maxValue - m_minValue);
}

void UI_SliderComponent::UpdateVisuals() {
    auto ownerRect = GetOwner()->GetComponent<RectTransform>();
    if (!ownerRect) return;

    float normalizedValue = GetNormalizedValue();
    float sliderWidth = ownerRect->GetSize().x;

    if (m_fillRect) {
        m_fillRect->SetSize({ sliderWidth * normalizedValue, m_fillRect->GetSize().y });
    }
    if (m_handleRect) {
        float handleX = -sliderWidth * 0.5f + sliderWidth * normalizedValue;
        m_handleRect->SetPosition({ handleX, m_handleRect->GetPosition().y });
    }
}

// 콜백 실행 헬퍼 함수들
void UI_SliderComponent::TriggerSimpleCallbacks(const std::vector<CallbackInfo>& callbacks) {
    for (const auto& cb : callbacks) { if (cb.function) cb.function(); }
}

void UI_SliderComponent::TriggerValueCallbacks(const std::vector<ValueCallbackInfo>& callbacks, float value) {
    for (const auto& cb : callbacks) { if (cb.function) cb.function(value); }
}
