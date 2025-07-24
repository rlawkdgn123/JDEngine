#include "pch.h"
#include "Framework.h"
////////////////////////////////////////////////////////////////////////////
/*
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
*/
/////////////////////////////////////////////////////////////////////////
#include <shobjidl.h>            // IFileOpenDialog
#include <filesystem>            // C++17 std::filesystem
#include <assert.h>
/////////////////////////////////////////////////////////////////////////
#include "EngineCore.h"
/////////////////////////////////////////////////////////////////////////
// ���� �ִϸ��̼� �κ��� �и� ����
using namespace std;

using GameTimer = JDEngine::GameTimer;
using D2DRenderer = JDEngine::D2DRenderer;
using InputManager = JDEngine::InputManager;
using SceneManager = JDEngine::SceneManager;
// (�Ϲ������� UTF-8 �Ǵ� �����Ϻ� ��Ƽ����Ʈ ���ڵ�)��
// std::wstring (�Ϲ������� Windows���� UTF-16) ���� ��ȯ�� ���� �� ���� ��ƿ��Ƽ �Լ�

std::wstring ConvertToWString(const std::string& str)
{
    size_t len = 0;
    mbstowcs_s(&len, nullptr, 0, str.c_str(), _TRUNCATE);
    if (len == 0)
        return L"";

    std::wstring wstr(len, L'\0');
    mbstowcs_s(&len, &wstr[0], len, str.c_str(), _TRUNCATE);
    wstr.resize(len - 1); // Remove the null terminator added by mbstowcs_s
    return wstr;
}

std::string WStringToString(const std::wstring& wstr)
{
    size_t len = 0;
    wcstombs_s(&len, nullptr, 0, wstr.c_str(), _TRUNCATE);
    if (len == 0)
        return "";
    std::string str(len, '\0');
    wcstombs_s(&len, &str[0], len, wstr.c_str(), _TRUNCATE);
    str.resize(len - 1); // Remove the null terminator added by wcstombs_s
    return str;
}

/////////////////////////////////////////////////////////////////////////


// EngineCore Ŭ������ ��� �Լ� ���Ǵ� ���� ���ӽ����̽��� �־�� �մϴ�.
// ���� ���� �ִ� �͸� ���ӽ����̽��� �����߽��ϴ�.

bool EngineCore::Initialize()
{
    // �����ڵ� ����� ������ Ŭ���� �̸��� ������ �̸� ����
    const wchar_t* className = L"JDEngine";
    const wchar_t* windowName = L"JDEngine";

    // �� Ŭ������ �θ� Ŭ����(__super)���� �����ϴ� Create �Լ� ȣ��
    // ���޵� Ŭ������ / ������� / ũ��� ������ ���� �õ�.
    if (false == __super::Create(className, windowName, 1204, 800)) {
        return false;
    }

    //if (false == InputManager::Instance().Initialize(m_hWnd))
    //{
    //    return false;
    //}

    m_EngineTimer = make_unique<GameTimer>(); // ���丮���� Ÿ�̸� unique ���·� �Ҵ�


    //m_Renderer = std::make_shared<D2DRenderer>(); // D2DRenderer ��ü�� shared_ptr�� ���� �� m_Renderer�� ����

    m_Renderer = make_shared<D2DRenderer>(); // ���丮���� ������ shared ���·� �Ҵ�
    m_Renderer->Initialize(m_hWnd);//Direct2D �ʱ�ȭ �۾�(����̽� ����) ����

    m_SceneManager = make_unique<SceneManager>(); // ���丮���� SceneManager unique ���·� �Ҵ�
    m_InputManager = make_unique<InputManager>(); // ���丮���� InputManager shared ���·� �Ҵ�




    m_SceneManager->RegisterScene(SceneType::SCENE_TEST, "TestScene01");
    /*
    // [ImGUI] ���ؽ�Ʈ & �鿣�� �ʱ�ȭ
    // ImGui ���ؽ�Ʈ ����
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGui_ImplWin32_Init(m_hWnd);
    */

    //D3D11Device* pd3dDevice = m_Renderer->GetD3DDevice(); // ���������� ������ ����̽� ����
    //m_Renderer->Initialize(m_hWnd);

    // �̾ ���������� ���ؽ�Ʈ �ޱ�
    ID3D11DeviceContext* pd3dDeviceContext = nullptr;
    pd3dDeviceContext = m_Renderer->GetD3DContext();

    // [ImGUI] DirectX 11 �鿣�� �ʱ�ȭ
    //ImGui_ImplDX11_Init(pd3dDevice, pd3dDeviceContext);

    // Ÿ�̸� �ʱ�ȭ
    m_EngineTimer->Reset();

    return true;
}

void EngineCore::Run()
{
    MSG msg = { 0 };

    while (WM_QUIT != msg.message) { // ���� �޼��� �������� ���� �ݺ�
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) // Ű �Է� �޼����� �޾��� ��
        {
            // PeekMessage�� �˻��� �޽���(msg)�� �м��Ͽ�
            // ���� Ű �ڵ� �޽���(��: WM_KEYDOWN, WM_KEYUP)�� ���� �޽���(��: WM_CHAR)�� ��ȯ
            if (false == m_InputManager->OnHandleMessage(msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else {
            // Ű �޼��� �Է��� ������ �⺻ ������Ʈ(�ݺ�) ���� ����
            UpdateTime();
            UpdateLogic();
            Render();
        }
    }

}

void EngineCore::Finalize()
{
    // [ImGUI] DirectX 11 �鿣�� ����
    //ImGui_ImplDX11_Shutdown();
    //ImGui_ImplWin32_Shutdown();
    //ImGui::DestroyContext();

    if (m_Renderer != nullptr) // ������ ����
    {
        m_Renderer->Uninitialize();
        m_Renderer.reset();
    }
}

bool EngineCore::OnWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    /*
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
    {
        return true; // ImGui�� �޽����� ó�������� true ��ȯ
    }
    */
    return false;
}

void EngineCore::UpdateTime()
{
    assert(m_EngineTimer != nullptr);
    assert(m_SceneManager != nullptr);

    m_EngineTimer->Tick();
    m_SceneManager->Update(m_EngineTimer->DeltaTime()); // 0.016f : fixedUpdate
    constexpr float FixedDeltaTime = 1.0f / 60.0f;
    m_SceneManager->FixedUpdate(FixedDeltaTime);
    m_SceneManager->LateUpdate(m_EngineTimer->DeltaTime());
}

void EngineCore::UpdateLogic()
{
    
}

void EngineCore::Render()
{
    if (m_Renderer == nullptr) return;

    m_Renderer->RenderBegin();

    m_Renderer->RenderEnd(false);

    m_Renderer->Present();
}


void EngineCore::OnResize(int width, int height) // â ũ�� ������
{
    __super::OnResize(width, height);

    if (m_Renderer != nullptr) m_Renderer->Resize(width, height);
}

void EngineCore::OnClose()
{
    std::cout << "OnClose" << std::endl;
}

