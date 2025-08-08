#include "pch.h"
#include "Framework.h"
///////////////////////////////////////////////////////////////////////////
#pragma comment(lib, "dxguid.lib")
////////////////////////////////////////////////////////////////////////////
// IMGUI
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
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
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
        return conv.to_bytes(wstr);
    }
    catch (...) {
        return "[[ConversionError]]";
    }
}

std::string WStringToUTF8(const std::wstring& wstr)
{
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
        return conv.to_bytes(wstr);
    }
    catch (...) {
        return "[[ConversionError]]";
    }
}

std::wstring UTF8ToWString(const std::string& utf8Str)
{
    if (utf8Str.empty()) return std::wstring();

    int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);
    if (sizeNeeded <= 0) return std::wstring();

    std::wstring wstr(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wstr[0], sizeNeeded);

    // null-terminator 제거
    if (!wstr.empty() && wstr.back() == L'\0')
        wstr.pop_back();

    return wstr;
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
    if (false == __super::Create(className, windowName, 1920, 1080)) {
        return false;
    }

    JDGlobal::Window::WindowSize::Instance().Set(this);

    AudioManager::Instance().Initialize();

    //FMODSystem::Instance().PlayOneShot("assets/sfx/explosion.wav");
    //AudioManager::Instance().LoadAudio("MainTheme","../Resource/Audio/TestSound.mp3", true);
    //AudioManager::Instance().LoadAudio("MainTheme", "../Resource/Audio/KJH.mp3", true);
    AudioManager::Instance().LoadAudio("MainTheme", "../Resource/Audio/Golden.mp3", true);
    AudioManager::Instance().LoadAudio("Step", "../Resource/Audio/Step.mp3", false);
    AudioManager::Instance().SetMusicVolume(1.0f);
    FMOD::Channel* bgmChannel = nullptr;
    AudioManager::Instance().PlayBGM("MainTheme", &bgmChannel);

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

    ID2D1RenderTarget* renderTarget = D2DRenderer::Instance().GetRenderTarget();

    if (!AssetManager::Instance().Initialize(renderTarget)) {
        return false;
    }

    DataTableManager::Instance().Initalize();
    DataTableManager::Instance().PrintAllTable();
    //파일 위치 확인용(디버그용)
    /*if (!std::experimental::filesystem::exists("../Resource/Test.png"))
        std::cout << "[ERROR] 파일이 존재하지 않음!" << std::endl;*/

    LoadResources();

    AssetManager::Instance().TextureSetUp();

    SceneManager::Instance().RegisterScene(make_unique< JDScene::TitleScene>(JDGlobal::Core::SceneType::SCENE_TITLE, "TitleScene"));
    SceneManager::Instance().RegisterScene(make_unique< JDScene::TutorialScene>(JDGlobal::Core::SceneType::SCENE_TUTORIAL, "TutorialScene"));
    SceneManager::Instance().RegisterScene(make_unique< JDScene::SelectNationScene>(JDGlobal::Core::SceneType::SCENE_SELECTNATION, "SelectNationScene"));
    SceneManager::Instance().RegisterScene(make_unique< JDScene::GameScene>(JDGlobal::Core::SceneType::SCENE_TEST, "GameScene"));

    // SceneManager::Instance().RegisterScene(make_unique< JDScene::TestScene>(JDGlobal::Core::SceneType::SCENE_TEST, "TestScene01"));

    SceneManager::Instance().ChangeScene("TitleScene");

    //SceneManager::Instance().ChangeScene("TestScene01");
    //SceneManager::Instance().ChangeScene("GameScene");
    //SceneManager::Instance().ChangeScene("TutorialScene");
    //SceneManager::Instance().ChangeScene("SelectNationScene");


    //SceneManager::Instance().RegisterScene(make_unique< JDScene::TestScene>(JDGlobal::Core::SceneType::SCENE_TEST, "TestScene01"));
    //SceneManager::Instance().ChangeScene("TestScene01");

    // 이어서 렌더러에게 컨텍스트 받기
    ID3D11DeviceContext* pd3dDeviceContext = nullptr;
    pd3dDeviceContext = D2DRenderer::Instance().GetD3DContext();

    // [ImGUI] DirectX 11 백엔드 초기화
    //ImGui_ImplDX11_Init(pd3dDevice, pd3dDeviceContext);

    // 타이머 초기화
    m_EngineTimer->Reset();

    ////////////////////////////////////////////////////////////////////////////////
    // 
    // ImGui Init
    //
    ///////////////////////////////////////////////////////////////////////////////

    // ImGui 컨텍스트 생성
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // 플랫폼/렌더러 초기화
    ImGui_ImplWin32_Init(m_hWnd);              // 윈도우 핸들 (GameApp 멤버에 저장된 윈도우 핸들)
    ImGui_ImplDX11_Init(D2DRenderer::Instance().GetD3DDevice(), D2DRenderer::Instance().GetD3DContext());



    return true;
}

void EngineCore::Run()
{
    MSG msg = { 0 };

    while (WM_QUIT != msg.message)
    {
        // 모든 Windows 메시지를 처리합니다.
        // 메시지가 있다면 TranslateMessage/DispatchMessage를 호출하고 루프의 처음으로 돌아가
        // 큐에 있는 다음 메시지를 처리합니다.
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // ★★ 키 입력이 안먹어서 직접 InputManager::OnHandleMessage에서 ImGui 입력을 우선시 하고 있음.
            if (ImGui_ImplWin32_WndProcHandler(msg.hwnd, msg.message, msg.wParam, msg.lParam))
            {
                continue; // ImGui가 처리했으면 여기서 끝냅니다.
            }

            // InputManager가 메시지를 처리하는지 확인합니다.
            if (InputManager::Instance().OnHandleMessage(msg))
            {
                continue; // InputManager가 처리했으면 여기서 끝냅니다.
            }

            // 위에서 아무도 처리하지 않은 메시지는 Windows의 기본 방식으로 처리합니다.
            // WM_CHAR와 같은 메시지 생성을 위해 필수적입니다.
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            // 메시지를 처리했으므로 다음 루프로 넘어갑니다.
            continue;
        }

        // 썬 전환 요청이 있으면 확인하고 처리함.
        SceneManager::Instance().ProcessSceneChange();

        // 메시지 큐가 비어있을 때만 게임 로직을 실행합니다.
        UpdateTime();
        UpdateLogic();
        AudioManager::Instance().Update();
        Render();
    }
}

void EngineCore::Finalize()
{
    // [ImGUI] DirectX 11 백엔드 정리
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    JDGlobal::Window::WindowSize::Instance().Set(nullptr); // 종료 시 해제 권장

    D2DRenderer::Instance().Uninitialize();
}

void EngineCore::UpdateTime()
{
    // ImGui
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ////////////////////////////////////////////////////////////////////////////////

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
    InputManager::Instance().SetMouseState()->leftClicked = false;
    InputManager::Instance().SetMouseState()->rightClicked = false;


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

    // 콜라이더 그리기 토글
    if (input.GetKeyPressed('T')) {
        SceneManager::Instance().ToggleDrawColider();
        std::cout << "ToggleDrawColider" << std::endl;
    }

    cam = D2DRenderer::Instance().GetCamera();
    //obj = GameObject

    if (cam)
    {
        D2D1_MATRIX_3X2_F view = cam->GetViewMatrix();
        D2DRenderer::Instance().SetTransform(view);

        const float moveSpeed = 300.0f;    // px/sec
        const float rotateSpeed = 90.0f;   // deg/sec
        const float zoomFactor = 0.8f;     // 줌 비율
        float dt = m_EngineTimer->DeltaTime();

        m_fader.Update(dt);

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
        if (input.IsKeyDown('Z'))
            cam->Rotate(-rotateSpeed * dt);

        if (input.IsKeyDown('X'))
            cam->Rotate(rotateSpeed * dt);

        // 줌
        if (input.IsKeyDown('C')) {
            D2D1_POINT_2F screenCenter = {
                cam->GetScreenWidth() * 0.5f,
                cam->GetScreenHeight() * 0.5f
            };
            cam->Zoom(1.f + dt, screenCenter);
        }

        if (input.IsKeyDown('V')) {
            D2D1_POINT_2F screenCenter = {
                cam->GetScreenWidth() * 0.5f,
                cam->GetScreenHeight() * 0.5f
            };
            cam->Zoom(1.f - dt, screenCenter);
        }

        if (input.IsKeyDown('V')) {

        }

        if (input.GetKeyPressed(VK_SPACE))
        {
            if (flag)
                cam->Shake(10.0f, 0.1f);
            else
                cam->Shake(0.0f, 0.1f);

            flag = !flag;
        }

        if (input.GetKeyPressed(VK_F1))
        {
            m_fader.FadeIn(1.0f, 1.0f, D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));
        }
        if (input.GetKeyPressed(VK_F2))
        {
            m_fader.FadeOut(2.5f, 0.0f, D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));
        }




        if (input.GetKeyPressed(VK_F3))
        {
            m_fader.FadeIn(0.1f, 0.5f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f));
        }
        if (input.GetKeyPressed(VK_F4))
        {
            m_fader.FadeIn(0.1f, 0.5f, D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));
        }
        if (input.GetKeyPressed(VK_F5))
        {
            m_fader.FadeIn(0.1f, 0.5f, D2D1::ColorF(1.0f, 0.0f, 0.0f, 1.0f));
        }
        if (input.GetKeyPressed(VK_F6))
        {
            m_fader.FadeIn(0.1f, 0.5f, D2D1::ColorF(0.0f, 0.0f, 1.0f, 1.0f));
        }
        if (input.GetKeyPressed(VK_F7))
        {
            m_fader.FadeIn(0.1f, 0.5f, D2D1::ColorF(0.0f, 1.0f, 0.0f, 1.0f));
        }
        if (input.GetKeyPressed(VK_F8))
        {
            m_fader.FadeIn(0.1f, 0.5f, D2D1::ColorF(0.0f, 1.0f, 1.0f, 1.0f));
        }
        if (input.GetKeyPressed(VK_F9))
        {
            m_fader.FadeIn(0.1f, 0.5f, D2D1::ColorF(1.0f, 0.0f, 1.0f, 1.0f));
        }
        if (input.GetKeyPressed(VK_F10))
        {
            m_fader.FadeIn(0.1f, 0.5f, D2D1::ColorF(1.0f, 1.0f, 0.0f, 1.0f));
        }
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

    SceneManager::Instance().Render(deltaTime);

    renderer.SetTransform(D2D1::Matrix3x2F::Identity());

    m_fader.Render(renderer.GetD2DContext(), screenSize);

    renderer.RenderEnd(false);

    RenderImGui();

    renderer.Present();
}

void EngineCore::RenderImGui()
{
    ID3D11DeviceContext* context = D2DRenderer::Instance().GetD3DContext();
    ID3D11RenderTargetView* rtv = D2DRenderer::Instance().GetD3DRenderTargetView();

    if (!context || !rtv) return;

    // --- 레이아웃 설정 ---
    const float labelWidth = 80.0f;
    const float comboWidth = 80.0f; // 콤보박스의 너비

    ////////////////////////////////////////////////////////////////////////////////
    // 하이어라키 그리기
    ////////////////////////////////////////////////////////////////////////////////

    ImGui::Begin("Hierarchy");

    // Game Objects
    if (ImGui::CollapsingHeader("Game Objects", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (auto& obj : SceneManager::Instance().GetCurrentScene()->GetGameObjects())
        {
            std::string nameStr = WStringToString(obj->GetID()) + ". " + WStringToString(obj->GetName());

            // 현재 선택된 오브젝트와 비교해서 선택 상태 지정 (선택된 오브젝트 포인터 필요)
            auto selectedObj = SceneManager::Instance().GetCurrentScene()->GetSelectedObject();
            bool isSelected = (selectedObj == obj.get());

            if (ImGui::Selectable(nameStr.c_str(), isSelected))
            {
                // 클릭 시 선택된 오브젝트 변경
                SceneManager::Instance().GetCurrentScene()->SetSelectedObject(obj.get());
            }
        }
    }

    // UI Objects
    if (ImGui::CollapsingHeader("UI Objects", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (auto& obj : SceneManager::Instance().GetCurrentScene()->GetUIObjects())
        {
            std::string nameStr = WStringToString(obj->GetID()) + ". " + WStringToString(obj->GetName());

            auto selectedObj = SceneManager::Instance().GetCurrentScene()->GetSelectedObject();
            bool isSelected = (selectedObj == obj.get());

            if (ImGui::Selectable(nameStr.c_str(), isSelected))
            {
                SceneManager::Instance().GetCurrentScene()->SetSelectedObject(obj.get());
            }
        }
    }

    ImGui::End();

    ////////////////////////////////////////////////////////////////////////////////
    // 인스펙터 창
    ////////////////////////////////////////////////////////////////////////////////

    ImGui::Begin("Inspector");

    // 선택된 오브젝트 정보 출력
    JDGameObject::GameObjectBase* selectObject
        = SceneManager::Instance().GetCurrentScene()->GetSelectedObject();

    if (selectObject != nullptr)
    {
        ////////////////////////////////////////////////////////////////////////////////
        // GameObjectBase
        ////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // ID
        // ID는 읽기 전용으로 출력
        std::wstring wid = selectObject->GetID();
        std::string id(wid.begin(), wid.end());
        ImGui::Text("ID:");
        ImGui::SameLine(30.f);
        ImGui::Text("%s", id.c_str());

        // Name은 편집 가능하게 변환
        std::wstring wname = selectObject->GetName();
        std::string nameStr(wname.begin(), wname.end());

        // ImGui InputText를 위한 버퍼 생성 (std::string → char[])
        // 버퍼 크기는 적당히 넉넉히 설정 (예: 256)
        char buffer[256];
        strncpy_s(buffer, sizeof(buffer), nameStr.c_str(), _TRUNCATE);
        buffer[sizeof(buffer) - 1] = '\0'; // 안전하게 널 종료

        ////////////////////////////////////////////////////////////////////////////////
        // 이름
        ImGui::Text("Name");
        ImGui::SameLine(50.f);

        if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
        {
            // 입력이 변경됐을 때 다시 std::wstring으로 변환해서 설정
            std::wstring newName(buffer, buffer + strlen(buffer));
            selectObject->SetName(newName);
        }

        ////////////////////////////////////////////////////////////////////////////////
        // 활성화 여부 - IsActive
        ImGui::Text("Active"); ImGui::SameLine();
        bool isActive = selectObject->IsActive();
        ImGui::Checkbox("##Active", &isActive);
        selectObject->SetActive(isActive);

        ////////////////////////////////////////////////////////////////////////////////
        // 레이어
        int layer = selectObject->GetLayer();
        ImGui::SameLine(90.0f);
        ImGui::Text("Layer"); ImGui::SameLine();
        ImGui::PushItemWidth(50);

        if (ImGui::DragInt("##Layer", &layer, 1.0f, 0, 100)) {
            selectObject->SetLayer(layer);
        }
        ImGui::PopItemWidth();

        ////////////////////////////////////////////////////////////////////////////////
        // 태그
        static const char* tagNames[] = {
            "None", "Player", "PlayerCat", "PlayerBuilding",
            "Enemy", "EnemyCat", "EnemyBuilding", "NPC", "UI"
        };
        int currentTag = static_cast<int>(selectObject->GetTag());
        ImGui::SameLine(200.0f);
        ImGui::Text("Tag"); ImGui::SameLine();
        ImGui::PushItemWidth(100);
        if (ImGui::Combo("##Tag", &currentTag, tagNames, IM_ARRAYSIZE(tagNames))) {
            selectObject->SetTag(static_cast<JDGlobal::Base::GameTag>(currentTag));
        }
        ImGui::PopItemWidth();

        //////////////////////////////////////////////////////////////////////////
        ////테스트용//////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        if (auto* grid = dynamic_cast<Grid*>(selectObject)) {
            ImGui::Text("isOccupied - 확장 완료했는지"); ImGui::SameLine();
            bool isOccupied = grid->IsOccupied();
            ImGui::Checkbox("##isOccupied", &isOccupied);
            grid->SetOccupied(isOccupied);

            ImGui::Text("isExpanded - 레벨업 시 확장 선택 가능한지"); ImGui::SameLine();
            bool isExpanded = grid->IsExpanded();
            ImGui::Checkbox("##isExpanded", &isExpanded);
            grid->SetExpanded(isExpanded);

            ImGui::Text("hasBuilding - 건물 소유중인지"); ImGui::SameLine();
            bool hasBuilding = grid->HasBuilding();
            ImGui::Checkbox("##hasBuilding", &hasBuilding);
            grid->SetHasBuilding(hasBuilding);
        }
        //////////////////////////////////////////////////////////////////////////
        ////테스트용//////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////
        // Transform
        ////////////////////////////////////////////////////////////////////////////////

        JDComponent::D2DTM::Transform* transform =
            selectObject->GetComponent<JDComponent::D2DTM::Transform>();

        // (1) 객체의 크기를 안정적으로 가져오는 로직
        D2D1_SIZE_F objectSize = { 0, 0 };
        bool hasSizeInfo = false;

        // 1순위: 콜라이더에서 크기 정보 가져오기
        auto col = selectObject->GetComponent<JDComponent::ColliderBase>();
        if (col)
        {
            hasSizeInfo = true;
            if (col->GetColliderType() == JDComponent::ColliderType::Box)
            {
                // BoxCollider로 타입 캐스팅 후 GetSize() 호출
                if (auto boxCollider = dynamic_cast<JDComponent::BoxCollider*>(col))
                {
                    const auto& sizeVec = boxCollider->GetSize();
                    objectSize = { sizeVec.x, sizeVec.y };
                }
            }
            else if (col->GetColliderType() == JDComponent::ColliderType::Circle)
            {
                // CircleCollider로 타입 캐스팅 후 GetRadius() 호출
                if (auto circleCollider = dynamic_cast<JDComponent::CircleCollider*>(col))
                {
                    float radius = circleCollider->GetRadius();
                    objectSize = { radius * 2.0f, radius * 2.0f };
                }
            }
        }
        // 2순위: 콜라이더가 없다면 스프라이트에서 크기 정보 가져오기
        else
        {
            auto trComp = selectObject->GetComponent<JDComponent::TextureRenderer>();
            if (trComp)
            {
                hasSizeInfo = true;
                objectSize = trComp->GetOriginalTextureSize(); // 스프라이트의 원본 크기
            }
        }

        if (transform)
        {
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 105, 180, 255)); // 핑크색 (분홍색)
            ImGui::Text("Transform");
            ImGui::PopStyleColor();
            ImGui::Spacing();

            ////////////////////////////////////////////////////////////////////////////////
            // 위치 (Position)
            ImGui::Text("Position_");
            ImGui::SameLine(labelWidth);
            ImGui::PushItemWidth(-1.0f);

            JDComponent::D2DTM::Transform::Vec2 position = transform->GetPosition();
            if (ImGui::DragFloat2("##Position_", &position.x, 0.1f))  // 0.1f는 드래그 감도
            {
                transform->SetPosition(position);
            }

            ////////////////////////////////////////////////////////////////////////////////
            // 회전 (Rotation)
            ImGui::Text("Rotation_");
            ImGui::SameLine(labelWidth);
            ImGui::PushItemWidth(-1.0f);

            float rotation = transform->GetRotation();
            if (ImGui::DragFloat("##Rotation_", &rotation, 0.1f))
            {
                transform->SetRotation(rotation);
            }

            ////////////////////////////////////////////////////////////////////////////////
            // 스케일 (Scale)
            ImGui::Text("Scale_");
            ImGui::SameLine(labelWidth);
            ImGui::PushItemWidth(-1.0f);

            JDComponent::D2DTM::Transform::Vec2 scale = transform->GetScale();
            if (ImGui::DragFloat2("##Scale_", &scale.x, 0.01f))  // 스케일은 좀 더 세밀하게
            {
                transform->SetScale(scale);
            }

            ////////////////////////////////////////////////////////////////////////////////
            // 피벗 (Pivot)
            ImGui::Text("Pivot_");
            ImGui::SameLine(labelWidth);
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - comboWidth);

            // (2) UI 표시를 위해 '상대 피벗' 값을 가져옵니다.
            JDComponent::D2DTM::Transform::Vec2 relativePivot = transform->GetRelativePivot(objectSize);

            if (ImGui::DragFloat2("##Pivot_", &relativePivot.x, 0.01f, 0.0f, 1.0f))
            {
                // (3) 수정된 상대 좌표를 다시 '픽셀 좌표'로 변환하여 SetPivot으로 설정합니다.
                D2D1_POINT_2F newPixelPivot = {
                    relativePivot.x * objectSize.width,
                    relativePivot.y * objectSize.height
                };
                transform->SetPivot(newPixelPivot);
            }

            // 피벗 프리셋을 위한 콤보박스
            ImGui::PopItemWidth();
            ImGui::SameLine();
            ImGui::PushItemWidth(comboWidth);
            const char* pivotPresets[] = {
                "Top-Left", "Top-Center", "Top-Right",
                "Center-Left", "Center", "Center-Right",
                "Bottom-Left", "Bottom-Center", "Bottom-Right"
            };

            // 현재 피벗 값(상대 좌표)을 보고 어떤 프리셋에 해당하는지 인덱스를 찾습니다.
            int currentPivotPreset = -1; // 일치하는 프리셋이 없으면 -1 (Custom 상태)
            if (relativePivot.x == 0.0f && relativePivot.y == 1.0f)         currentPivotPreset = 0; // TopLeft
            else if (relativePivot.x == 0.5f && relativePivot.y == 1.0f)    currentPivotPreset = 1; // TopCenter
            else if (relativePivot.x == 1.0f && relativePivot.y == 1.0f)    currentPivotPreset = 2; // TopRight
            else if (relativePivot.x == 0.0f && relativePivot.y == 0.5f)    currentPivotPreset = 3; // CenterLeft
            else if (relativePivot.x == 0.5f && relativePivot.y == 0.5f)    currentPivotPreset = 4; // Center
            else if (relativePivot.x == 1.0f && relativePivot.y == 0.5f)    currentPivotPreset = 5; // CenterRight
            else if (relativePivot.x == 0.0f && relativePivot.y == 0.0f)    currentPivotPreset = 6; // BottomLeft
            else if (relativePivot.x == 0.5f && relativePivot.y == 0.0f)    currentPivotPreset = 7; // BottomCenter
            else if (relativePivot.x == 1.0f && relativePivot.y == 0.0f)    currentPivotPreset = 8; // BottomRight

            // 선택된 프리셋이 있으면 해당 이름을, 없으면 "Custom"을 표시합니다.
            if (ImGui::BeginCombo("##PivotPreset_", currentPivotPreset != -1 ? pivotPresets[currentPivotPreset] : "Custom"))
            {
                // 배열에 있는 모든 프리셋을 항목으로 표시합니다.
                for (int n = 0; n < IM_ARRAYSIZE(pivotPresets); n++)
                {
                    const bool is_selected = (currentPivotPreset == n);
                    if (ImGui::Selectable(pivotPresets[n], is_selected))
                    {
                        // 항목을 선택하면, 해당 인덱스(n)를 PivotPreset enum으로 변환하여
                        // SetPivot(PivotPreset) 함수를 직접 호출합니다.
                        // 이 코드는 enum의 정수 값이 0부터 8까지 순서대로라는 것을 전제로 합니다.
                        transform->SetPivotPreset((JDComponent::D2DTM::Transform::PivotPreset)n, objectSize);
                    }

                    // 현재 선택된 항목으로 스크롤을 자동으로 맞춰줍니다.
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();
        }

        ////////////////////////////////////////////////////////////////////////////////
        // TextureRenderer
        ////////////////////////////////////////////////////////////////////////////////

        JDComponent::TextureRenderer* textureRenderer =
            selectObject->GetComponent<JDComponent::TextureRenderer>();

        if (textureRenderer)
        {
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 105, 180, 255)); // 핑크색 (분홍색)
            ImGui::Text("TextureRenderer");
            ImGui::PopStyleColor();
            ImGui::Spacing();

            ////////////////////////////////////////////////////////////////////////////////
            // 텍스처 변경
            // AssetManager에서 텍스처 이름 목록 가져오기
            std::vector<std::string> tNames;
            for (const auto& pair : AssetManager::Instance().GetTextures()) {
                tNames.push_back(pair.first);
            }

            // 현재 선택된 텍스처 이름 인덱스 찾기
            std::string currentName = textureRenderer->GetTextureName();
            int currentIndex = 0;
            for (int i = 0; i < tNames.size(); ++i) {
                if (tNames[i] == currentName) {
                    currentIndex = i;
                    break;
                }
            }

            // 텍스처 선택 콤보박스
            ImGui::Text("Texture_");
            ImGui::SameLine(labelWidth);
            ImGui::PushItemWidth(200.0f);

            if (ImGui::BeginCombo("##Texture_", tNames[currentIndex].c_str())) {
                for (int i = 0; i < tNames.size(); ++i) {
                    const bool isSelected = (i == currentIndex);
                    if (ImGui::Selectable(tNames[i].c_str(), isSelected)) {
                        textureRenderer->SetTextureName(tNames[i]);
                    }
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();

            ////////////////////////////////////////////////////////////////////////////////
            // 텍스처 레이어 변경

            JDGlobal::Base::RenderLayerInfo layerInfo = textureRenderer->GetLayerInfo();

            // Sorting Layer
            ImGui::Text("Sorting Layer");
            ImGui::SameLine();
            ImGui::SetCursorPosX(150);
            ImGui::PushItemWidth(200.0f);

            // SortingLayer enum을 문자열로 보여주기 위한 예시
            const char* sortingLayerNames[] = { "None", "Background", "Default", "Foreground" };
            int currentLayer = static_cast<int>(layerInfo.sortingLayer);
            if (ImGui::Combo("##SortingLayerCombo", &currentLayer, sortingLayerNames, IM_ARRAYSIZE(sortingLayerNames))) {
                layerInfo.sortingLayer = static_cast<JDGlobal::Base::SortingLayer>(currentLayer);
                textureRenderer->SetLayerInfo(layerInfo);
            }
            ImGui::PopItemWidth();


            // Order in Layer
            ImGui::Text("Order in Layer");
            ImGui::SameLine();
            ImGui::SetCursorPosX(150);
            ImGui::PushItemWidth(200.0f);

            int orderInLayer = layerInfo.orderInLayer;
            if (ImGui::InputInt("##OrderInLayerInput", &orderInLayer)) {
                layerInfo.orderInLayer = orderInLayer;
                textureRenderer->SetLayerInfo(layerInfo);
            }
            ImGui::PopItemWidth();

        }


        ////////////////////////////////////////////////////////////////////////////////
        // ColliderBase
        ////////////////////////////////////////////////////////////////////////////////

        JDComponent::ColliderBase* collider =
            selectObject->GetComponent<JDComponent::ColliderBase>();

        if (collider)
        {
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 105, 180, 255)); // 핑크색 (분홍색)
            ImGui::Text("Collider");
            ImGui::PopStyleColor();
            ImGui::Spacing();

            ////////////////////////////////////////////////////////////////////////////////
            // 오프셋 편집
            ImGui::Text("C_Offset");
            ImGui::SameLine(labelWidth);
            ImGui::PushItemWidth(-1.0f);

            JDGlobal::Math::Vector2F offset = collider->GetColliderOffset();
            if (ImGui::DragFloat2("##C_Offset", &offset.x, 0.01f))
            {
                collider->SetColliderOffset(offset);
            }

            ////////////////////////////////////////////////////////////////////////////////
            // 트리거 여부
            ImGui::Text("IsTrigger");
            ImGui::SameLine();
            ImGui::SetCursorPosX(labelWidth);
            ImGui::PushItemWidth(200);

            bool isTrigger = collider->IsTrigger();
            if (ImGui::Checkbox("##IsTrigger", &isTrigger))
            {
                collider->SetTrigger(isTrigger);
            }
            ImGui::PopItemWidth();


            ////////////////////////////////////////////////////////////////////////////////
            // 타입별 속성 표시
            if (collider->GetColliderType() == JDComponent::ColliderType::Circle)
            {
                auto* circle = dynamic_cast<JDComponent::CircleCollider*>(collider);
                if (circle)
                {
                    ImGui::Text("C_Radius");
                    ImGui::SameLine();
                    ImGui::SetCursorPosX(labelWidth);
                    ImGui::PushItemWidth(200);

                    float radius = circle->GetRadius();
                    if (ImGui::DragFloat("##C_Radius", &radius, 0.01f, 0.0f))
                    {
                        circle->SetRadius(radius);
                    }
                    ImGui::PopItemWidth();
                }


            }
            else if (collider->GetColliderType() == JDComponent::ColliderType::Box)
            {
                auto* box = dynamic_cast<JDComponent::BoxCollider*>(collider);
                if (box)
                {
                    ImGui::Text("C_BoxSize");
                    ImGui::SameLine();
                    ImGui::SetCursorPosX(labelWidth);
                    ImGui::PushItemWidth(200);

                    JDGlobal::Math::Vector2F halfSize = box->GetHalfSize();
                    if (ImGui::DragFloat2("##C_BoxSize", &halfSize.x, 0.01f, 0.0f))
                    {
                        box->SetHalfSize(halfSize);
                    }
                    ImGui::PopItemWidth();
                }

            }
        }

        ////////////////////////////////////////////////////////////////////////////////
        // AnimationRender
        ////////////////////////////////////////////////////////////////////////////////

        JDComponent::AnimationRender* animRenderer =
            selectObject->GetComponent<JDComponent::AnimationRender>();

        if (animRenderer)
        {
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 105, 180, 255)); // 핑크색 (분홍색)
            ImGui::Text("Animation");
            ImGui::PopStyleColor();
            ImGui::Spacing();

            ////////////////////////////////////////////////////////////////////////////////
            // 애니메이션 클립 선택
            std::vector<std::string> clipNames;
            for (const auto& pair : AssetManager::Instance().GetAnimations()) {
                clipNames.push_back(pair.first);
            }

            // 현재 선택된 클립 이름 인덱스 찾기
            std::string currentClip = animRenderer->GetClipName(); // 또는 animRenderer.m_clipName
            int currentIndex = 0;
            for (int i = 0; i < clipNames.size(); ++i) {
                if (clipNames[i] == currentClip) {
                    currentIndex = i;
                    break;
                }
            }

            ImGui::Text("Clip Name");
            ImGui::SameLine();
            ImGui::SetCursorPosX(labelWidth);

            if (ImGui::BeginCombo("##ClipCombo", currentClip.c_str())) {
                for (int i = 0; i < clipNames.size(); ++i) {
                    bool isSelected = (i == currentIndex);
                    if (ImGui::Selectable(clipNames[i].c_str(), isSelected)) {
                        animRenderer->SetClipName(clipNames[i]);
                    }
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ////////////////////////////////////////////////////////////////////////////////
            // 재생 속도
            ImGui::Text("AnimSpeed");
            ImGui::SameLine();
            ImGui::SetCursorPosX(labelWidth);

            float speed = animRenderer->GetSpeed();
            if (ImGui::DragFloat("##AnimSpeed", &speed, 0.1f, 0.1f, 10.0f, "%.2fx")) {
                animRenderer->SetSpeed(speed);
            }
            ImGui::PopItemWidth();

            ////////////////////////////////////////////////////////////////////////////////
            // 렌더 레이어 정보

            // Sorting Layer
            auto layerInfo = animRenderer->GetLayerInfo();
            int currentLayer = static_cast<int>(layerInfo.sortingLayer);

            const char* layerNames[] = { "None", "Background", "Default", "UI" }; // 예시
            int layerCount = IM_ARRAYSIZE(layerNames);

            ImGui::Text("Sorting Layer");
            ImGui::SameLine();
            ImGui::SetCursorPosX(labelWidth);
            if (ImGui::Combo("##SortingLayer", &currentLayer, layerNames, layerCount)) {
                layerInfo.sortingLayer = static_cast<SortingLayer>(currentLayer);
                animRenderer->SetLayerInfo(layerInfo);
            }

            // Order In Layer
            int orderInLayer = layerInfo.orderInLayer;
            ImGui::Text("Order In Layer");
            ImGui::SameLine();
            ImGui::SetCursorPosX(labelWidth);
            if (ImGui::DragInt("##OrderInLayer", &orderInLayer, 1.0f, -100, 100)) {
                layerInfo.orderInLayer = orderInLayer;
                animRenderer->SetLayerInfo(layerInfo);
            }
            ImGui::PopItemWidth();
        }


        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // RectTransform
        ////////////////////////////////////////////////////////////////////////////////

        JDComponent::D2DTM::RectTransform* rectTransform =
            selectObject->GetComponent<JDComponent::D2DTM::RectTransform>();

        if (rectTransform)
        {
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 105, 180, 255)); // 핑크색 (분홍색)
            ImGui::Text("RectTransform");
            ImGui::PopStyleColor();
            ImGui::Spacing();

            ////////////////////////////////////////////////////////////////////////////////
            // 크기 (Size)
            ImGui::Text("Size");
            ImGui::SameLine(labelWidth);
            ImGui::PushItemWidth(-1.0f);

            JDComponent::D2DTM::RectTransform::Vector2F size = rectTransform->GetSize();
            if (ImGui::DragFloat2("##Size", &size.x, 0.1f))
            {
                rectTransform->SetSize(size);
            }

            ////////////////////////////////////////////////////////////////////////////////
            // 위치 (Position)
            ImGui::Text("Position");
            ImGui::SameLine(labelWidth);
            ImGui::PushItemWidth(-1.0f);

            JDComponent::D2DTM::RectTransform::Vector2F position = rectTransform->GetPosition();
            if (ImGui::DragFloat2("##Position", &position.x, 0.1f))  // 0.1f는 드래그 감도
            {
                rectTransform->SetPosition(position);
            }

            ////////////////////////////////////////////////////////////////////////////////
            // 회전 (Rotation)
            ImGui::Text("Rotation");
            ImGui::SameLine(labelWidth);
            ImGui::PushItemWidth(-1.0f);

            float rotation = rectTransform->GetRotation();
            if (ImGui::DragFloat("##Rotation", &rotation, 0.1f))
            {
                rectTransform->SetRotation(rotation);
            }

            ////////////////////////////////////////////////////////////////////////////////
            // 스케일 (Scale)
            ImGui::Text("Scale");
            ImGui::SameLine(labelWidth);
            ImGui::PushItemWidth(-1.0f);

            JDComponent::D2DTM::RectTransform::Vector2F scale = rectTransform->GetScale();
            if (ImGui::DragFloat2("##Scale", &scale.x, 0.01f))  // 스케일은 좀 더 세밀하게
            {
                rectTransform->SetScale(scale);
            }

            ////////////////////////////////////////////////////////////////////////////////
            // 피벗 (Pivot)
            ImGui::Text("Pivot");
            ImGui::SameLine(labelWidth);

            // DragFloat2가 차지할 너비 = (전체 너비 - 콤보박스 너비)
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - comboWidth);

            JDComponent::D2DTM::RectTransform::Vector2F pivot = rectTransform->GetPivot();
            if (ImGui::DragFloat2("##Pivot", &pivot.x, 0.01f, 0.0f, 1.0f))
            {
                // 수정된 상대 좌표를 SetPivot으로 다시 설정
                rectTransform->SetPivot(pivot);
            }

            // 피벗 프리셋을 위한 콤보박스
            ImGui::PopItemWidth();
            ImGui::SameLine();
            ImGui::PushItemWidth(comboWidth);
            const char* pivotPresets[] = {
                "Top-Left", "Top-Center", "Top-Right",
                "Center-Left", "Center", "Center-Right",
                "Bottom-Left", "Bottom-Center", "Bottom-Right"
            };

            // 현재 피벗 값(상대 좌표)을 보고 어떤 프리셋에 해당하는지 인덱스를 찾습니다.
            int currentPivotPreset = -1; // 일치하는 프리셋이 없으면 -1 (Custom 상태)
            if (pivot.x == 0.0f && pivot.y == 1.0f)         currentPivotPreset = 0; // TopLeft
            else if (pivot.x == 0.5f && pivot.y == 1.0f)    currentPivotPreset = 1; // TopCenter
            else if (pivot.x == 1.0f && pivot.y == 1.0f)    currentPivotPreset = 2; // TopRight
            else if (pivot.x == 0.0f && pivot.y == 0.5f)    currentPivotPreset = 3; // CenterLeft
            else if (pivot.x == 0.5f && pivot.y == 0.5f)    currentPivotPreset = 4; // Center
            else if (pivot.x == 1.0f && pivot.y == 0.5f)    currentPivotPreset = 5; // CenterRight
            else if (pivot.x == 0.0f && pivot.y == 0.0f)    currentPivotPreset = 6; // BottomLeft
            else if (pivot.x == 0.5f && pivot.y == 0.0f)    currentPivotPreset = 7; // BottomCenter
            else if (pivot.x == 1.0f && pivot.y == 0.0f)    currentPivotPreset = 8; // BottomRight

            // 선택된 프리셋이 있으면 해당 이름을, 없으면 "Custom"을 표시합니다.
            if (ImGui::BeginCombo("##PivotPreset", currentPivotPreset != -1 ? pivotPresets[currentPivotPreset] : "Custom"))
            {
                // 배열에 있는 모든 프리셋을 항목으로 표시합니다.
                for (int n = 0; n < IM_ARRAYSIZE(pivotPresets); n++)
                {
                    const bool is_selected = (currentPivotPreset == n);
                    if (ImGui::Selectable(pivotPresets[n], is_selected))
                    {
                        // 항목을 선택하면, 해당 인덱스(n)를 PivotPreset enum으로 변환하여
                        // SetPivot(PivotPreset) 함수를 직접 호출합니다.
                        // 이 코드는 enum의 정수 값이 0부터 8까지 순서대로라는 것을 전제로 합니다.
                        rectTransform->SetPivot((JDComponent::D2DTM::RectTransform::PivotPreset)n);
                    }

                    // 현재 선택된 항목으로 스크롤을 자동으로 맞춰줍니다.
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();

            ////////////////////////////////////////////////////////////////////////////////
            // 앵커 (Anchor)
            ImGui::Text("Anchor");
            ImGui::SameLine(labelWidth);
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - comboWidth);

            JDComponent::D2DTM::RectTransform::Vector2F anchor = rectTransform->GetAnchor();
            if (ImGui::DragFloat2("##Anchor", &anchor.x, 0.01f, 0.0f, 1.0f))
            {
                // 변경된 상대 좌표 값으로 앵커를 설정합니다.
                rectTransform->SetAnchor(anchor);
            }

            // 앵커 프리셋을 위한 콤보박스
            ImGui::PopItemWidth();
            ImGui::SameLine();
            ImGui::PushItemWidth(comboWidth);
            const char* anchorPresets[] = {
                "Top-Left", "Top-Center", "Top-Right",
                "Center-Left", "Center", "Center-Right",
                "Bottom-Left", "Bottom-Center", "Bottom-Right"
                // Stretch 프리셋은 현재 구현에서 제외
            };

            // 현재 앵커 값과 일치하는 프리셋의 인덱스를 찾습니다.
            int currentAnchorPreset = -1;
            if (anchor.x == 0.0f && anchor.y == 1.0f)       currentAnchorPreset = 0; // TopLeft
            else if (anchor.x == 0.5f && anchor.y == 1.0f)  currentAnchorPreset = 1; // TopCenter
            else if (anchor.x == 1.0f && anchor.y == 1.0f)  currentAnchorPreset = 2; // TopRight
            else if (anchor.x == 0.0f && anchor.y == 0.5f)  currentAnchorPreset = 3; // CenterLeft
            else if (anchor.x == 0.5f && anchor.y == 0.5f)  currentAnchorPreset = 4; // Center
            else if (anchor.x == 1.0f && anchor.y == 0.5f)  currentAnchorPreset = 5; // CenterRight
            else if (anchor.x == 0.0f && anchor.y == 0.0f)  currentAnchorPreset = 6; // BottomLeft
            else if (anchor.x == 0.5f && anchor.y == 0.0f)  currentAnchorPreset = 7; // BottomCenter
            else if (anchor.x == 1.0f && anchor.y == 0.0f)  currentAnchorPreset = 8; // BottomRight

            if (ImGui::BeginCombo("##AnchorPreset", currentAnchorPreset != -1 ? anchorPresets[currentAnchorPreset] : "Custom"))
            {
                for (int n = 0; n < IM_ARRAYSIZE(anchorPresets); n++)
                {
                    const bool is_selected = (currentAnchorPreset == n);
                    if (ImGui::Selectable(anchorPresets[n], is_selected))
                    {
                        switch (n)
                        {
                        case 0: rectTransform->SetAnchor({ 0.0f, 1.0f }); break; // TopLeft
                        case 1: rectTransform->SetAnchor({ 0.5f, 1.0f }); break; // TopCenter
                        case 2: rectTransform->SetAnchor({ 1.0f, 1.0f }); break; // TopRight
                        case 3: rectTransform->SetAnchor({ 0.0f, 0.5f }); break; // CenterLeft
                        case 4: rectTransform->SetAnchor({ 0.5f, 0.5f }); break; // Center
                        case 5: rectTransform->SetAnchor({ 1.0f, 0.5f }); break; // CenterRight
                        case 6: rectTransform->SetAnchor({ 0.0f, 0.0f }); break; // BottomLeft
                        case 7: rectTransform->SetAnchor({ 0.5f, 0.0f }); break; // BottomCenter
                        case 8: rectTransform->SetAnchor({ 1.0f, 0.0f }); break; // BottomRight
                        }
                    }
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
        }

        ////////////////////////////////////////////////////////////////////////////////
        // UI_ImageComponent
        ////////////////////////////////////////////////////////////////////////////////

        JDComponent::UI_ImageComponent* imageComponent =
            selectObject->GetComponent<JDComponent::UI_ImageComponent>();

        if (imageComponent)
        {
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(135, 206, 250, 255)); // 연한 파랑색
            ImGui::Text("ImageComponent");
            ImGui::PopStyleColor();
            ImGui::Spacing();

            ////////////////////////////////////////////////////////////////////////////////
            // 텍스쳐

            // AssetManager에서 텍스처 이름 목록 가져오기
            std::vector<std::string> textureNames;
            for (const auto& pair : AssetManager::Instance().GetTextures()) {
                textureNames.push_back(pair.first);
            }

            // 현재 선택된 텍스처 이름 인덱스 찾기
            std::string currentName = imageComponent->GetTextureName();
            int currentIndex = 0;
            for (int i = 0; i < textureNames.size(); ++i) {
                if (textureNames[i] == currentName) {
                    currentIndex = i;
                    break;
                }
            }

            // 텍스처 선택 콤보박스
            ImGui::Text("Texture");
            ImGui::SameLine(labelWidth);
            ImGui::PushItemWidth(200.0f);

            if (ImGui::BeginCombo("##Texture", textureNames[currentIndex].c_str())) {
                for (int i = 0; i < textureNames.size(); ++i) {
                    const bool isSelected = (i == currentIndex);
                    if (ImGui::Selectable(textureNames[i].c_str(), isSelected)) {
                        imageComponent->SetTextureName(textureNames[i]);
                    }
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();

            ////////////////////////////////////////////////////////////////////////////////
            // 원본 크기로 사이즈 설정 버튼
            ImGui::SameLine();
            if (ImGui::Button("SetNativeSize", ImVec2(120, 0)))
            {
                imageComponent->SetSizeToOriginal();
            }

            ////////////////////////////////////////////////////////////////////////////////
            // 색상 (Image_Color)
            ImGui::Text("I_Color");
            ImGui::SameLine(labelWidth);
            ImGui::PushItemWidth(-1.0f);

            D2D1_COLOR_F imageColor = imageComponent->GetColor();
            float imageColorArr[4] = { imageColor.r, imageColor.g, imageColor.b, imageColor.a };
            if (ImGui::ColorEdit4("##I_Color", imageColorArr))
            {
                imageComponent->SetColor(D2D1::ColorF(imageColorArr[0], imageColorArr[1], imageColorArr[2], imageColorArr[3]));
            }
            ImGui::PopItemWidth();
        }

        ////////////////////////////////////////////////////////////////////////////////
        // UI_TextComponent
        ////////////////////////////////////////////////////////////////////////////////

        JDComponent::UI_TextComponent* textComponent =
            selectObject->GetComponent<JDComponent::UI_TextComponent>();

        if (textComponent)
        {
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(135, 206, 250, 255)); // 연한 파랑색
            ImGui::Text("TextComponent");
            ImGui::PopStyleColor();
            ImGui::Spacing();

            ////////////////////////////////////////////////////////////////////////////////
            // Text

            ImGui::Text("Text");
            ImGui::SameLine(labelWidth);
            ImGui::PushItemWidth(-1.0f);

            static char textBuffer[256];
            std::string currentText = WStringToUTF8(textComponent->GetText());
            strncpy_s(textBuffer, currentText.c_str(), sizeof(textBuffer));

            // 입력 후 변경되면 저장
            if (ImGui::InputText("##Text", textBuffer, sizeof(textBuffer)))
            {
                textComponent->SetText(UTF8ToWString(textBuffer));
            }

            ImGui::PopItemWidth();

            ////////////////////////////////////////////////////////////////////////////////
            // 색상 (Text_Color)
            ImGui::Text("T_Color");
            ImGui::SameLine(labelWidth);
            ImGui::PushItemWidth(-1.0f);

            D2D1_COLOR_F textColor = textComponent->GetColor();
            float textColorArr[4] = { textColor.r, textColor.g, textColor.b, textColor.a };
            if (ImGui::ColorEdit4("##T_Color", textColorArr))
            {
                textComponent->SetColor(D2D1::ColorF(textColorArr[0], textColorArr[1], textColorArr[2], textColorArr[3]));
            }

            ImGui::PopItemWidth();

            ////////////////////////////////////////////////////////////////////////////////
            // 텍스트 포맷 선택 ( 렌더러 단계에서 추가해야 함. )

            std::vector<std::string> formatNames;
            for (auto& pair : D2DRenderer::Instance().GetTextFormats()) {
                formatNames.push_back(pair.first);
            }

            std::string currentFormatName = textComponent->GetTextFormatName();
            int currentFormatIndex = 0;
            for (int i = 0; i < formatNames.size(); ++i) {
                if (formatNames[i] == currentFormatName) {
                    currentFormatIndex = i;
                    break;
                }
            }

            ImGui::Text("Text Format");
            ImGui::SameLine(labelWidth);
            ImGui::PushItemWidth(200.0f);

            const char* previewLabel =
                (currentFormatIndex >= 0 && currentFormatIndex < formatNames.size()) ?
                formatNames[currentFormatIndex].c_str() : "None";

            if (ImGui::BeginCombo("##TextFormat", previewLabel)) {
                for (int i = 0; i < formatNames.size(); ++i) {
                    const bool isSelected = (i == currentFormatIndex);
                    if (ImGui::Selectable(formatNames[i].c_str(), isSelected)) {
                        textComponent->SetTextFormatName(formatNames[i]);
                    }
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::PopItemWidth();
        }

        ////////////////////////////////////////////////////////////////////////////////
        // UI_ButtonComponent
        ////////////////////////////////////////////////////////////////////////////////

        JDComponent::UI_ButtonComponent* buttonComponent =
            selectObject->GetComponent<JDComponent::UI_ButtonComponent>();

        if (buttonComponent)
        {
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(135, 206, 250, 255)); // 연한 파랑색
            ImGui::Text("ButtonComponent");
            ImGui::PopStyleColor();
            ImGui::Spacing();

            ////////////////////////////////////////////////////////////////////////////////
            // 상호작용 가능 (Interactable)

            ImGui::Text("Interactable");
            ImGui::SameLine(labelWidth); // labelWidth 변수가 정의되어 있다고 가정

            // 현재 Interactable 상태 가져오기
            bool isInteractable = buttonComponent->GetInteractable();
            if (ImGui::Checkbox("##Interactable", &isInteractable))
            {
                buttonComponent->SetInteractable(isInteractable);
            }

            ////////////////////////////////////////////////////////////////////////////////
            // 현재 상태 (State) - 읽기 전용

            ImGui::Text("State");
            ImGui::SameLine(labelWidth);

            // 현재 버튼 상태를 문자열로 변환하여 표시 (디버깅용)
            const char* stateStr = "Unknown";
            switch (buttonComponent->GetState())
            {
            case JDComponent::ButtonState::Idle:
                stateStr = "Idle";
                break;
            case JDComponent::ButtonState::Hovered:
                stateStr = "Hovered";
                break;
            case JDComponent::ButtonState::Pressed:
                stateStr = "Pressed";
                break;
            }
            ImGui::Text(stateStr);

            ////////////////////////////////////////////////////////////////////////////////
            // 콜백 목록 표시
            auto displayCallbackList = [&](const char* title, const std::vector<JDComponent::CallbackInfo>& callbacks) {
                ImGui::Text(title);
                ImGui::BeginChild(title, ImVec2(0, 80), true);
                if (callbacks.empty()) {
                    ImGui::TextDisabled("(None)");
                }
                else {
                    for (size_t i = 0; i < callbacks.size(); ++i) {
                        ImGui::Text("[%zu]: %s", i, callbacks[i].name.c_str());
                    }
                }
                ImGui::EndChild();
                };

            ImGui::Spacing();
            displayCallbackList("On Enter", buttonComponent->GetOnEnterCallbacks());
            ImGui::Spacing();
            displayCallbackList("On Exit", buttonComponent->GetOnExitCallbacks());
            ImGui::Spacing();
            displayCallbackList("On Down", buttonComponent->GetOnDownCallbacks());
            ImGui::Spacing();
            displayCallbackList("On Click", buttonComponent->GetOnClickCallbacks());
        }


        ////////////////////////////////////////////////////////////////////////////////
        // UI_ButtonComponent
        ////////////////////////////////////////////////////////////////////////////////

        JDComponent::UI_SliderComponent* sliderComponent =
            selectObject->GetComponent<JDComponent::UI_SliderComponent>();

        if (sliderComponent)
        {
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(135, 206, 250, 255)); // 연한 파랑색
            ImGui::Text("SliderComponent");
            ImGui::PopStyleColor();
            ImGui::Spacing();

            ////////////////////////////////////////////////////////////////////////////////
            // 상호작용 가능 (Interactable)
            bool isInteractable = sliderComponent->GetInteractable();
            if (ImGui::Checkbox("Interactable", &isInteractable))
            {
                sliderComponent->SetInteractable(isInteractable);
            }

            ////////////////////////////////////////////////////////////////////////////////
            // 현재 값 (Value)
            float value = sliderComponent->GetValue();
            float minVal = sliderComponent->GetMinValue();
            float maxVal = sliderComponent->GetMaxValue();
            if (ImGui::SliderFloat("Value", &value, minVal, maxVal))
            {
                // ImGui에서 값이 변경되면 컴포넌트의 값을 업데이트
                // (콜백을 다시 호출하지 않도록 broadcast는 false로 설정)
                sliderComponent->SetValue(value, false);
            }

            ////////////////////////////////////////////////////////////////////////////////
            // 최소/최대 값 편집
            if (ImGui::InputFloat("Min Value", &minVal))
            {
                sliderComponent->SetMinValue(minVal);
            }
            if (ImGui::InputFloat("Max Value", &maxVal))
            {
                sliderComponent->SetMaxValue(maxVal);
            }

            ////////////////////////////////////////////////////////////////////////////////
            // 현재 상태 (읽기 전용)
            ImGui::Text("State: ");
            ImGui::SameLine();
            const char* stateStr = "Unknown";
            switch (sliderComponent->GetState())
            {
            case JDComponent::SliderState::Idle:     stateStr = "Idle"; break;
            case JDComponent::SliderState::Hovered:  stateStr = "Hovered"; break;
            case JDComponent::SliderState::Dragging: stateStr = "Dragging"; break;
            }
            ImGui::TextUnformatted(stateStr);

            ////////////////////////////////////////////////////////////////////////////////
            // 콜백 목록 표시 (읽기 전용)
            ImGui::Spacing();

            // 1. 인자가 없는 콜백 목록을 표시하는 헬퍼 람다 (OnEnter, OnExit 등)
            auto displaySimpleCallbackList = [&](const char* title, const std::vector<JDComponent::CallbackInfo>& callbacks) {
                ImGui::Text(title);
                ImGui::BeginChild(title, ImVec2(0, 60), true); // 높이를 약간 줄임
                if (callbacks.empty()) {
                    ImGui::TextDisabled("(None)");
                }
                else {
                    for (size_t i = 0; i < callbacks.size(); ++i) {
                        ImGui::Text("[%zu]: %s", i, callbacks[i].name.c_str());
                    }
                }
                ImGui::EndChild();
                };

            // 2. float 인자가 있는 콜백 목록을 표시하는 헬퍼 람다 (OnValueChanged)
            auto displayValueCallbackList = [&](const char* title, const std::vector<JDComponent::ValueCallbackInfo>& callbacks) {
                ImGui::Text(title);
                ImGui::BeginChild(title, ImVec2(0, 60), true);
                if (callbacks.empty()) {
                    ImGui::TextDisabled("(None)");
                }
                else {
                    for (size_t i = 0; i < callbacks.size(); ++i) {
                        ImGui::Text("[%zu]: %s", i, callbacks[i].name.c_str());
                    }
                }
                ImGui::EndChild();
                };

            // 3. 각 콜백에 맞는 헬퍼 함수를 호출하여 목록을 표시
            displayValueCallbackList("On Value Changed", sliderComponent->GetOnValueChangedCallbacks());
            ImGui::Spacing();
            displaySimpleCallbackList("On Enter", sliderComponent->GetOnEnterCallbacks());
            ImGui::Spacing();
            displaySimpleCallbackList("On Exit", sliderComponent->GetOnExitCallbacks());
            ImGui::Spacing();
            displaySimpleCallbackList("On Down", sliderComponent->GetOnDownCallbacks());
            ImGui::Spacing();
            displaySimpleCallbackList("On Up", sliderComponent->GetOnUpCallbacks());
        }
    }
    else
    {
        ImGui::Text("No object selected");
    }
    ImGui::End();

    context->OMSetRenderTargets(1, &rtv, nullptr);

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
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

void EngineCore::LoadResources()
{
    // 예시 파일 Exam
////////////////////////////////////////////////////////////////////////////////
    if (!AssetManager::Instance().LoadTexture("TITLE_Exam", L"../Resource/TITLE_Exam.jpg"))
    {
        std::cout << "[ERROR] TITLE_Exam 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Option_Exam01", L"../Resource/Option_Exam01.png"))
    {
        std::cout << "[ERROR] Option_Exam01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Option_Exam02", L"../Resource/Option_Exam02.png"))
    {
        std::cout << "[ERROR] Option_Exam02 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Option_Exam03", L"../Resource/Option_Exam03.png"))
    {
        std::cout << "[ERROR] Option_Exam03 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("SelectCharacter_Exam", L"../Resource/SelectCharacter_Exam.png"))
    {
        std::cout << "[ERROR] SelectCharacter_Exam 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("BATTLE_MAP_Exam", L"../Resource/BATTLE_MAP_Exam.png"))
    {
        std::cout << "[ERROR] BATTLE_MAP_Exam 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("UI_Exam", L"../Resource/UI_Exam.png"))
    {
        std::cout << "[ERROR] UI_Exam 텍스처 로드 실패" << std::endl;
    }

    // TITLE
    ////////////////////////////////////////////////////////////////////////////////
    if (!AssetManager::Instance().LoadTexture("Test", L"../Resource/Texture/Test.png"))
    {
        std::cout << "[ERROR] Test 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ATest", L"../Resource/Texture/ATest.png"))
    {
        std::cout << "[ERROR] ATest 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Title", L"../Resource/TITLE/TITLE.png"))
    {
        std::cout << "[ERROR] Title 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("GAME_START_A", L"../Resource/TITLE/GAME_START_A.png"))
    {
        std::cout << "[ERROR] GAME_START_A 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("GAME_START_B", L"../Resource/TITLE/GAME_START_B.png"))
    {
        std::cout << "[ERROR] GAME_START_B 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("SETTING_A", L"../Resource/TITLE/SETTING_A.png"))
    {
        std::cout << "[ERROR] SETTING_A 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("SETTING_B", L"../Resource/TITLE/SETTING_B.png"))
    {
        std::cout << "[ERROR] SETTING_B 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("QUIT_GAME_A", L"../Resource/TITLE/QUIT_GAME_A.png"))
    {
        std::cout << "[ERROR] QUITGAME_A 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("QUIT_GAME_B", L"../Resource/TITLE/QUIT_GAME_B.png"))
    {
        std::cout << "[ERROR] QUITGAME_B 텍스처 로드 실패" << std::endl;
    }

    // OPTION
    ////////////////////////////////////////////////////////////////////////////////
    if (!AssetManager::Instance().LoadTexture("OPTION_1", L"../Resource/OPTION/OPTION_1.png"))
    {
        std::cout << "[ERROR] OPTION_1 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("OPTION_2", L"../Resource/OPTION/OPTION_2.png"))
    {
        std::cout << "[ERROR] OPTION_2 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("OPTION_3", L"../Resource/OPTION/OPTION_3.png"))
    {
        std::cout << "[ERROR] OPTION_3 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("BACK_1", L"../Resource/OPTION/BACK_1.png"))
    {
        std::cout << "[ERROR] BACK_1 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("BACK_2", L"../Resource/OPTION/BACK_2.png"))
    {
        std::cout << "[ERROR] BACK_2 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("BACK_GROUND_OPACITY", L"../Resource/OPTION/BACK_GROUND_OPACITY.png"))
    {
        std::cout << "[ERROR] BACK_GROUND_OPACITY 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("BACKTOTITLE_BUTTON_1", L"../Resource/OPTION/BACKTOTITLE_BUTTON_1.png"))
    {
        std::cout << "[ERROR] BACKTOTITLE_BUTTON_1 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("BACKTOTITLE_BUTTON_2", L"../Resource/OPTION/BACKTOTITLE_BUTTON_2.png"))
    {
        std::cout << "[ERROR] BACKTOTITLE_BUTTON_2 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("QUIT_BUTTON_1", L"../Resource/OPTION/QUIT_BUTTON_1.png"))
    {
        std::cout << "[ERROR] QUIT_BUTTON_1 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("QUIT_BUTTON_2", L"../Resource/OPTION/QUIT_BUTTON_2.png"))
    {
        std::cout << "[ERROR] QUIT_BUTTON_2 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("CONTROLS_BUTTON", L"../Resource/OPTION/CONTROLS_BUTTON.png"))
    {
        std::cout << "[ERROR] CONTROLS_BUTTON 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("CREDITS_BUTTON", L"../Resource/OPTION/CREDITS_BUTTON.png"))
    {
        std::cout << "[ERROR] CREDITS_BUTTON 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("VOLUME_BUTTON", L"../Resource/OPTION/VOLUME_BUTTON.png"))
    {
        std::cout << "[ERROR] VOLUME_BUTTON 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("VOLUME_CAT_1", L"../Resource/OPTION/VOLUME_CAT_1.png"))
    {
        std::cout << "[ERROR] VOLUME_CAT_1 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("VOLUME_CAT_2", L"../Resource/OPTION/VOLUME_CAT_2.png"))
    {
        std::cout << "[ERROR] VOLUME_CAT_2 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("VOLUME_LINE_1", L"../Resource/OPTION/VOLUME_LINE_1.png"))
    {
        std::cout << "[ERROR] VOLUME_LINE_1 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("VOLUME_LINE_2", L"../Resource/OPTION/VOLUME_LINE_2.png"))
    {
        std::cout << "[ERROR] VOLUME_LINE_2 텍스처 로드 실패" << std::endl;
    }

    // CHARACTER_SELECT
    ////////////////////////////////////////////////////////////////////////////////
    if (!AssetManager::Instance().LoadTexture("ART_Back01_mouseout", L"../Resource/CHARACTER SELECT/ART_Back01_mouseout.png"))
    {
        std::cout << "[ERROR] ART_Back01_mouseout 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Back01_mouseover", L"../Resource/CHARACTER SELECT/ART_Back01_mouseover.png"))
    {
        std::cout << "[ERROR] ART_Back01_mouseover 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_CH_BACK", L"../Resource/CHARACTER SELECT/ART_CH_BACK.png"))
    {
        std::cout << "[ERROR] ART_CH_BACK 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_CHAT", L"../Resource/CHARACTER SELECT/ART_CHAT.png"))
    {
        std::cout << "[ERROR] ART_CHAT 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_CHAT_mouseout", L"../Resource/CHARACTER SELECT/ART_CHAT_mouseout.png"))
    {
        std::cout << "[ERROR] ART_CHAT_mouseout 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_CHAT_mouseover", L"../Resource/CHARACTER SELECT/ART_CHAT_mouseover.png"))
    {
        std::cout << "[ERROR] ART_CHAT_mouseover 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Q_1", L"../Resource/CHARACTER SELECT/ART_Q_1.png"))  // 기본
    {
        std::cout << "[ERROR] ART_Q_1 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Q_2", L"../Resource/CHARACTER SELECT/ART_Q_2.png"))  // 코네
    {
        std::cout << "[ERROR] ART_Q_2 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Q_3", L"../Resource/CHARACTER SELECT/ART_Q_3.png"))  // 펠리스
    {
        std::cout << "[ERROR] ART_Q_3 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Q_4", L"../Resource/CHARACTER SELECT/ART_Q_4.png"))  // 나비
    {
        std::cout << "[ERROR] ART_Q_4 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_SelectFelis01_mouseout", L"../Resource/CHARACTER SELECT/ART_SelectFelis01_mouseout.png"))
    {
        std::cout << "[ERROR] ART_SelectFelis01_mouseout 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_SelectFelis01_mouseover", L"../Resource/CHARACTER SELECT/ART_SelectFelis01_mouseover.png"))
    {
        std::cout << "[ERROR] ART_SelectFelis01_mouseover 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_SelectKone01_mouseout", L"../Resource/CHARACTER SELECT/ART_SelectKone01_mouseout.png"))
    {
        std::cout << "[ERROR] ART_SelectKone01_mouseout 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_SelectKone01_mouseover", L"../Resource/CHARACTER SELECT/ART_SelectKone01_mouseover.png"))
    {
        std::cout << "[ERROR] ART_SelectKone01_mouseover 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_SelectNavi01_mouseout", L"../Resource/CHARACTER SELECT/ART_SelectNavi01_mouseout.png"))
    {
        std::cout << "[ERROR] ART_SelectNavi01_mouseout 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_SelectNavi01_mouseover", L"../Resource/CHARACTER SELECT/ART_SelectNavi01_mouseover.png"))
    {
        std::cout << "[ERROR] ART_SelectNavi01_mouseover 텍스처 로드 실패" << std::endl;
    }

    // 고양이 애니메이션 NORWAY, RUSS
    ////////////////////////////////////////////////////////////////////////////////

    // 애니메이션 속도 정배율을 1.4x 가 마음에 든다고 했음.
    if (!AssetManager::Instance().LoadTexture("Norway", L"../Resource/Animation/character_norway_sprite_01.png"))
    {
        std::cout << "[ERROR] Norway 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadAnimationRender("Norway", L"../Resource/Animation/character_norway_sprite_01.json"))
    {
        std::cout << "[ERROR] Norway 애니메이션 로드 실패!" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("Russ", L"../Resource/Animation/character_russ_sprite_01.png"))
    {
        std::cout << "[ERROR] Russ 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadAnimationRender("Russ", L"../Resource/Animation/character_russ_sprite_01.json"))
    {
        std::cout << "[ERROR] Russ 애니메이션 로드 실패!" << std::endl;
    }

    // BATTLE
    ////////////////////////////////////////////////////////////////////////////////
    if (!AssetManager::Instance().LoadTexture("CAT", L"../Resource/BATTLE/CAT.png"))
    {
        std::cout << "[ERROR] CAT 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("BATTLE_MAP", L"../Resource/BATTLE/BATTLE_MAP.png"))
    {
        std::cout << "[ERROR] BATTLE_MAP 텍스처 로드 실패" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("cabin", L"../Resource/BATTLE/cabin.png"))
    {
        std::cout << "[ERROR] cabin 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("fishing", L"../Resource/BATTLE/fishing.png"))
    {
        std::cout << "[ERROR] fishing 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("lab", L"../Resource/BATTLE/lab.png"))
    {
        std::cout << "[ERROR] lab 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("lab", L"../Resource/BATTLE/lumbermill.png"))
    {
        std::cout << "[ERROR] lumbermill 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("lab", L"../Resource/BATTLE/mine.png"))
    {
        std::cout << "[ERROR] mine 텍스처 로드 실패" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("MON_1", L"../Resource/BATTLE/MON_1.png"))
    {
        std::cout << "[ERROR] MON_1 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("MON_2", L"../Resource/BATTLE/MON_2.png"))
    {
        std::cout << "[ERROR] MON_2 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("MON_3", L"../Resource/BATTLE/MON_3.png"))
    {
        std::cout << "[ERROR] MON_3 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("MON_4", L"../Resource/BATTLE/MON_4.png"))
    {
        std::cout << "[ERROR] MON_4 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("MON_5", L"../Resource/BATTLE/MON_5.png"))
    {
        std::cout << "[ERROR] MON_5 텍스처 로드 실패" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("PLAY", L"../Resource/BATTLE/PLAY.png"))
    {
        std::cout << "[ERROR] PLAY 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("FAST", L"../Resource/BATTLE/FAST.png"))
    {
        std::cout << "[ERROR] FAST 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("STOP", L"../Resource/BATTLE/STOP.png"))
    {
        std::cout << "[ERROR] STOP 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("SETTING_ICON", L"../Resource/BATTLE/SETTING_ICON.png"))
    {
        std::cout << "[ERROR] SETTING_ICON 텍스처 로드 실패" << std::endl;
    }
    // Patikle Resource
    ////////////////////////////////////////////////////////////////////////////////
    if (!AssetManager::Instance().LoadTexture("blossom", L"../Resource/Patikle/ART_Flower01.png"))
    {
        std::cout << "[ERROR] SETTING_ICON 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("dust", L"../Resource/Dust.png"))
    {
        std::cout << "[ERROR] SETTING_ICON 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("dust2", L"../Resource/Dust2.png"))
    {
        std::cout << "[ERROR] SETTING_ICON 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("mouse", L"../Resource/Mouse.png"))
    {
        std::cout << "[ERROR] SETTING_ICON 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("spakle", L"../Resource/Spakle.png"))
    {
        std::cout << "[ERROR] SETTING_ICON 텍스처 로드 실패" << std::endl;
    }
    // TEST Resource
    ////////////////////////////////////////////////////////////////////////////////
    if (!AssetManager::Instance().LoadTexture("house", L"../Resource/house.png"))
    {
        std::cout << "[ERROR] house 텍스처 로드 실패" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("house2", L"../Resource/house2.png"))
    {
        std::cout << "[ERROR] house2 텍스처 로드 실패" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("Menu", L"../Resource/Menu.png"))
    {
        std::cout << "[ERROR] Menu 텍스처 로드 실패" << std::endl;
    }


    // TEST Resource
    ////////////////////////////////////////////////////////////////////////////////
    if (!AssetManager::Instance().LoadTexture("house", L"../Resource/house.png"))
    {
        std::cout << "[ERROR] house 텍스처 로드 실패" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("house2", L"../Resource/house2.png"))
    {
        std::cout << "[ERROR] house2 텍스처 로드 실패" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("Menu", L"../Resource/Menu.png"))
    {
        std::cout << "[ERROR] Menu 텍스처 로드 실패" << std::endl;
    }

    ////////////////////////////////////////////////////////////////////////////////
    if (!AssetManager::Instance().LoadTexture("GrayBird", L"../Resource/Animation/graybirdsheet.png"))
    {
        std::cout << "[ERROR] GrayBird 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadAnimationRender("GrayBird", L"../Resource/Animation/graybirdsheet.json"))
    {
        std::cout << "[ERROR] 애니메이션 로드 실패!" << std::endl;
    }
}

