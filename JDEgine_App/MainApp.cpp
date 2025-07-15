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

// 추후 애니메이션 부분은 분리 예정


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

namespace Module {
    bool MainApp::Initalize()
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

        m_EngineTimer = BaseFactory::GameTimerFactory::CreateUnique(); // 팩토리에서 타이머 unique 형태로 할당

        //m_Renderer = std::make_shared<D2DRenderer>(); // D2DRenderer 객체를 shared_ptr로 생성 뒤 m_Renderer에 저장
        m_Renderer = BaseFactory::D2DRendererFactory::CreateShared(); // 팩토리에서 렌더러 shared 형태로 할당


        //방금 생성된 Renderer에 윈도우 핸들 전달 → Direct2D 초기화 작업(디바이스 생성) 수행
        m_Renderer->Initialize(m_hWnd);

        /*
        // [ImGUI] 컨텍스트 & 백엔드 초기화
        // ImGui 컨텍스트 생성
        IMGUI_CHECKVERSION();

        ImGui::CreateContext();

        ImGui_ImplWin32_Init(m_hWnd);
        */

        //D3D11Device* pd3dDevice = m_Renderer->GetD3DDevice(); // 렌더러에서 생성한 디바이스 연결
        m_Renderer->Initialize(m_hWnd);

        // 이어서 렌더러에게 컨텍스트 받기
        ID3D11DeviceContext* pd3dDeviceContext = nullptr;
        pd3dDeviceContext = m_Renderer->GetD3DContext();

        // [ImGUI] DirectX 11 백엔드 초기화
        //ImGui_ImplDX11_Init(pd3dDevice, pd3dDeviceContext);

        // 타이머 초기화
        m_EngineTimer->Reset();

        return true;
    }

    void MainApp::Run()
    {
        MSG msg = { 0 };

        while (WM_QUIT != msg.message) { // 종료 메세지 이전까지 무한 반복
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) // 키 입력 메세지를 받았을 때
            {
                // PeekMessage가 검색한 메시지(msg)를 분석하여 
                // 가상 키 코드 메시지(예: WM_KEYDOWN, WM_KEYUP)를 문자 메시지(예: WM_CHAR)로 변환
                TranslateMessage(&msg);

                DispatchMessage(&msg); // 메세지를 윈도우 프로시저에 전달
            }
            else {
                // 키 메세지 입력이 없으면 기본 업데이트(반복) 로직 실행
                UpdateTime();
                UpdateInput();
                UpdateLogic();
                Render();
            }
        }

    }

    void MainApp::Finalize()
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

    bool MainApp::OnWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        /*
        if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
        {
            return true; // ImGui가 메시지를 처리했으면 true 반환
        }
        */
        return false;
    }

    void MainApp::UpdateTime()
    {
        m_GameTimer.Tick();
    }

    void MainApp::UpdateInput() // 인풋 매니저에게 위임 예정
    {
    }

    void MainApp::UpdateLogic() // 추후 애니메이션 매니저 만들어서 위임 예정
    {
        LoadAssets();

        //if (m_selectedAssetKey.empty()) return; // 선택된 애셋 키가 비어있으면 리턴

        //const AnimationClips& clips = m_AssetManager.GetClips(m_selectedAssetKey);

        //if (clips.empty()) return; // 클립이 없으면 리턴

        //if (m_bChangedFile)
        //{
        //    // 파일이 변경되었으면 애니메이션 정보 초기화
        //    m_curSprites.clear();
        //    m_bChangedFile = false;

        //    // 애니메이션 플레이어 생성 및 클립 설정
        //    for (const auto& [name, clip] : clips)
        //    {
        //        SpriteAnimator ap;

        //        ap.SetClip(&clip);

        //        m_curSprites.push_back(ap);
        //    }
        //}

        //// 애니메이션 플레이어 업데이트
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
            return; // 렌더링 컨텍스트나 뷰가 없으면 리턴
        }
        m_Renderer->GetD3DContext()->OMSetRenderTargets(1, rtvs, nullptr);

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // 데모 UI 창 표시
        //static bool showDemo = true;
        //ImGui::ShowDemoWindow(&showDemo);

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                // 메뉴 아이템을 누르면 패널 열기 플래그 토글
                if (ImGui::MenuItem("Open Folder", "Ctrl+O"))
                {
                    m_showFolderPanel = true;
                }

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        // 모달리스 패널로 변경
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

            // 파일 목록 렌더링
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

            ImGui::End(); // Folder Browser 패널 끝
        }

        // ImGui 렌더링
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
        if (ext.empty()) return; // 확장자가 없으면 리턴

        std::filesystem::path keyPath = fullPath;
        keyPath.replace_extension(); // 확장자 제거하고 키로 쓸꺼에요

        std::wstring keyWide = keyPath.wstring();

        if (keyWide == m_selectedAssetKey)
        {
            // 이미 로드된 파일이면 다시 로드하지 않음
            return;
        }
        m_selectedAssetKey = keyWide; // 선택된 파일 키 저장

        if (ext == L".png")
        {
            m_AssetManager.LoadTexture(m_Renderer.get(), keyWide, fullPath);
        }
        else if (ext == L".json")
        {
            m_AssetManager.LoadAseprite(m_Renderer.get(), keyWide, fullPath);
        }

        m_bChangedFile = true; // 파일이 변경되었음을 표시
        */
    }

    void MainApp::OnResize(int width, int height) // 창 크기 재조정
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

        // COM 대화상자 생성
        hr = CoCreateInstance(
            CLSID_FileOpenDialog, nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&pDialog)
        );

        if (FAILED(hr) || !pDialog) return;

        // 폴더 선택 모드로 설정
        DWORD opts = 0;
        if (SUCCEEDED(pDialog->GetOptions(&opts)))
            pDialog->SetOptions(opts | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);

        // 다이얼로그 표시
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
                    m_folderPath = pszFolder;        // 선택된 폴더 경로 저장
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
                // 파일만 추가, 디렉토리는 제외
                if (entry.path().extension() == L".png" ||
                    entry.path().extension() == L".json")
                    // 이미지 파일만 추가
                    m_fileList.push_back(entry.path().filename().wstring());
            }
        }
        */
    }

}
