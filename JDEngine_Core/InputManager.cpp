#include "pch.h"
#include "framework.h"
#include "InputManager.h"

#include "imgui.h"
// imgui.cpp에서 정의된 전역 컨텍스트 포인터를 사용하기 위해 extern 선언
extern IMGUI_API ImGuiContext* GImGui;

inline void GetVkName(const RAWKEYBOARD& kb, WCHAR* out, int cch) {
    if (cch <= 0 || !out) return;

    // 스캔코드+확장 플래그를 lParam 형식으로 변환
    UINT scan = kb.MakeCode;
    if (kb.Flags & RI_KEY_E0)
        scan |= 0xE000;            // 확장 키(예: 오른쪽 Ctrl, 화살표 등)

    LONG lParam = static_cast<LONG>(scan << 16);
    if (kb.Flags & RI_KEY_E0)
        lParam |= (1 << 24);

    if (!GetKeyNameTextW(lParam, out, cch))
        swprintf_s(out, cch, L"VK 0x%02X", kb.VKey); // 실패 시 VK 코드로 대체
}
bool InputManager::Initialize(HWND hwnd)
{
    RAWINPUTDEVICE rid[2] = {};

    // 키보드
    rid[0].usUsagePage = 0x01;
    rid[0].usUsage = 0x06; // Keyboard
    rid[0].dwFlags = 0;
    rid[0].hwndTarget = hwnd;

    // 마우스
    rid[1].usUsagePage = 0x01;
    rid[1].usUsage = 0x02; // Mouse
    rid[1].dwFlags = 0;
    rid[1].hwndTarget = hwnd;

    if (!RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE)))

        return true;
}

bool InputManager::GetKeyPressed(UINT vk)
{
    bool p = m_keyEdge[vk].pressed;
    m_keyEdge[vk].pressed = false;
    return p;
}


bool InputManager::OnHandleMessage(const MSG& msg)
{
    if (GImGui == nullptr)
    {
        // 아직 ImGui가 준비되지 않았으므로, 모든 입력을 InputManager가 처리하도록 합니다.
        // (또는 return false; 하여 기본 처리되게 할 수도 있지만, 기존 로직 유지를 위해 이대로 둡니다)
        switch (msg.message)
        {
        case WM_INPUT: 
            HandleRawInput(msg.lParam);
            break;

        case WM_KEYDOWN: 
            HandleMsgKeyDown(msg.wParam, msg.lParam); 
            break;

        case WM_KEYUP:
            HandleMsgKeyUp(msg.wParam, msg.lParam); 
            break;

        case WM_MOUSEMOVE: 
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP: 
        case WM_RBUTTONDOWN: 
        case WM_RBUTTONUP: 
            HandleMsgMouse(msg); 
            break;

        default: return false;
        }
        return true;
    }

    ImGuiIO& io = ImGui::GetIO();

    switch (msg.message)
    {
    case WM_INPUT:
    {
        // ImGui가 키보드를 사용 중이면, Raw Input을 무시합니다.
        if (io.WantCaptureKeyboard)
        {
            return false; // 처리하지 않았다고 알림
        }

        HandleRawInput(msg.lParam);
    }
    break;

    case WM_KEYDOWN:
    {
        // ImGui가 키보드를 사용 중이면, KeyDown 메시지를 무시합니다.
        if (io.WantCaptureKeyboard)
        {
            return false;
        }

        HandleMsgKeyDown(msg.wParam, msg.lParam);
    }
    break;

    case WM_KEYUP:
    {
        // ImGui가 키보드를 사용 중이면, KeyUp 메시지를 무시합니다.
        if (io.WantCaptureKeyboard)
        {
            return false;
        }

        HandleMsgKeyUp(msg.wParam, msg.lParam);
    }
    break;

    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    {
        // ImGui가 마우스를 사용 중이면, 마우스 메시지를 무시합니다.
        if (io.WantCaptureMouse)
        {
            return false;
        }

        HandleMsgMouse(msg);
    }
    break;

    default:
        return false; // Unhandled message
    }

    return true;
}

void InputManager::HandleMsgKeyDown(WPARAM wParam, LPARAM lParam)
{
    // lParam: 비트 30 = 이전 상태(1이면 이미 Down), 비트 0-15 = 반복 수
    bool wasDown = (lParam & (1 << 30)) != 0;
    UINT repeat = lParam & 0xFFFF;

    char buf[128];
    std::snprintf(buf, sizeof(buf),
        "KEY DOWN  VK=0x%02X  repeat=%u  (wasDown=%d)\n",
        static_cast<unsigned int>(wParam), repeat, wasDown);

    // std::cout << buf;
}

void InputManager::HandleMsgKeyUp(WPARAM wParam, LPARAM lParam)
{
    char buf[64];
    std::snprintf(buf, sizeof(buf),
        "KEY UP    VK=0x%02X\n",
        static_cast<unsigned int>(wParam));

    //  std::cout << buf;
}

void InputManager::HandleMsgMouse(const MSG& msg)
{
    int x = GetXFromLParam(msg.lParam);
    int y = GetYFromLParam(msg.lParam);

    m_CurMouse.pos = { x, y };

    if (msg.message == WM_LBUTTONDOWN)
    {
        m_CurMouse.leftPressed = true;
        SetCapture(msg.hwnd);
    }
    else if (msg.message == WM_RBUTTONDOWN)
    {
        m_CurMouse.rightPressed = true;
        SetCapture(msg.hwnd);
    }
    else if (msg.message == WM_LBUTTONUP)
    {
        m_CurMouse.leftPressed = false;
        ReleaseCapture();
    }
    else if (msg.message == WM_RBUTTONUP)
    {
        m_CurMouse.rightPressed = false;
        ReleaseCapture();
    }

    if (msg.message == WM_MOUSEMOVE)
    {
        m_CurMouse.delta.x = x - m_CurMouse.pos.x;
        m_CurMouse.delta.y = y - m_CurMouse.pos.y;
    }
    else
    {
        m_CurMouse.delta.x = 0;
        m_CurMouse.delta.y = 0;
    }

    char buf[128];
    std::snprintf(buf, sizeof(buf),
        "MOUSE (%d,%d)  left=%d right=%d\n",
        x, y, m_CurMouse.leftPressed, m_CurMouse.rightPressed);

    std::cout << buf;
}

void InputManager::HandleRawInput(LPARAM lParam)
{
    UINT dwSize = 0;
    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));

    if (dwSize == 0)
    {
        // 오류 처리: dwSize가 0인 경우는 잘못된 입력이거나 지원되지 않는 입력일 수 있음
        if (GetLastError() != ERROR_SUCCESS)
        {
            std::cerr << "GetRawInputData failed with error: " << GetLastError() << std::endl;
        }
        return;
    }

    std::unique_ptr<BYTE[]> lpb(new BYTE[dwSize]);

    if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb.get(), &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
    {
        std::cerr << "GetRawInputData failed to read data." << std::endl;
        return;
    }

    RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(lpb.get());

    if (raw->header.dwType == RIM_TYPEKEYBOARD)
    {
        HandleKeyboardInput(*raw);
    }
    else if (raw->header.dwType == RIM_TYPEMOUSE)
    {
        HandleMouseInput(*raw);
    }
}

void InputManager::HandleKeyboardInput(RAWINPUT& raw)
{
    RAWKEYBOARD& kb = raw.data.keyboard;
    bool breakCode = (kb.Flags & RI_KEY_BREAK) != 0;   // 1 = KeyUp
    UINT vk = kb.VKey;

    if (vk >= 0xFF)  return;      // 0xFF = 잘못된 키 신호


    KeyEdge& edge = m_keyEdge[vk];
    bool& down = m_keyDown[vk];

    if (breakCode) // KeyUp
    {
        if (true == down) edge.released = true;
        down = false;

        std::cout << "Key Up: VK " << vk << std::endl;
    }
    else // KeyDown
    {
        if (false == down) edge.pressed = true;
        down = true;

        std::cout << "Key Down: VK " << vk << std::endl;
    }
}

void InputManager::HandleMouseInput(RAWINPUT& raw)
{
    
}
