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
// 추후 애니메이션 부분은 분리 예정
using namespace std;

using GameTimer = JDEngine::GameTimer;
using D2DRenderer = JDEngine::D2DRenderer;
using InputManager = JDEngine::InputManager;
using SceneManager = JDEngine::SceneManager;
// (일반적으로 UTF-8 또는 로케일별 멀티바이트 인코딩)과
// std::wstring (일반적으로 Windows에서 UTF-16) 간의 변환을 위한 두 가지 유틸리티 함수

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


// EngineCore 클래스의 멤버 함수 정의는 전역 네임스페이스에 있어야 합니다.
// 파일 끝에 있던 익명 네임스페이스를 제거했습니다.

bool EngineCore::Initialize()
{
    // 유니코드 기반의 윈도우 클래스 이름과 윈도우 이름 설정
    const wchar_t* className = L"JDEngine";
    const wchar_t* windowName = L"JDEngine";

    // 이 클래스의 부모 클래스(__super)에서 제공하는 Create 함수 호출
    // 전달된 클래스명 / 윈도우명 / 크기로 윈도우 생성 시도.
    if (false == __super::Create(className, windowName, 1204, 800)) {
        return false;
    }

    //if (false == InputManager::Instance().Initialize(m_hWnd))
    //{
    //    return false;
    //}

    m_EngineTimer = make_unique<GameTimer>(); // 팩토리에서 타이머 unique 형태로 할당


    //m_Renderer = std::make_shared<D2DRenderer>(); // D2DRenderer 객체를 shared_ptr로 생성 뒤 m_Renderer에 저장

    m_Renderer = make_shared<D2DRenderer>(); // 팩토리에서 렌더러 shared 형태로 할당
    m_Renderer->Initialize(m_hWnd);//Direct2D 초기화 작업(디바이스 생성) 수행

    m_SceneManager = make_unique<SceneManager>(); // 팩토리에서 SceneManager unique 형태로 할당
    m_InputManager = make_unique<InputManager>(); // 팩토리에서 InputManager shared 형태로 할당




    m_SceneManager->RegisterScene(SceneType::SCENE_TEST, "TestScene01");
    /*
    // [ImGUI] 컨텍스트 & 백엔드 초기화
    // ImGui 컨텍스트 생성
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGui_ImplWin32_Init(m_hWnd);
    */

    //D3D11Device* pd3dDevice = m_Renderer->GetD3DDevice(); // 렌더러에서 생성한 디바이스 연결
    //m_Renderer->Initialize(m_hWnd);

    // 이어서 렌더러에게 컨텍스트 받기
    ID3D11DeviceContext* pd3dDeviceContext = nullptr;
    pd3dDeviceContext = m_Renderer->GetD3DContext();

    // [ImGUI] DirectX 11 백엔드 초기화
    //ImGui_ImplDX11_Init(pd3dDevice, pd3dDeviceContext);

    // 타이머 초기화
    m_EngineTimer->Reset();

    return true;
}

void EngineCore::Run()
{
    MSG msg = { 0 };

    while (WM_QUIT != msg.message) { // 종료 메세지 이전까지 무한 반복
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) // 키 입력 메세지를 받았을 때
        {
            // PeekMessage가 검색한 메시지(msg)를 분석하여
            // 가상 키 코드 메시지(예: WM_KEYDOWN, WM_KEYUP)를 문자 메시지(예: WM_CHAR)로 변환
            if (false == m_InputManager->OnHandleMessage(msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else {
            // 키 메세지 입력이 없으면 기본 업데이트(반복) 로직 실행
            UpdateTime();
            UpdateLogic();
            Render();
        }
    }

}

void EngineCore::Finalize()
{
    // [ImGUI] DirectX 11 백엔드 정리
    //ImGui_ImplDX11_Shutdown();
    //ImGui_ImplWin32_Shutdown();
    //ImGui::DestroyContext();

    if (m_Renderer != nullptr) // 렌더러 종료
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
        return true; // ImGui가 메시지를 처리했으면 true 반환
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


void EngineCore::OnResize(int width, int height) // 창 크기 재조정
{
    __super::OnResize(width, height);

    if (m_Renderer != nullptr) m_Renderer->Resize(width, height);
}

void EngineCore::OnClose()
{
    std::cout << "OnClose" << std::endl;
}

