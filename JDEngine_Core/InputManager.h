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

		bool Initialize(HWND hwnd); // ��ǲ�Ŵ��� �ʱ�ȭ

		bool OnHandleMessage(const MSG& msg); // Ű �Է� ������ �ڵ鿡 �ѱ��

		bool GetKeyPressed(UINT vk);

	protected:

		void HandleMsgKeyDown(WPARAM wParam, LPARAM lParam); // ������ ��

		void HandleMsgKeyUp(WPARAM wParam, LPARAM lParam); // ���� ��

		void HandleMsgMouse(const MSG& msg); // ���콺 �޽���

		void HandleRawInput(LPARAM lParam); // ������ �Է�

		void HandleKeyboardInput(RAWINPUT& raw);

		void HandleMouseInput(RAWINPUT& raw);

		MouseState m_CurMouse;

		std::array<bool, 256>    m_keyDown = {};   // ���� Down ����
		std::array<KeyEdge, 256> m_keyEdge = {};   // �̹� ������ Edge ���

	private:

		InputManager() = default;
		~InputManager() = default;

		InputManager(const InputManager&) = delete;
		InputManager& operator=(const InputManager&) = delete;
	};
}

