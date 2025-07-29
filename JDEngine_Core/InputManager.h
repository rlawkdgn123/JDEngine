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

	bool Initialize(HWND hwnd); // ��ǲ�Ŵ��� �ʱ�ȭ

	bool OnHandleMessage(const MSG& msg); // Ű �Է� ������ �ڵ鿡 �ѱ��

	bool GetKeyPressed(UINT vk);

	MouseState GetMouseState() const { return m_CurMouse; }

	bool IsKeyDown(UINT vk) const {
		if (vk >= 0 && vk < 256) {
			//std::cout << "[IsKeyDown] vk=" << vk << ", down=" << m_keyDown[vk] << std::endl;
			return m_keyDown[vk];
		}
		return false;
	}

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

};
