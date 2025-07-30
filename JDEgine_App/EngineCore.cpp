#include "pch.h"
#include "Framework.h"
///////////////////////////////////////////////////////////////////////////
#pragma comment(lib, "dxguid.lib")
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
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>//C++14
#include <assert.h>
/////////////////////////////////////////////////////////////////////////
#include "EngineCore.h"
/////////////////////////////////////////////////////////////////////////
// 추후 애니메이션 부분은 분리 예정
using namespace std;

// (일반적으로 UTF-8 또는 로케일별 멀티바이트 인코딩)과
// std::wstring (일반적으로 Windows에서 UTF-16) 간의 변환을 위한 두 가지 유틸리티 함수
using WindowSize = JDGlobal::Window::WindowSize;
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
    if (false == __super::Create(className, windowName, 1200, 800)) {
        return false;
    }

    InputManager::Instance().Initialize(m_hWnd);
    //if (false == InputManager::Instance().Initialize(m_hWnd))
    //{
    //    return false;
    //}

    m_EngineTimer = make_unique<GameTimer>(); // 팩토리에서 타이머 unique 형태로 할당

    D2DRenderer::Instance().Initialize(m_hWnd);
    D2DRenderer::Instance().SetCamera(cam);
    //m_Renderer = std::make_shared<D2DRenderer>(); // D2DRenderer 객체를 shared_ptr로 생성 뒤 m_Renderer에 저장



    //m_SceneManager = make_unique<SceneManager>(); // 팩토리에서 SceneManager unique 형태로 할당

    WindowSize::Instance().Set(this);

    SceneManager::Instance().RegisterScene(make_unique< JDScene::TestScene>(JDGlobal::Core::SceneType::SCENE_TEST, "TestScene01"));
    SceneManager::Instance().ChangeScene("TestScene01");



    ID2D1RenderTarget* renderTarget = D2DRenderer::Instance().GetRenderTarget();

    if (!ResourceManager::Instance().Initialize(renderTarget)) {
        return false;
    }

    //파일 위치 확인용(디버그용)
    /*if (!std::experimental::filesystem::exists("../Resource/Test.png"))
        std::cout << "[ERROR] 파일이 존재하지 않음!" << std::endl;*/

    if (!ResourceManager::Instance().LoadTexture("Test", L"../Resource/Test.png")) {
        std::cout << "[ERROR] 텍스처 로드 실패" << std::endl;
    }

    /*if (!ResourceManager::Instance().LoadTexture("cat", L"../Resource/cat.png")) {
        std::cout << "[ERROR] 텍스처 로드 실패" << std::endl;
    }
    if (!ResourceManager::Instance().LoadTexture("Grid", L"../Resource/Grid.png")) {
        std::cout << "[ERROR] 텍스처 로드 실패" << std::endl;
    }
    */

    if (!ResourceManager::Instance().LoadTexture("GrayBird", L"../Resource/graybirdsheet.png")) {
        std::cout << "[ERROR] GrayBird 텍스처 로드 실패" << std::endl;
    }
    if (!ResourceManager::Instance().LoadAnimation("GrayBird", L"../Resource/graybirdsheet.json")) {
        std::cout << "[ERROR] 애니메이션 로드 실패!" << std::endl;
    }

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
    pd3dDeviceContext = D2DRenderer::Instance().GetD3DContext();

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
            if (false == InputManager::Instance().OnHandleMessage(msg))
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

    JDGlobal::Window::WindowSize::Instance().Set(nullptr); // 종료 시 해제 권장

    D2DRenderer::Instance().Uninitialize();
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

    m_EngineTimer->Tick();
    float deltaTime = m_EngineTimer->DeltaTime();
    float fixedDeltaTime = 1.0f / 60.0f;

    SceneManager::Instance().Update(deltaTime); // 0.016f : fixedUpdate
    // FixedUpdate 누적 처리
    m_FixedTimeAccumulator += deltaTime;
    while (m_FixedTimeAccumulator >= fixedDeltaTime)
    {
        SceneManager::Instance().FixedUpdate(fixedDeltaTime);
        m_FixedTimeAccumulator -= fixedDeltaTime;
    }
    SceneManager::Instance().LateUpdate(deltaTime);
    //SceneManager::Instance().Render();
}

void EngineCore::UpdateLogic()
{
    // 배속 키 입력처리
    static float speeds[] = { 2.f, 4.f, 8.f };
    static int   idx = 0;
    InputManager& input = InputManager::Instance();
    if (input.GetKeyPressed('1')) {
        SceneManager::Instance().SetSceneTimeScale(0.0f);
        std::cout << "0" << std::endl;
        idx = 0;
    }
    else if (input.GetKeyPressed('2')) {
        SceneManager::Instance().SetSceneTimeScale(1.0f);
        std::cout << "1" << std::endl;
        idx = 0;
    }
    else if (input.GetKeyPressed('3')) {
        SceneManager::Instance().SetSceneTimeScale(speeds[idx]);
        std::cout << speeds[idx] << std::endl;
        idx = (idx + 1) % (sizeof(speeds) / sizeof(*speeds));
    }
    //
    
    cam = D2DRenderer::Instance().GetCamera();
    /*m_cameraPosition = cam->GetPosition();
    m_cameraRotationDeg = cam->GetRotationDeg();
    m_cameraZoom = cam->GetZoom();*/
    if (cam)
    {

        D2D1_MATRIX_3X2_F view = cam->GetViewMatrix();
        D2DRenderer::Instance().SetTransform(view);

        const float moveSpeed = 300.0f;    // px/sec
        const float rotateSpeed = 90.0f;   // deg/sec
        const float zoomFactor = 0.8f;     // 줌 비율
        float dt = m_EngineTimer->DeltaTime();

        InputManager& input = InputManager::Instance();
        //ScreanWidth, ScreanHeight))
        // 이동
        if (input.IsKeyDown('W')) {
            cam->Move(0.f, -moveSpeed * dt);
            //cam->SetScreenWidth(-moveSpeed * dt);
        }

        if (input.IsKeyDown('S'))
            cam->Move(0.f, moveSpeed * dt);

        if (input.IsKeyDown('A'))
            cam->Move(-moveSpeed * dt, 0.f);

        if (input.IsKeyDown('D'))
            cam->Move(moveSpeed * dt, 0.f);

        // 회전
        if (input.IsKeyDown(VK_LEFT))
            cam->Rotate(-rotateSpeed * dt);

        if (input.IsKeyDown(VK_RIGHT))
            cam->Rotate(rotateSpeed * dt);

        // 줌
        if (input.IsKeyDown(VK_UP)) {
            D2D1_POINT_2F screenCenter = {
                cam->GetScreenWidth() * 0.5f,
                cam->GetScreenHeight() * 0.5f
            };
            cam->Zoom(1.f + dt, screenCenter);
        }

        if (input.IsKeyDown(VK_DOWN)) {
            D2D1_POINT_2F screenCenter = {
                cam->GetScreenWidth() * 0.5f,
                cam->GetScreenHeight() * 0.5f
            };
            cam->Zoom(1.f - dt, screenCenter);
        }


        if (input.IsKeyDown(VK_SPACE))
            cam->Shake(10.0f, 0.1f);
    }
}

void EngineCore::Render()
{
    static float elapsed = 0.0f;
    static size_t frameIndex = 0;

    float deltaTime = m_EngineTimer->DeltaTime();

    D2DRenderer& renderer = D2DRenderer::Instance();
    renderer.RenderBegin();
    // [1] 카메라 적용
    ID2D1DeviceContext* context = renderer.GetD2DContext();

    D2D1_POINT_2F  camPos = m_cameraPosition;       // 예: {100.f, 50.f}
    float camRot = m_cameraRotationDeg;      // 예: 30.f
    float camZoom = m_cameraZoom;            // 예: 1.5f

    D2D1_SIZE_F screenSize = context->GetSize();
    D2D1_POINT_2F screenCenter = D2D1::Point2F(screenSize.width * 0.5f, screenSize.height * 0.5f);

    D2D1_MATRIX_3X2_F cameraMatrix =
        D2D1::Matrix3x2F::Translation(-camPos.x, -camPos.y) *
        D2D1::Matrix3x2F::Rotation(-camRot, screenCenter) *
        D2D1::Matrix3x2F::Scale(camZoom, camZoom, { 0, 0 }) *
        D2D1::Matrix3x2F::Translation(screenCenter.x, screenCenter.y);

    D2D1_MATRIX_3X2_F unityFlip = D2D1::Matrix3x2F::Scale(1.f, -1.f, { 0, 0 });
    cameraMatrix = cameraMatrix * unityFlip;

    renderer.SetTransform(cameraMatrix);
    //std::cout << deltaTime << std::endl;
    SceneManager::Instance().Render(deltaTime);

    renderer.RenderEnd();

}


void EngineCore::OnResize(int width, int height) // 창 크기 재조정
{
    __super::OnResize(width, height);

    D2DRenderer::Instance().Resize(width, height);

    cam->SetScreenSize(static_cast<float>(width), static_cast<float>(height));
}

void EngineCore::OnClose()
{
    std::cout << "OnClose" << std::endl;
}

