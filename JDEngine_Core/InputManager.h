#pragma once
#include <array>
#include "../JDEngine_Common/InputUtil.h"

struct MouseState
{
	POINT pos{ 0, 0 };
	POINT delta{ 0, 0 };

	bool leftPressed{ false };
	bool rightPressed{ false };

	bool leftClicked{ false };
	bool rightClicked{ false };
};

struct KeyEdge
{
	bool pressed = false;
	bool released = false;
};

 
class InputManager
{
public:

	static InputManager& Instance()
	{
		static InputManager instance;

		return instance;
	}

private:

	InputManager() = default;
	~InputManager() = default;

	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;

public:

	bool Initialize(HWND hwnd); // 인풋매니저 초기화

	bool OnHandleMessage(const MSG& msg, bool imguiHandled = false);

	bool GetKeyPressed(UINT vk);

	MouseState GetMouseState() const { return m_CurMouse; }

	MouseState* SetMouseState() { return &m_CurMouse; }

	bool IsKeyDown(UINT vk) const {
		if (vk >= 0 && vk < 256) {
			//std::cout << "[IsKeyDown] vk=" << vk << ", down=" << m_keyDown[vk] << std::endl;
			return m_keyDown[vk];
		}
		return false;
	}

	void BeginFrame() {
		m_CurMouse.leftClicked = false;
		m_CurMouse.rightClicked = false;
		m_CurMouse.delta = { 0, 0 }; // 이번 프레임 델타 누적 안 쓰면 0으로 시작
	}
protected:

	void HandleMsgKeyDown(WPARAM wParam, LPARAM lParam); // 눌렀을 때

	void HandleMsgKeyUp(WPARAM wParam, LPARAM lParam); // 뗐을 때

	void HandleMsgMouse(const MSG& msg); // 마우스 메시지

	void HandleRawInput(LPARAM lParam); // 고정밀 입력

	void HandleKeyboardInput(RAWINPUT& raw);

	void HandleMouseInput(RAWINPUT& raw);

	MouseState m_CurMouse;

	std::array<bool, 256>    m_keyDown = {};   // 현재 Down 상태
	std::array<KeyEdge, 256> m_keyEdge = {};   // 이번 프레임 Edge 결과

};
