#pragma once
#include "Component.h"
#include "InputManager.h"
#include "GameObjectBase.h"
#include "RectTransform.h"

namespace JDComponent {

    using RectTransform = JDComponent::D2DTM::RectTransform;
    using Vector2F = JDMath::Vector2F;

    enum class ButtonState      // 버튼 상태
    {
        Idle,
        Hovered,
        Pressed
    };

    struct CallbackInfo         // 콜백 정보를 담는 구조체
    {
        std::string name;
        std::function<void()> function;
    };

    class UI_ButtonComponent : public Component
    {
    public:
        // Component을(를) 통해 상속됨
        void Update(float deltaTime) override;

        void OnEvent(const std::string& event) override {};
        void OnEnable() override {};
        void OnDisable() override {};

        // 이벤트 추가 / 제거 함수들
        void AddOnDown(const std::string& name, std::function<void()> callback);
        void AddOnClick(const std::string& name, std::function<void()> callback);    // 마우스를 뗐을 때 (주요 액션)
        void AddOnEnter(const std::string& name, std::function<void()> callback);   // 마우스가 영역에 들어왔을 때
        void AddOnExit(const std::string& name, std::function<void()> callback);    // 마우스가 영역에서 나갔을 때
        void ClearAllCallbacks();

        // 상태 관련 함수들
        void SetInteractable(bool value) { m_Interactable = value; }
        bool GetInteractable() const { return m_Interactable; }
        ButtonState GetState() const { return m_State; }

        // Getter
        const std::vector<CallbackInfo>& GetOnDownCallbacks() const { return m_OnDownCallbacks; }
        const std::vector<CallbackInfo>& GetOnClickCallbacks() const { return m_OnClickCallbacks; }
        const std::vector<CallbackInfo>& GetOnEnterCallbacks() const { return m_OnEnterCallbacks; }
        const std::vector<CallbackInfo>& GetOnExitCallbacks() const { return m_OnExitCallbacks; }

    private:
        void UpdateState();
        bool IsMouseOver();
        void TriggerCallbacks(const std::vector<CallbackInfo>& callbacks);

        // 상태 변수들
        ButtonState m_State = ButtonState::Idle;
        bool m_Interactable = true;

        // 이전 프레임 상태 저장을 위한 변수들
        bool m_WasMouseDown = false;
        bool m_WasMouseOver = false;

        // 이벤트별 콜백 목록
        std::vector<CallbackInfo> m_OnDownCallbacks;
        std::vector<CallbackInfo> m_OnClickCallbacks;
        std::vector<CallbackInfo> m_OnEnterCallbacks;
        std::vector<CallbackInfo> m_OnExitCallbacks;
    };
}

