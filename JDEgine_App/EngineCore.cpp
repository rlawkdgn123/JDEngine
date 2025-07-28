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

    //m_SceneManager = make_unique<SceneManager>(); // ���丮���� SceneManager unique ���·� �Ҵ�

    JDGlobal::Window::GlobalWindowSize::Instance().Set(this);
    
    SceneManager::Instance().RegisterScene(make_unique< JDScene::TestScene>(JDGlobal::Core::SceneType::SCENE_TEST, "TestScene01"));
    SceneManager::Instance().ChangeScene("TestScene01");

    m_ResourceManager = make_shared<ResourceManager>();

    ID2D1RenderTarget* renderTarget = m_Renderer->GetRenderTarget();

    if (!m_ResourceManager->Initialize(renderTarget)) {
        return false;
    }
    //���� ��ġ Ȯ�ο�(����׿�)
    if (!std::experimental::filesystem::exists("../Resource/Test.png"))
        std::cout << "[ERROR] ������ �������� ����!" << std::endl;

    if (!m_ResourceManager->LoadTexture("Test", L"../Resource/Test.png")) {
        std::cout << "[ERROR] �ؽ�ó �ε� ����" << std::endl;
    }
    if (!m_ResourceManager->LoadTexture("cat", L"../Resource/cat.png")) {
        std::cout << "[ERROR] �ؽ�ó �ε� ����" << std::endl;
    }

    if (!m_ResourceManager->LoadTexture("graybirdsheet", L"../Resource/graybirdsheet.png")) {
        std::cout << "[ERROR] graybirdsheet �ؽ�ó �ε� ����" << std::endl;
    }
    if (!m_ResourceManager->LoadAnimation("GrayBird", L"../Resource/graybirdsheet.json")) {
        std::cout << "[ERROR] �ִϸ��̼� �ε� ����!" << std::endl;
    }
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
    
    JDGlobal::Window::GlobalWindowSize::Instance().Set(nullptr); // ���� �� ���� ����

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
    SceneManager::Instance().Render();
}

void EngineCore::UpdateLogic()
{
}

void EngineCore::Render()
{
    if (m_Renderer == nullptr) return;

    static float elapsed = 0.0f;
    static size_t frameIndex = 0;

    float deltaTime = m_EngineTimer->DeltaTime();

    m_Renderer->RenderBegin();

    ID2D1Bitmap* bitmap = m_ResourceManager->GetTexture("graybirdsheet"); // PNG �̸�
    const AnimationClip* clip = m_ResourceManager->GetAnimation("GrayBird"); // JSON �ִϸ��̼� �̸�

    if (bitmap && clip && !clip->frames.empty())
    {
        elapsed += deltaTime;

        if (elapsed >= clip->frames[frameIndex].duration)
        {
            elapsed = 0.0f;
            frameIndex++;
            if (frameIndex >= clip->frames.size())
            {
                frameIndex = clip->loop ? 0 : clip->frames.size() - 1;
            }
        }

        const D2D1_RECT_F& src = clip->frames[frameIndex].srcRect;
        D2D1_RECT_F dest = D2D1::RectF(100, 100,
            100 + (src.right - src.left),
            100 + (src.bottom - src.top));

        auto context = m_Renderer->GetD2DContext();

        // 1. ���� �����Ӹ� �߶� �ӽ� ��Ʈ�� ����
        ComPtr<ID2D1Bitmap1> cropped = m_Renderer->CreateCroppedBitmap(static_cast<ID2D1Bitmap1*>(bitmap), src);
        if (cropped) {
            auto effect = m_Renderer->CreateGaussianBlurEffect(cropped.Get(), 5.0f);
            if (effect) {
                m_Renderer->GetD2DContext()->DrawImage(effect.Get(), D2D1::Point2F(dest.left, dest.top));
            }
            else {
                m_Renderer->DrawBitmap(cropped.Get(), dest);
            }
        }
        else {
            m_Renderer->DrawBitmap(static_cast<ID2D1Bitmap1*>(bitmap), dest, src, 1.0f);
        }

        // 2. �� ����Ʈ ����
        auto effect = m_Renderer->CreateGaussianBlurEffect(cropped.Get(), 5.0f);

        // 3. ���
        if (effect)
            context->DrawImage(effect.Get(), D2D1::Point2F(dest.left, dest.top));
        else
            m_Renderer->DrawBitmap(cropped.Get(), dest);
    }

    ID2D1Bitmap* bitmap1 = m_ResourceManager->GetTexture("Test");
    if (bitmap1)
    {
        D2D1_RECT_F dest1 = D2D1::RectF(400, 100, 400 + bitmap1->GetSize().width, 100 + bitmap1->GetSize().height);
        m_Renderer->DrawBitmap(static_cast<ID2D1Bitmap1*>(bitmap1), dest1);
    }

    ID2D1Bitmap* bitmap2 = m_ResourceManager->GetTexture("cat");
    if (bitmap2)
    {
        auto context = m_Renderer->GetD2DContext();

        // 1. ���� ��Ʈ�ʿ��� �׷��̽����� ȿ�� ����
        ComPtr<ID2D1Effect> grayEffect;
        HRESULT hr = context->CreateEffect(CLSID_D2D1Grayscale, &grayEffect);
        if (FAILED(hr)) {
            std::cout << "�׷��̽����� ȿ�� ���� ����" << std::endl;
            return;
        }

        grayEffect->SetInput(0, bitmap2);

        // 2. ��� ��ġ ����
        D2D1_POINT_2F destPos = { 600.f, 200.f };

        // 3. �׷��̽����� ȿ�� ���
        context->DrawImage(grayEffect.Get(), destPos);
    }

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

