#pragma once
#include "Component.h"
#include "InputManager.h"
#include "GameObjectBase.h"
#include "RectTransform.h"
#include "UI_ButtonComponent.h"

namespace JDComponent {

    using CallbackInfo = JDComponent::CallbackInfo;
    using RectTransform = JDComponent::D2DTM::RectTransform;
    using Vector2F = JDMath::Vector2F;

    // 슬라이더의 상태 정의
    enum class SliderState
    {
        Idle,
        Hovered,
        Dragging
    };

    // float 값을 인자로 받는, 값 변경 콜백 정보 구조체
    struct ValueCallbackInfo
    {
        std::string name;
        std::function<void(float)> function;
    };

    class UI_SliderComponent : public Component
    {
    public:
        // Component 상속 메서드
        void Update(float deltaTime) override;
        void OnEvent(const std::string& event) override {};
        void OnEnable() override {};
        void OnDisable() override {};

        // 이벤트 추가/제거 함수들
        void AddOnValueChanged(const std::string& name, std::function<void(float)> callback);
        void AddOnEnter(const std::string& name, std::function<void()> callback);
        void AddOnExit(const std::string& name, std::function<void()> callback);
        void AddOnDown(const std::string& name, std::function<void()> callback);
        void AddOnUp(const std::string& name, std::function<void()> callback);
        void ClearAllCallbacks();

        // 상태 및 값 관련 함수들
        void        SetInteractable(bool value) { m_Interactable = value; }
        bool        GetInteractable() const { return m_Interactable; }

        SliderState GetState() const { return m_State; }

        void        SetValue(float value, bool broadcast = true);
        float       GetValue() const { return m_currentValue; }

        void        SetMinValue(float value);
        float       GetMinValue() const { return m_minValue; }
        void        SetMaxValue(float value);
        float       GetMaxValue() const { return m_maxValue; }

        float       GetNormalizedValue() const;

        // 시각적 요소 연결
        void SetFillRect(RectTransform* rect) { m_fillRect = rect; }
        void SetHandleRect(RectTransform* rect) { m_handleRect = rect; }

        // ImGui를 위한 Getter
        const std::vector<ValueCallbackInfo>& GetOnValueChangedCallbacks() const { return m_OnValueChangedCallbacks; }
        const std::vector<CallbackInfo>& GetOnEnterCallbacks() const { return m_OnEnterCallbacks; }
        const std::vector<CallbackInfo>& GetOnExitCallbacks() const { return m_OnExitCallbacks; }
        const std::vector<CallbackInfo>& GetOnDownCallbacks() const { return m_OnDownCallbacks; }
        const std::vector<CallbackInfo>& GetOnUpCallbacks() const { return m_OnUpCallbacks; }

    private:
        void UpdateState();
        bool IsMouseOver();
        void UpdateValueByMousePos(const Vector2F& mousePos);
        void UpdateVisuals();

        // 콜백 실행 헬퍼 함수
        void TriggerSimpleCallbacks(const std::vector<CallbackInfo>& callbacks);
        void TriggerValueCallbacks(const std::vector<ValueCallbackInfo>& callbacks, float value);

    private:
        // 상태 변수들
        SliderState m_State = SliderState::Idle;
        bool m_Interactable = true;

        // 이전 프레임 상태 저장용 변수들
        bool m_WasMouseDown = false;
        bool m_WasMouseOver = false;

        // 값 변수들
        float m_minValue = 0.0f;
        float m_maxValue = 1.0f;
        float m_currentValue = 1.0f;

        // 제어할 자식 UI 요소의 RectTransform
        RectTransform* m_fillRect = nullptr;
        RectTransform* m_handleRect = nullptr;

        // 이벤트별 콜백 목록
        std::vector<ValueCallbackInfo> m_OnValueChangedCallbacks;
        std::vector<CallbackInfo> m_OnEnterCallbacks;
        std::vector<CallbackInfo> m_OnExitCallbacks;
        std::vector<CallbackInfo> m_OnDownCallbacks;
        std::vector<CallbackInfo> m_OnUpCallbacks;
    };
}