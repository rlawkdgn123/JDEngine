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

/////////////////////////////////////////////////////////////////////////
#include "MainApp.h"
/////////////////////////////////////////////////////////////////////////

// ���� �ִϸ��̼� �κ��� �и� ����


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

namespace Module {
    bool MainApp::Initalize()
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

        m_EngineTimer = BaseFactory::GameTimerFactory::CreateUnique(); // ���丮���� Ÿ�̸� unique ���·� �Ҵ�

        //m_Renderer = std::make_shared<D2DRenderer>(); // D2DRenderer ��ü�� shared_ptr�� ���� �� m_Renderer�� ����
        m_Renderer = BaseFactory::D2DRendererFactory::CreateShared(); // ���丮���� ������ shared ���·� �Ҵ�


        //��� ������ Renderer�� ������ �ڵ� ���� �� Direct2D �ʱ�ȭ �۾�(����̽� ����) ����
        m_Renderer->Initialize(m_hWnd);

        /*
        // [ImGUI] ���ؽ�Ʈ & �鿣�� �ʱ�ȭ
        // ImGui ���ؽ�Ʈ ����
        IMGUI_CHECKVERSION();

        ImGui::CreateContext();

        ImGui_ImplWin32_Init(m_hWnd);
        */

        //D3D11Device* pd3dDevice = m_Renderer->GetD3DDevice(); // ���������� ������ ����̽� ����
        m_Renderer->Initialize(m_hWnd);

        // �̾ ���������� ���ؽ�Ʈ �ޱ�
        ID3D11DeviceContext* pd3dDeviceContext = nullptr;
        pd3dDeviceContext = m_Renderer->GetD3DContext();

        // [ImGUI] DirectX 11 �鿣�� �ʱ�ȭ
        //ImGui_ImplDX11_Init(pd3dDevice, pd3dDeviceContext);

        // Ÿ�̸� �ʱ�ȭ
        m_EngineTimer->Reset();

        return true;
    }

    void MainApp::Run()
    {
        MSG msg = { 0 };

        while (WM_QUIT != msg.message) { // ���� �޼��� �������� ���� �ݺ�
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) // Ű �Է� �޼����� �޾��� ��
            {
                // PeekMessage�� �˻��� �޽���(msg)�� �м��Ͽ� 
                // ���� Ű �ڵ� �޽���(��: WM_KEYDOWN, WM_KEYUP)�� ���� �޽���(��: WM_CHAR)�� ��ȯ
                TranslateMessage(&msg);

                DispatchMessage(&msg); // �޼����� ������ ���ν����� ����
            }
            else {
                // Ű �޼��� �Է��� ������ �⺻ ������Ʈ(�ݺ�) ���� ����
                UpdateTime();
                UpdateInput();
                UpdateLogic();
                Render();
            }
        }

    }

    void MainApp::Finalize()
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

    bool MainApp::OnWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        /*
        if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
        {
            return true; // ImGui�� �޽����� ó�������� true ��ȯ
        }
        */
        return false;
    }

    void MainApp::UpdateTime()
    {
        m_GameTimer.Tick();
    }

    void MainApp::UpdateInput() // ��ǲ �Ŵ������� ���� ����
    {
    }

    void MainApp::UpdateLogic() // ���� �ִϸ��̼� �Ŵ��� ���� ���� ����
    {
        LoadAssets();

        //if (m_selectedAssetKey.empty()) return; // ���õ� �ּ� Ű�� ��������� ����

        //const AnimationClips& clips = m_AssetManager.GetClips(m_selectedAssetKey);

        //if (clips.empty()) return; // Ŭ���� ������ ����

        //if (m_bChangedFile)
        //{
        //    // ������ ����Ǿ����� �ִϸ��̼� ���� �ʱ�ȭ
        //    m_curSprites.clear();
        //    m_bChangedFile = false;

        //    // �ִϸ��̼� �÷��̾� ���� �� Ŭ�� ����
        //    for (const auto& [name, clip] : clips)
        //    {
        //        SpriteAnimator ap;

        //        ap.SetClip(&clip);

        //        m_curSprites.push_back(ap);
        //    }
        //}

        //// �ִϸ��̼� �÷��̾� ������Ʈ
        //for (auto& sprite : m_curSprites)
        //{
        //    sprite.Update(m_GameTimer.DeltaTime());
        //}
    }

    void MainApp::Render()
    {
        if (m_Renderer == nullptr) return;

        m_Renderer->RenderBegin();
    }

    void MainApp::RenderImGUI()
    {
        /*
         ID3D11DeviceContext* pd3dDeviceContext = nullptr;
        pd3dDeviceContext = m_Renderer->GetD3DContext();
        ID3D11RenderTargetView* rtvs[] = { m_Renderer->GetD3DRenderTargetView() };

        if (pd3dDeviceContext == nullptr || rtvs[0] == nullptr)
        {
            return; // ������ ���ؽ�Ʈ�� �䰡 ������ ����
        }
        m_Renderer->GetD3DContext()->OMSetRenderTargets(1, rtvs, nullptr);

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // ���� UI â ǥ��
        //static bool showDemo = true;
        //ImGui::ShowDemoWindow(&showDemo);

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                // �޴� �������� ������ �г� ���� �÷��� ���
                if (ImGui::MenuItem("Open Folder", "Ctrl+O"))
                {
                    m_showFolderPanel = true;
                }

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        // ��޸��� �гη� ����
        if (m_showFolderPanel)
        {
            ImGui::Begin("Folder Browser", &m_showFolderPanel, ImGuiWindowFlags_AlwaysAutoResize);

            if (ImGui::Button("Browse..."))
            {
                BrowseForFolder();
                m_pathInput = std::filesystem::path(m_folderPath).u8string();
            }

            ImGui::SameLine();
            if (ImGui::Button("Load"))
            {
                if (false == m_folderPath.empty())
                {
                    UpdateFileList();
                    m_selectedFile.clear();
                }
            }

            ImGui::Text("Folder: %ls", m_folderPath.c_str());

            // ���� ��� ������
            if (false == m_folderPath.empty())
            {
                if (ImGui::BeginListBox("Files", ImVec2(300.0f, 8 * ImGui::GetTextLineHeightWithSpacing())))
                {
                    for (const auto& name : m_fileList)
                    {
                        bool isSelected = (m_selectedFile == name);
                        if (ImGui::Selectable(WStringToString(name).c_str(), isSelected, ImGuiSelectableFlags_AllowDoubleClick))
                        {
                            m_selectedFile = name;
                        }

                        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                        {
                            m_selectedFile = name;
                        }
                    }

                    ImGui::EndListBox();
                }

                if (!m_selectedFile.empty())
                {
                    ImGui::Text("Selected File: %ls", m_selectedFile.c_str());
                }
            }

            ImGui::End(); // Folder Browser �г� ��
        }

        // ImGui ������
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        */
    }

    void MainApp::LoadAssets()
    {
        /*
         std::filesystem::path fullPath =
            m_folderPath / std::filesystem::path(m_selectedFile);

        auto ext = fullPath.extension();
        if (ext.empty()) return; // Ȯ���ڰ� ������ ����

        std::filesystem::path keyPath = fullPath;
        keyPath.replace_extension(); // Ȯ���� �����ϰ� Ű�� ��������

        std::wstring keyWide = keyPath.wstring();

        if (keyWide == m_selectedAssetKey)
        {
            // �̹� �ε�� �����̸� �ٽ� �ε����� ����
            return;
        }
        m_selectedAssetKey = keyWide; // ���õ� ���� Ű ����

        if (ext == L".png")
        {
            m_AssetManager.LoadTexture(m_Renderer.get(), keyWide, fullPath);
        }
        else if (ext == L".json")
        {
            m_AssetManager.LoadAseprite(m_Renderer.get(), keyWide, fullPath);
        }

        m_bChangedFile = true; // ������ ����Ǿ����� ǥ��
        */
    }

    void MainApp::OnResize(int width, int height) // â ũ�� ������
    {
        __super::OnResize(width, height);

        if (m_Renderer != nullptr) m_Renderer->Resize(width, height);
    }

    void MainApp::OnClose()
    {
        std::cout << "OnClose" << std::endl;
    }

    void MainApp::BrowseForFolder()
    {
        /*
        HRESULT hr;
        IFileOpenDialog* pDialog = nullptr;

        // COM ��ȭ���� ����
        hr = CoCreateInstance(
            CLSID_FileOpenDialog, nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&pDialog)
        );

        if (FAILED(hr) || !pDialog) return;

        // ���� ���� ���� ����
        DWORD opts = 0;
        if (SUCCEEDED(pDialog->GetOptions(&opts)))
            pDialog->SetOptions(opts | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);

        // ���̾�α� ǥ��
        hr = pDialog->Show(m_hWnd);

        if (SUCCEEDED(hr))
        {
            IShellItem* pItem = nullptr;
            hr = pDialog->GetResult(&pItem);

            if (SUCCEEDED(hr) && pItem)
            {
                PWSTR pszFolder = nullptr;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFolder);
                if (SUCCEEDED(hr) && pszFolder)
                {
                    m_folderPath = pszFolder;        // ���õ� ���� ��� ����
                    CoTaskMemFree(pszFolder);
                }
                pItem->Release();
            }
        }

        pDialog->Release();
        */
    }

    void MainApp::UpdateFileList()
    {
        /*
         m_fileList.clear();
        for (const auto& entry : std::filesystem::directory_iterator(m_folderPath))
        {
            if (entry.is_regular_file())
            {
                // ���ϸ� �߰�, ���丮�� ����
                if (entry.path().extension() == L".png" ||
                    entry.path().extension() == L".json")
                    // �̹��� ���ϸ� �߰�
                    m_fileList.push_back(entry.path().filename().wstring());
            }
        }
        */
    }

}
