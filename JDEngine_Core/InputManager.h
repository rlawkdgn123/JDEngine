#pragma once
#include "InputUtil.h"

struct MouseState
{
	POINT  pos{ 0, 0 };
	POINT  delta{ 0, 0 };

	bool   leftPressed{ false };
	bool   rightPressed{ false };
};

struct KeyEdge
{
	bool pressed = false;
	bool released = false;
};

namespace JDModule {
	class InputManager
	{
	public:
		static InputManager& Instance()
		{
			// Simple singleton implementation
			static InputManager instance;

			return instance;
		}

		bool Initialize(HWND hwnd); // 인풋매니저 초기화

		bool OnHandleMessage(const MSG& msg); // 키 입력 정보를 핸들에 넘기기

		bool GetKeyPressed(UINT vk);

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

	private:

		InputManager() = default;
		~InputManager() = default;

		InputManager(const InputManager&) = delete;
		InputManager& operator=(const InputManager&) = delete;
	};
}

