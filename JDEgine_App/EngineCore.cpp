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
// ���� �ִϸ��̼� �κ��� �и� ����
using namespace std;

// (�Ϲ������� UTF-8 �Ǵ� �����Ϻ� ��Ƽ����Ʈ ���ڵ�)��
// std::wstring (�Ϲ������� Windows���� UTF-16) ���� ��ȯ�� ���� �� ���� ��ƿ��Ƽ �Լ�
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


// EngineCore Ŭ������ ��� �Լ� ���Ǵ� ���� ���ӽ����̽��� �־�� �մϴ�.
// ���� ���� �ִ� �͸� ���ӽ����̽��� �����߽��ϴ�.

bool EngineCore::Initialize()
{
    // �����ڵ� ����� ������ Ŭ���� �̸��� ������ �̸� ����
    const wchar_t* className = L"JDEngine";
    const wchar_t* windowName = L"JDEngine";

    // �� Ŭ������ �θ� Ŭ����(__super)���� �����ϴ� Create �Լ� ȣ��
    // ���޵� Ŭ������ / ������� / ũ��� ������ ���� �õ�.
    if (false == __super::Create(className, windowName, 1200, 800)) {
        return false;
    }

    InputManager::Instance().Initialize(m_hWnd);
    //if (false == InputManager::Instance().Initialize(m_hWnd))
    //{
    //    return false;
    //}
    //
    m_EngineTimer = make_unique<GameTimer>(); // ���丮���� Ÿ�̸� unique ���·� �Ҵ�

    D2DRenderer::Instance().Initialize(m_hWnd);
    D2DRenderer::Instance().SetCamera(cam);
    //m_Renderer = std::make_shared<D2DRenderer>(); // D2DRenderer ��ü�� shared_ptr�� ���� �� m_Renderer�� ����

    

    //m_SceneManager = make_unique<SceneManager>(); // ���丮���� SceneManager unique ���·� �Ҵ�

    WindowSize::Instance().Set(this);

    SceneManager::Instance().RegisterScene(make_unique< JDScene::TestScene>(JDGlobal::Core::SceneType::SCENE_TEST, "TestScene01"));
    SceneManager::Instance().ChangeScene("TestScene01");

    

    ID2D1RenderTarget* renderTarget = D2DRenderer::Instance().GetRenderTarget();
    
    if (!ResourceManager::Instance().Initialize(renderTarget)) {
        return false;
    }

    //���� ��ġ Ȯ�ο�(����׿�)
    /*if (!std::experimental::filesystem::exists("../Resource/Test.png"))
        std::cout << "[ERROR] ������ �������� ����!" << std::endl;*/

    if (!ResourceManager::Instance().LoadTexture("Test", L"../Resource/Test.png")) {
        std::cout << "[ERROR] �ؽ�ó �ε� ����" << std::endl;
    }

    /*if (!ResourceManager::Instance().LoadTexture("cat", L"../Resource/cat.png")) {
        std::cout << "[ERROR] �ؽ�ó �ε� ����" << std::endl;
    }
    if (!ResourceManager::Instance().LoadTexture("Grid", L"../Resource/Grid.png")) {
        std::cout << "[ERROR] �ؽ�ó �ε� ����" << std::endl;
    }

    if (!ResourceManager::Instance().LoadTexture("graybirdsheet", L"../Resource/graybirdsheet.png")) {
        std::cout << "[ERROR] graybirdsheet �ؽ�ó �ε� ����" << std::endl;
    }
    if (!ResourceManager::Instance().LoadAnimation("GrayBird", L"../Resource/graybirdsheet.json")) {
        std::cout << "[ERROR] �ִϸ��̼� �ε� ����!" << std::endl;
    }*/

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
    pd3dDeviceContext = D2DRenderer::Instance().GetD3DContext();

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
            if (false == InputManager::Instance().OnHandleMessage(msg))
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
    
    JDGlobal::Window::WindowSize::Instance().Set(nullptr); // ���� �� ���� ����

    D2DRenderer::Instance().Uninitialize();
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

    m_EngineTimer->Tick();
    float deltaTime = m_EngineTimer->DeltaTime();
    float fixedDeltaTime = 1.0f/60.0f;

    SceneManager::Instance().Update(deltaTime); // 0.016f : fixedUpdate
    // FixedUpdate ���� ó��
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
        const float zoomFactor = 0.8f;     // �� ����
        float dt = m_EngineTimer->DeltaTime();

        InputManager& input = InputManager::Instance();
        //ScreanWidth, ScreanHeight))
        // �̵�
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

        // ȸ��
        if (input.IsKeyDown(VK_LEFT))
            cam->Rotate(-rotateSpeed * dt);

        if (input.IsKeyDown(VK_RIGHT))
            cam->Rotate(rotateSpeed * dt);

        // ��
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
    // [1] ī�޶� ����
    ID2D1DeviceContext* context = renderer.GetD2DContext();

    D2D1_POINT_2F  camPos = m_cameraPosition;       // ��: {100.f, 50.f}
    float camRot = m_cameraRotationDeg;      // ��: 30.f
    float camZoom = m_cameraZoom;            // ��: 1.5f

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

    SceneManager::Instance().Render();

    renderer.RenderEnd();

}


void EngineCore::OnResize(int width, int height) // â ũ�� ������
{
    __super::OnResize(width, height);

    D2DRenderer::Instance().Resize(width, height);

    cam->SetScreenSize(static_cast<float>(width), static_cast<float>(height));
}

void EngineCore::OnClose()
{
    std::cout << "OnClose" << std::endl;
}

