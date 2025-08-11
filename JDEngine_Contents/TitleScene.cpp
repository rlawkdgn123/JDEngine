#include "pch.h"
#include "framework.h"
#include "SceneList.h"
#include "TitleScene.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "SceneManager.h"

// IMGUI
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"


using namespace std;
using namespace JDGameObject::Content;

namespace JDScene {

    using namespace JDGameObject;
    using namespace JDComponent;
    using Vector2F = JDGlobal::Math::Vector2F;

    // TitleScene
    void TitleScene::OnEnter() {
        this->cam = D2DRenderer::Instance().GetCamera(); 
        CreateTitleScene();     // 타이틀 씬 생성
        InitSound();            // 사운드 초기화
        InitParticle();         // 파티클 초기화

        CaptureBaseUILayout(); // 기준 해상도의 좌표/크기 스냅샷
    }

    void TitleScene::OnLeave() {

        FinalizeTitleScene();   // 씬 정리
    }

    void TitleScene::Update(float deltaTime) {

        SceneBase::Update(deltaTime);

        ParticleUpdate(deltaTime);      // 파티클 업데이트
        ClickUpdate();                  // 클릭 업데이트
    }

    void TitleScene::FixedUpdate(float fixedDeltaTime) {

        SceneBase::FixedUpdate(fixedDeltaTime);
    }

    void TitleScene::LateUpdate(float deltaTime) {

        SceneBase::LateUpdate(deltaTime);
    }

    void TitleScene::Render(float deltaTime) {

        RenderTitleScene(deltaTime);    // 타이틀 렌더
        RenderParticle();               // 파티클 렌더
    }

    void TitleScene::OnResize(int width, int height)
    {
        
        if (cam) cam->SetScreenSize((float)width, (float)height);
        if (auto rc = D2DRenderer::Instance().GetCamera())
            rc->SetScreenSize((float)width, (float)height);

        ApplyUILayout(width, height);
    }

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////

    void TitleScene::CreateTitleScene()
    {

        ShowCursor(FALSE);
        // UI
        ////////////////////////////////////////////////////////////////////////////////
        this->cam = D2DRenderer::Instance().GetCamera();

        float screenWidth = static_cast<float>(this->cam->GetScreenWidth());
        float screenHeight = static_cast<float>(this->cam->GetScreenHeight());

        // 숨은 배경
        hidden_Cat = CreateUIObject<Image>(L"Hidden_Cat");
        hidden_Cat->SetTextureName("Hidden_Cat");
        hidden_Cat->SetSize(Vector2F{ 500, screenHeight });
        hidden_Cat->SetPosition({ 0.0f,0.0f });

        //타이틀 배경
        m_titleUI = CreateUIObject<Image>(L"Title");
        m_titleUI->SetTextureName("Title");

        
        if (this->cam)
        {
            // 화면 크기로 설정
            m_titleUI->SetSize(Vector2F{ screenWidth, screenHeight });
            m_titleUI->SetPosition({ 0.0f,0.0f });
            m_titleUI->SetPivot({ 0.5f, 0.5f });
        }

        ////////////////////////////////////////////////////////////////////////////////
        // GameStart 버튼
        this->gameStart = CreateUIObject<Button>(L"GameStart_Button");
        gameStart->SetTextureName("GAME_START_B");
        gameStart->SetText(L"");
        gameStart->SetSize({ 360, 58 });
        gameStart->SetPosition({ -522, -32 });

        // 1. OnClick: 클릭하면 실행될 이벤트
        gameStart->AddOnClick("Load GameScene", [this]()
            {
                if (isOpenOption) return;
                
                // SceneManager를 이용해 다음 씬으로 넘어갑니다.
                SceneManager::Instance().ChangeScene("SelectNationScene");
                AudioManager::Instance().PlaySFX("SFX_Button_Click", &sfxChannel);

            });

        // 2. OnEnter: 마우스를 올리면 텍스처 변경
        gameStart->AddOnEnter("Highlight On", [this]()
            {
                if (isOpenOption) return;

                gameStart->SetTextureName("GAME_START_A");
                if (sfxChannel == nullptr)
                {
                    // 채널 포인터를 TitleScene의 멤버 변수에 저장
                    AudioManager::Instance().PlaySFX("SFX_Button_Hover", &sfxChannel);
                }
            });

        // 3. OnExit: 마우스가 벗어나면 원래 텍스처로 복원
        gameStart->AddOnExit("Highlight Off", [this]()
            {
                if (isOpenOption) return;

                gameStart->SetTextureName("GAME_START_B");
                if (sfxChannel)
                {
                    sfxChannel = nullptr;      // 포인터를 다시 nullptr로 초기화 (중요!)
                }
            });


        ////////////////////////////////////////////////////////////////////////////////

        // Setting 버튼
        this->setting = CreateUIObject<Button>(L"Setting_Button");
        setting->SetTextureName("SETTING_B");
        setting->SetText(L"");
        setting->SetSize({ 360, 58 });
        setting->SetPosition({ -522, -115 });

        // 1. OnClick: 클릭하면 실행될 이벤트
        setting->AddOnClick("OpenSettingUI", [this]()
            {
                if (isOpenOption) return;

                isOpenOption = true;

                if (m_optionUI) {
                    m_optionUI->SetActive(true);
                    m_optionVolume->SetActive(true);

                    m_selectVolume->SetActive(true);
                    m_selectControl->SetActive(true);
                    m_selectCredit->SetActive(true);

                    m_closeOption->SetActive(true);

                    m_selectVolumeDummy->SetActive(true);
                    m_selectControlDummy->SetActive(true);
                    m_selectCreditDummy->SetActive(true);

                    m_masterSlider->SetActiveSlider(true);
                    m_bgmSlider->SetActiveSlider(true);
                    m_sfxSlider->SetActiveSlider(true);
                }

                // 마우스 벗어남 이벤트 수동 실행
                setting->SetTextureName("SETTING_B");
                AudioManager::Instance().PlaySFX("SFX_Button_Click", &sfxChannel);
            });

        // 2. OnEnter: 마우스를 올리면 텍스처 변경
        setting->AddOnEnter("Highlight On", [this]()
            {
                if (isOpenOption) return;

                setting->SetTextureName("SETTING_A");
                if (sfxChannel == nullptr)
                {
                    // 채널 포인터를 TitleScene의 멤버 변수에 저장
                    AudioManager::Instance().PlaySFX("SFX_Button_Hover", &sfxChannel);
                }
            });

        // 3. OnExit: 마우스가 벗어나면 원래 텍스처로 복원
        setting->AddOnExit("Highlight Off", [this]()
            {
                if (isOpenOption) return;

                setting->SetTextureName("SETTING_B");
                if (sfxChannel)
                {
                    sfxChannel = nullptr;      // 포인터를 다시 nullptr로 초기화 (중요!)
                }
            });

        //////////////////////////////////////////////////////////////////////////////////

        // Quit Game 버튼
        this->quitGame = CreateUIObject<Button>(L"QuitGame_Button");
        quitGame->SetTextureName("QUIT_GAME_B");
        quitGame->SetText(L"");
        quitGame->SetSize({ 360, 58 });
        quitGame->SetPosition({ -522, -198 });

        // 1. OnClick: 클릭하면 실행될 이벤트
        quitGame->AddOnClick("Quit Game", [this]()
            {
                if (isOpenOption) return;
                AudioManager::Instance().PlaySFX("SFX_Button_Click", &sfxChannel);
                // 1. 메인 윈도우의 핸들(HWND)을 가져옵니다.
                HWND mainWindowHandle = JDGlobal::Window::WindowSize::Instance().GetHWND();

                // 2. PostMessage를 사용해 WM_CLOSE 메시지를 보냅니다.
                if (mainWindowHandle)
                {
                    PostMessage(mainWindowHandle, WM_CLOSE, 0, 0);
                }
            });

        // 2. OnEnter: 마우스를 올리면 텍스처 변경
        quitGame->AddOnEnter("Highlight On", [this]()
            {
                if (isOpenOption) return;

                quitGame->SetTextureName("QUIT_GAME_A");
                if (sfxChannel == nullptr)
                {
                    // 채널 포인터를 TitleScene의 멤버 변수에 저장
                    AudioManager::Instance().PlaySFX("SFX_Button_Hover", &sfxChannel);
                }
            });

        // 3. OnExit: 마우스가 벗어나면 원래 텍스처로 복원
        quitGame->AddOnExit("Highlight Off", [this]()
            {
                if (isOpenOption) return;

                quitGame->SetTextureName("QUIT_GAME_B");
                if (sfxChannel)
                {
                    sfxChannel = nullptr;      // 포인터를 다시 nullptr로 초기화 (중요!)
                }
            });



        //////////////////////////////////////////////////////////////////////////////////

        // 옵션창
        m_optionUI = CreateUIObject<Image>(L"Option_Popup");
        m_optionUI->SetTextureName("BACK_GROUND_OPACITY");
        m_optionUI->SetColor(D2D1::ColorF(D2D1::ColorF::White, 0.65f));
        m_optionUI->SetActive(false);

        if (cam)
        {
            float screenWidth = static_cast<float>(cam->GetScreenWidth());
            float screenHeight = static_cast<float>(cam->GetScreenHeight());

            // 화면 크기로 설정
            m_optionUI->SetSize(Vector2F{ screenWidth, screenHeight });
            m_optionUI->SetPosition({ 0.0f,0.0f });
            m_optionUI->SetPivot({ 0.5f, 0.5f });
        }

        //////////////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////////////
        // 볼륨 조절 창
        //////////////////////////////////////////////////////////////////////////////////
        m_optionVolume = CreateUIObject<Image>(L"Option_Volume");
        m_optionVolume->SetTextureName("OPTION_1");
        m_optionVolume->SetActive(false);
        m_optionVolume->SetSize({ 1920, 1080 });
        m_optionVolume->SetPosition({ 0.0f,0.0f });
        m_optionVolume->SetPivot({ 0.5f, 0.5f });

        //////////////////////////////////////////////////////////////////////////////////
        // BGM 볼륨 조절 슬라이더
        //////////////////////////////////////////////////////////////////////////////////

        // 마스터 사운드
        m_masterSlider = CreateUIObject<Slider>(L"Slider_MasterVolume");
        m_masterSlider->Assemble(this); // 씬의 도움을 받아 슬라이더 자식들을 조립합니다.

        m_masterSlider->SetBackgroundImage("VOLUME_LINE_1");
        m_masterSlider->SetFillImage("VOLUME_LINE_2");
        m_masterSlider->SetHandleImage("VOLUME_CAT_1");

        m_masterSlider->SetSize({ 600, 8 });
        m_masterSlider->SetRootSize({ 600, 60 });
        m_masterSlider->SetPosition({ 150, 170 });
        m_masterSlider->SetFillImagePosition({ -300, 0 });

        m_masterSlider->SetHandleImageSize({ 57.f, 52.f });

        // TODO : 마스터 볼륨으로 받아야함
        //m_masterSlider->SetValue(AudioManager::Instance().GetMusicVolume());
        //m_masterSlider->AddOnValueChanged("Set BGM Volume", [](float newValue) {
        //    AudioManager::Instance().SetMusicVolume(newValue);
        //    });

        m_masterSlider->SetActiveSlider(false);

        // 배경음
        m_bgmSlider = CreateUIObject<Slider>(L"Slider_BGM");
        m_bgmSlider->Assemble(this); // 씬의 도움을 받아 슬라이더 자식들을 조립합니다.

        m_bgmSlider->SetBackgroundImage("VOLUME_LINE_1");
        m_bgmSlider->SetFillImage("VOLUME_LINE_2");
        m_bgmSlider->SetHandleImage("VOLUME_CAT_2");

        m_bgmSlider->SetSize({ 600, 8 });
        m_bgmSlider->SetRootSize({ 600, 60 });
        m_bgmSlider->SetPosition({ 150, 28 });
        m_bgmSlider->SetFillImagePosition({ -300, 0 });

        m_bgmSlider->SetHandleImageSize({ 39.5f, 35.f });

        m_bgmSlider->SetValue(AudioManager::Instance().GetMusicVolume());
        m_bgmSlider->AddOnValueChanged("Set BGM Volume", [](float newValue) {
            AudioManager::Instance().SetMusicVolume(newValue);
            });

        m_bgmSlider->SetActiveSlider(false);

        //////////////////////////////////////////////////////////////////////////////////

        // 효과음
        m_sfxSlider = CreateUIObject<Slider>(L"Slider_SFX");
        m_sfxSlider->Assemble(this); // 씬의 도움을 받아 슬라이더 자식들을 조립합니다.

        m_sfxSlider->SetBackgroundImage("VOLUME_LINE_1");
        m_sfxSlider->SetFillImage("VOLUME_LINE_2");
        m_sfxSlider->SetHandleImage("VOLUME_CAT_2");

        m_sfxSlider->SetSize({ 600, 8 });
        m_sfxSlider->SetRootSize({ 600, 60 });
        m_sfxSlider->SetPosition({ 150, -42 });
        m_sfxSlider->SetFillImagePosition({ -300, 0 });

        m_sfxSlider->SetHandleImageSize({ 39.5f, 35.f });

        m_sfxSlider->SetValue(AudioManager::Instance().GetSFXVolume());
        m_sfxSlider->AddOnValueChanged("Set SFX Volume", [](float newValue) {
            AudioManager::Instance().SetSFXVolume(newValue);
            });

        m_sfxSlider->SetActiveSlider(false);

        //////////////////////////////////////////////////////////////////////////////////

        // 컨트롤 조절 창
        m_optionControl = CreateUIObject<Image>(L"Option_Control");
        m_optionControl->SetTextureName("OPTION_2");
        m_optionControl->SetActive(false);
        m_optionControl->SetSize({ 1920, 1080 });
        m_optionControl->SetPosition({ 0.0f, 0.0f });
        m_optionControl->SetPivot({ 0.5f, 0.5f });

        //////////////////////////////////////////////////////////////////////////////////

        // 크레딧 보기 창
        m_optionCredit = CreateUIObject<Image>(L"Option_Credit");
        m_optionCredit->SetTextureName("OPTION_3");
        m_optionCredit->SetActive(false);
        m_optionCredit->SetSize({ 1920, 1080 });
        m_optionCredit->SetPosition({ 0.0f,0.0f });
        m_optionCredit->SetPivot({ 0.5f, 0.5f });

        //////////////////////////////////////////////////////////////////////////////////

        // 옵션 선택 버튼 ( 볼륨 )
        m_selectVolumeDummy = CreateUIObject<Image>(L"Volume_Button_Dummy");
        m_selectVolumeDummy->SetTextureName("VOLUME_BUTTON");
        m_selectVolumeDummy->SetSize({ 115.5f, 23.f });
        m_selectVolumeDummy->SetPosition({ -497.4f, 244.1f });
        m_selectVolumeDummy->SetActive(false);

        m_selectVolume = CreateUIObject<Button>(L"SelectVolume_Button");
        m_selectVolume->SetTextureName("Test");
        m_selectVolume->SetTextureColor(D2D1::ColorF(D2D1::ColorF::White, 0.f));
        m_selectVolume->SetText(L"");
        m_selectVolume->SetSize({ 180, 70 });
        m_selectVolume->SetPosition({ -497.4f, 244.1f });
        m_selectVolume->SetActive(false);

        m_selectVolume->AddOnClick("OpenVolumeUI", [this]() {
            AudioManager::Instance().PlaySFX("SFX_Button_Click", &sfxChannel);
            m_optionVolume->SetActive(true);
            m_optionControl->SetActive(false);
            m_optionCredit->SetActive(false);

            m_selectVolumeDummy->SetActive(false);
            m_selectControlDummy->SetActive(true);
            m_selectCreditDummy->SetActive(true);

            m_bgmSlider->SetActiveSlider(true);
            m_sfxSlider->SetActiveSlider(true);
            });

        // 옵션 선택 버튼 ( 컨트롤 )
        m_selectControlDummy = CreateUIObject<Image>(L"Control_Button_Dummy");
        m_selectControlDummy->SetTextureName("CONTROLS_BUTTON");
        m_selectControlDummy->SetSize({ 115.5f, 23.f });
        m_selectControlDummy->SetPosition({ -497.4f, 172.7f });
        m_selectControlDummy->SetActive(false);

        m_selectControl = CreateUIObject<Button>(L"SelectControl_Button");
        m_selectControl->SetTextureName("Test");
        m_selectControl->SetTextureColor(D2D1::ColorF(D2D1::ColorF::White, 0.f));
        m_selectControl->SetText(L"");
        m_selectControl->SetSize({ 180, 70 });
        m_selectControl->SetPosition({ -497.4f, 172.7f });
        m_selectControl->SetActive(false);

        m_selectControl->AddOnClick("OpenControlUI", [this]() {
            AudioManager::Instance().PlaySFX("SFX_Button_Click", &sfxChannel);
            m_optionVolume->SetActive(false);
            m_optionControl->SetActive(true);
            m_optionCredit->SetActive(false);

            m_selectVolumeDummy->SetActive(true);
            m_selectControlDummy->SetActive(false);
            m_selectCreditDummy->SetActive(true);

            m_bgmSlider->SetActiveSlider(false);
            m_sfxSlider->SetActiveSlider(false);
            });

        // 옵션 선택 버튼 ( 크레딧 )
        m_selectCreditDummy = CreateUIObject<Image>(L"Credit_Button_Dummy");
        m_selectCreditDummy->SetTextureName("CREDITS_BUTTON");
        m_selectCreditDummy->SetSize({ 115.5f, 23.f });
        m_selectCreditDummy->SetPosition({ -497.4f, 101.6f });
        m_selectCreditDummy->SetActive(false);

        m_selectCredit = CreateUIObject<Button>(L"SelectCredit_Button");
        m_selectCredit->SetTextureName("Test");
        m_selectCredit->SetTextureColor(D2D1::ColorF(D2D1::ColorF::White, 0.f));
        m_selectCredit->SetText(L"");
        m_selectCredit->SetSize({ 180, 70 });
        m_selectCredit->SetPosition({ -497.4f, 101.6f });
        m_selectCredit->SetActive(false);

        m_selectCredit->AddOnClick("OpenCreditUI", [this]() {
            AudioManager::Instance().PlaySFX("SFX_Button_Click", &sfxChannel);
            m_optionVolume->SetActive(false);
            m_optionControl->SetActive(false);
            m_optionCredit->SetActive(true);

            m_selectVolumeDummy->SetActive(true);
            m_selectControlDummy->SetActive(true);
            m_selectCreditDummy->SetActive(false);

            m_bgmSlider->SetActiveSlider(false);
            m_sfxSlider->SetActiveSlider(false);
            });

        //////////////////////////////////////////////////////////////////////////////////

        // 옵션 닫기 버튼 ( Close )
        m_closeOption = CreateUIObject<Button>(L"CloseSetting_Button");
        m_closeOption->SetTextureName("ART_Back01_mouseover");
        m_closeOption->SetText(L"");
        m_closeOption->SetSize({ 66, 60 });
        m_closeOption->SetPosition({ -550, 340 });
        m_closeOption->SetActive(false);

        // 1. OnClick: 클릭하면 실행될 이벤트
        m_closeOption->AddOnClick("Load TitleScene", [this]() {
            AudioManager::Instance().PlaySFX("SFX_Button_Click", &sfxChannel);
            m_optionUI->SetActive(false);
            m_optionVolume->SetActive(false);
            m_optionControl->SetActive(false);
            m_optionCredit->SetActive(false);

            m_selectVolume->SetActive(false);
            m_selectControl->SetActive(false);
            m_selectCredit->SetActive(false);

            m_closeOption->SetActive(false);

            m_selectVolumeDummy->SetActive(false);
            m_selectControlDummy->SetActive(false);
            m_selectCreditDummy->SetActive(false);

            m_masterSlider->SetActiveSlider(false);
            m_bgmSlider->SetActiveSlider(false);
            m_sfxSlider->SetActiveSlider(false);

            isOpenOption = false;
            });

        // 2. OnEnter: 마우스를 올리면 텍스처 변경
        m_closeOption->AddOnEnter("Highlight On", [this]() {
            // 텍스트 변경
            m_closeOption->SetTextureName("BACK_2");
            });

        // 3. OnExit: 마우스가 벗어나면 원래 텍스처로 복원
        m_closeOption->AddOnExit("Highlight Off", [this]() {
            // 텍스트 변경
            m_closeOption->SetTextureName("BACK_1");
            });

        //////////////////////////////////////////////////////////////////////////////////

        //// BGM 볼륨 조절 슬라이더
        //auto bgmSlider = CreateUIObject<Slider>(L"BGM_Slider");
        //bgmSlider->Assemble(this); // 씬의 도움을 받아 슬라이더 자식들을 조립합니다.

        //// 이미지 개별 설정
        //bgmSlider->SetBackgroundImage("VOLUME_LINE_2");
        //bgmSlider->SetFillImage("VOLUME_LINE_1");
        //bgmSlider->SetHandleImage("VOLUME_CAT_1");

        //// 1. 초기값 설정: 현재 오디오 매니저의 BGM 볼륨 값으로 설정합니다.
        //bgmSlider->SetValue(AudioManager::Instance().GetMusicVolume());

        //// 2. OnValueChanged: 슬라이더 값이 바뀔 때마다 BGM 볼륨을 조절하도록 연결합니다.
        //bgmSlider->AddOnValueChanged("Set BGM Volume", [](float newValue) {
        //    AudioManager::Instance().SetMusicVolume(newValue);
        //    });

        //////////////////////////////////////////////////////////////////////////////////

        //// SFX 볼륨 조절 슬라이더
        //auto sfxSlider = CreateUIObject<Slider>(L"SFX_Slider");
        //sfxSlider->Assemble(this);
        //sfxSlider->SetPosition({ -522, 320 }); // BGM 슬라이더 아래

        //// 이미지 개별 설정
        //bgmSlider->SetBackgroundImage("VOLUME_LINE_1");
        //bgmSlider->SetFillImage("VOLUME_LINE_2");
        //bgmSlider->SetHandleImage("VOLUME_CAT_2");
    }

    void TitleScene::CaptureBaseUILayout()
    {
        using V2 = JDGlobal::Math::Vector2F;

        //숨은배경
        base_hiddenCat = { V2{ 1800,0 }, V2{ 1080,1080 } };

        // ── 풀스크린 이미지들(생성 시 1920x1080 기준으로 맞춰놨다고 가정) ──
        base_titleUI = { V2{ 0,0 }, V2{ 1920,1080 } };
        base_optionUI = { V2{ 0,0 }, V2{ 1920,1080 } };
        base_optionVolume = { V2{ 0,0 }, V2{ 1920,1080 } };
        base_optionControl = { V2{ 0,0 }, V2{ 1920,1080 } };
        base_optionCredit = { V2{ 0,0 }, V2{ 1920,1080 } };

        // ── 버튼들 ──
        // ── 메인 메뉴 버튼 3종 ── 
        base_gameStartBtn = { V2{ -522.f,  -32.f }, V2{ 360.f, 58.f } };
        base_settingBtn = { V2{ -522.f, -115.f }, V2{ 360.f, 58.f } };
        base_quitBtn = { V2{ -522.f, -198.f }, V2{ 360.f, 58.f } };
        
        base_closeOption = { V2{ -550, 340 },  V2{ 66, 60 } };

        base_selectVolumeBtn = { V2{ -497.4f, 244.1f }, V2{ 180, 70 } };
        base_selectControlBtn = { V2{ -497.4f, 172.7f }, V2{ 180, 70 } };
        base_selectCreditBtn = { V2{ -497.4f, 101.6f }, V2{ 180, 70 } };

        base_selectVolumeDummy = { V2{ -497.4f, 244.1f }, V2{ 115.5f, 23.f } };
        base_selectControlDummy = { V2{ -497.4f, 172.7f }, V2{ 115.5f, 23.f } };
        base_selectCreditDummy = { V2{ -497.4f, 101.6f }, V2{ 115.5f, 23.f } };

        // ── 슬라이더들 ──
        base_masterSlider.track.size = V2{ 600, 8 };
        base_masterSlider.track.pos = V2{ 150, 170 };
        base_masterSlider.rootSize = V2{ 600, 60 };
        base_masterSlider.fillOffset = V2{ -300, 0 };
        base_masterSlider.handleSize = V2{ 57.f, 52.f };

        base_bgmSlider.track.size = V2{ 600, 8 };
        base_bgmSlider.track.pos = V2{ 150, 28 };
        base_bgmSlider.rootSize = V2{ 600, 60 };
        base_bgmSlider.fillOffset = V2{ -300, 0 };
        base_bgmSlider.handleSize = V2{ 39.5f, 35.f };

        base_sfxSlider.track.size = V2{ 600, 8 };
        base_sfxSlider.track.pos = V2{ 150, -42 };
        base_sfxSlider.rootSize = V2{ 600, 60 };
        base_sfxSlider.fillOffset = V2{ -300, 0 };
        base_sfxSlider.handleSize = V2{ 39.5f, 35.f };

        // ── 텍스트(폰트 크기 포함) ──
        base_stopText = { V2{ /*예시*/ -400, -300 }, /*fontSize*/ 24.f };
        base_playText = { V2{ /*예시*/ -400, -260 }, /*fontSize*/ 24.f };
        base_speedText = { V2{ /*예시*/ -400, -220 }, /*fontSize*/ 24.f };
        // 
    }

    void TitleScene::ApplyUILayout(int w, int h)
    {
        using V2 = JDGlobal::Math::Vector2F;

        const float sx = w / kBaseW;
        const float sy = h / kBaseH;
        const float s = (std::min)(sx, sy);   //  균일 스케일 (비율 고정)

        auto SPos = [&](const V2& p)->V2 { return V2{ p.x * s, p.y * s }; };     //  s만
        auto SSize = [&](const V2& sz)->V2 { return V2{ sz.x * s, sz.y * s }; };  //  s만

        // 비율 유지로 맞추는 Fit(여백 발생 가능)
        auto FitScreen = [&](Image* img) {
            if (!img) return;
            img->SetSize({ kBaseW * s, kBaseH * s });  //  w,h로 늘리지 않음 (왜곡 방지)
            img->SetPosition({ 0.f, 0.f });            // 중앙정렬 안 원한다 했으니 (0,0)
            img->SetPivot({ 0.5f, 0.5f });
            };

        auto ApplyRect = [&](auto* ui, const Rect& r) {
            if (!ui) return;
            ui->SetSize(SSize(r.size));       // s만 적용
            ui->SetPosition(SPos(r.pos));     // s만 적용
            ui->SetPivot({ 0.5f, 0.5f });
            };

        // --- 콘텐츠/바 크기 계산 ---
        const float contentW = kBaseW * s;
        const float contentH = kBaseH * s;
        const float barRight = w - contentW;   // >0이면 오른쪽에 여백(필러박스)
        const float barLeft = 0.0f;

        // --- 숨은 배경: 오른쪽 필러박스 영역을 채움 ---
        if (hidden_Cat)
        {
            // 필러박스가 있을 때만 보이도록
            float fillW = (std::max)(0.0f, barRight);
            float fillH = contentH;            // 콘텐츠 높이만큼

            // 이미지 원본(기준값)에 s만 적용 – 크기는 '고정'
            const float imgW = base_hiddenCat.size.x * s;
            const float imgH = base_hiddenCat.size.y * s;
            // 중심 좌표계(0,0이 화면 중앙) 기준:
            // 콘텐츠의 오른쪽 끝은 +contentW/2
            // 바 영역의 가운데는 +contentW/2 + fillW/2
            const float posX = contentW * 0.5f + imgW * 0.5f; // 피벗 0.5 가정
            const float posY = 0.0f;               // 수직 중앙 정렬

            hidden_Cat->SetSize({ imgW, imgH  });   // 바 영역을 그대로 채움(늘림)
            hidden_Cat->SetPosition({ posX, posY });
            hidden_Cat->SetPivot({ 0.5f, 0.5f });
        }

        // 1) 풀스크린 이미지(비율 유지)
        FitScreen(m_titleUI);
        FitScreen(m_optionUI);
        FitScreen(m_optionVolume);
        FitScreen(m_optionControl);
        FitScreen(m_optionCredit);

        // 2) 메인 메뉴 버튼 3종
        ApplyRect(gameStart, base_gameStartBtn);
        ApplyRect(setting, base_settingBtn);
        ApplyRect(quitGame, base_quitBtn);

        // 3) 옵션창 버튼/더미
        ApplyRect(m_closeOption, base_closeOption);
        ApplyRect(m_selectVolume, base_selectVolumeBtn);
        ApplyRect(m_selectControl, base_selectControlBtn);
        ApplyRect(m_selectCredit, base_selectCreditBtn);
        ApplyRect(m_selectVolumeDummy, base_selectVolumeDummy);
        ApplyRect(m_selectControlDummy, base_selectControlDummy);
        ApplyRect(m_selectCreditDummy, base_selectCreditDummy);

        // 4) 슬라이더: offset은 "로컬 오프셋"이면 위치 변환이 아닌 '스케일만' 적용하는 게 안전
        auto SOnly = [&](const V2& v)->V2 { return V2{ v.x * s, v.y * s }; };
        auto ApplySlider = [&](Slider* sl, const SliderLayout& b) {
            if (!sl) return;
            sl->SetSize(SSize(b.track.size));
            sl->SetRootSize(SSize(b.rootSize));
            sl->SetPosition(SPos(b.track.pos));
            sl->SetFillImagePosition(SOnly(b.fillOffset));   // 위치가 아니라 로컬 오프셋이므로 s만
            sl->SetHandleImageSize(SSize(b.handleSize));
            };
        ApplySlider(m_masterSlider, base_masterSlider);
        ApplySlider(m_bgmSlider, base_bgmSlider);
        ApplySlider(m_sfxSlider, base_sfxSlider);

        // 5) 텍스트
        auto ApplyText = [&](Text* t, const TextLayout& b) {
            if (!t) return;
            t->SetPosition(SPos(b.pos));          // 위치는 s만
            float fs = b.fontSize * s;            // 폰트도 균일 스케일
            t->SetSize({ fs, fs });               // SetFontSize(fs)가 있으면 그걸 쓰는 게 더 좋음
            };
        ApplyText(m_stopKeyText, base_stopText);
        ApplyText(m_playKeyText, base_playText);
        ApplyText(m_speedKeyText, base_speedText);

        //파티클 생성위치 변경
        sakuraMin = SPos(base_sakuraMin);   // = base * s
        sakuraMax = SPos(base_sakuraMax);   // = base * s
    }


    void TitleScene::FinalizeTitleScene()
    {
        ShowCursor(TRUE);

        // 효과음 재생 중이면 정지
        if (sfxChannel)
        {
            //sfxChannel->stop();        // 사운드 정지
            sfxChannel = nullptr;      // 포인터를 다시 nullptr로 초기화 (중요!)
        }
        if (bgmChannel) {
            bgmChannel->stop(); // FMOD에서 채널을 멈춤
            bgmChannel = nullptr; // 포인터도 초기화 (안전)
        }

        // 선택된 오브젝트 포인터 초기화
        SetSelectedObject(nullptr);

        // 오브젝트 제거
        for (const auto& gameObject : m_gameObjects)
        {
            DestroyObject(gameObject.get());
        }
        for (const auto& uiObject : m_uiObjects)
        {
            DestroyObject(uiObject.get());
        }

        ////////////////////////////////////////////////////////////////////////////////

        // 옵션창
        isOpenOption = false;

        m_optionUI = nullptr;
        m_optionVolume = nullptr;
        m_optionControl = nullptr;
        m_optionCredit = nullptr;

        m_closeOption = nullptr;

        // 옵션 선택 실제 버튼
        m_selectVolume = nullptr;
        m_selectControl = nullptr;
        m_selectCredit = nullptr;

        // 옵션 선택 더미 이미지
        m_selectVolumeDummy = nullptr;
        m_selectControlDummy = nullptr;
        m_selectCreditDummy = nullptr;

        // 볼륨 선택 슬라이더
        m_masterSlider = nullptr;
        m_bgmSlider = nullptr;
        m_sfxSlider = nullptr;

        // 배속 키 텍스트
        m_stopKeyText = nullptr;
        m_playKeyText = nullptr;
        m_speedKeyText = nullptr;

        ////////////////////////////////////////////////////////////////////////////////
    }

    void TitleScene::InitSound()
    {
        //// 1. 초기값 설정: 현재 오디오 매니저의 SFX 볼륨 값으로 설정합니다.
        //sfxSlider->SetValue(AudioManager::Instance().GetSFXVolume());

        //// 2. OnValueChanged: 슬라이더 값이 바뀔 때마다 SFX 볼륨을 조절하도록 연결합니다.
        //sfxSlider->AddOnValueChanged("Set SFX Volume", [](float newValue) {
        //    AudioManager::Instance().SetSFXVolume(newValue);
        //    });

        AudioManager::Instance().PlayBGM("BGM_Title", &bgmChannel);
    }

    void TitleScene::InitParticle()
    {
        //파티클 초기화
        m_mouseParticles = std::make_unique<ParticleSystem>(
            D2DRenderer::Instance().GetD2DContext()
        );
        m_sakuraParticles = std::make_unique<ParticleSystem>(
            D2DRenderer::Instance().GetD2DContext()
        );
        m_dustParticles = std::make_unique<ParticleSystem>(
            D2DRenderer::Instance().GetD2DContext()
        );
        m_dust2Particles = std::make_unique<ParticleSystem>(
            D2DRenderer::Instance().GetD2DContext()
        );
        m_sparkleParticles = std::make_unique<ParticleSystem>(
            D2DRenderer::Instance().GetD2DContext()
        );
    }

    void TitleScene::ClickUpdate() {

        // ImGui가 마우스 입력을 사용 중이면 게임 내 클릭을 무시합니다.
        if (ImGui::GetIO().WantCaptureMouse)
            return;

        InputManager& input = InputManager::Instance();
        MouseState state = input.GetMouseState();

        // state.leftClicked 또는 state.leftPressed 등 필요한 입력 상태를 사용합니다.
        if (state.leftClicked)
        {
            // (1) 마우스 좌표를 맨 위에서 한 번만 계산해서 재사용합니다.
            // UI 클릭 판정에 사용할 스크린 좌표 (D2D 기준: Y 아래가 양수)
            Vector2F screenMousePos(
                static_cast<float>(state.pos.x),
                static_cast<float>(state.pos.y)
            );
            // 게임 오브젝트 클릭 판정에 사용할 월드 좌표 (Unity 기준: Y 위가 양수)
            Vector2F worldMousePos = GetMouseWorldPos(); // 우리가 만든 통일된 함수 사용!

            bool clicked = false;

            ////////////////////////////////////////////////////////////////////////////////
            // 1. UI 클릭 검사 (스크린 좌표계 사용)
            ////////////////////////////////////////////////////////////////////////////////
            // 이 로직은 스크린 좌표를 사용하므로 기존과 동일하게 올바르게 동작합니다.
            for (int i = static_cast<int>(m_uiObjects.size()) - 1; i >= 0; --i)
            {
                auto& uiObj = m_uiObjects[i];
                if (!uiObj || !uiObj->IsActive()) continue;

                auto clickable = uiObj->GetComponent<Editor_Clickable>();
                // UI의 IsHit 함수에는 '스크린 좌표'를 그대로 넘겨줍니다.
                if (clickable && clickable->IsHit(screenMousePos))
                {
                    SetSelectedObject(uiObj.get());
                    clicked = true;
                    std::cout << " UI 오브젝트 클릭 함!!!!! ";
                    break; // UI를 클릭했으면 더 이상 진행 안 함
                }
            }

            ////////////////////////////////////////////////////////////////////////////////
            // 2. 게임오브젝트 클릭 검사 (월드 좌표계 사용)
            ////////////////////////////////////////////////////////////////////////////////
            if (!clicked)
            {
                // (2) 불필요하고 잘못된 좌표 변환 로직을 모두 제거합니다.
                /* Vector2F unityMousePos(mousePos.x, screenHeight - mousePos.y);
                    Vector2F worldMousePos = camera->ScreenToWorldPoint(unityMousePos);
                    -> 이 부분은 GetMouseWorldPos()로 대체되었으므로 삭제!
                */

                for (int i = static_cast<int>(m_gameObjects.size()) - 1; i >= 0; --i)
                {
                    auto& obj = m_gameObjects[i];
                    if (!obj || !obj->IsActive()) continue;

                    auto clickable = obj->GetComponent<Editor_Clickable>();
                    // (3) 게임 오브젝트의 IsHit 함수에는 위에서 계산한 '월드 좌표'를 넘겨줍니다.
                    if (clickable && clickable->IsHit(worldMousePos))
                    {
                        SetSelectedObject(obj.get());
                        clicked = true;
                        std::cout << " 게임 오브젝트 클릭 함!!!!! ";
                        break;
                    }
                }
            }

            // 아무것도 클릭되지 않았다면 선택 해제
            if (!clicked)
            {
                SetSelectedObject(nullptr);
            }
        }
    }

    void TitleScene::ParticleUpdate(float deltaTime)
    {
        // 파티클
        float screenW = JDGlobal::Window::WindowSize::Instance().GetWidth();
        float screenH = JDGlobal::Window::WindowSize::Instance().GetHeight();
        Vector2F centerPos{ screenW * 0.5f, screenH * 0.5f };

        // 1) 마우스 파티클 업데이트 & Emit
        {
            MouseState ms = InputManager::Instance().GetMouseState();
            Vector2F mousePos{ (float)ms.pos.x, (float)ms.pos.y };

            m_mouseParticles->Update(deltaTime, mousePos);
            m_mouseParticles->Emit(
                mousePos,
                /*count=*/30,
                D2D1::ColorF(0.0f, 0.0f, 1.0f, 1.0f),
                /*scale=*/2.5f
            );
        }

        // 2) 벚꽃 낙하 파티클 스폰 & 업데이트
        {
            m_sakuraEmitAccumulator += deltaTime * m_sakuraEmissionRate;
            int spawnCount = static_cast<int>(m_sakuraEmitAccumulator);
            m_sakuraEmitAccumulator -= spawnCount;

            float scale = RandomFloat(1.0f, 2.0f);
            float maxLife = RandomFloat(6.0f, 12.0f);

            for (int i = 0; i < spawnCount; ++i) {
                float x = RandomFloat(sakuraMin.x, sakuraMax.x);
                float y = RandomFloat(sakuraMin.y, sakuraMax.y);
                Vector2F spawnPos{ x, y };
                m_sakuraParticles->SpawnFallingParticle(
                    spawnPos,
                    /*color=*/D2D1::ColorF(1.f, 0.8f, 0.9f, 1.f),
                    /*scale=*/scale,
                    /*maxLife=*/maxLife
                );
            }
            m_sakuraParticles->UpdateFalling(deltaTime, screenW, screenH);
        }

        // 3) 먼지 구름 파티클 스폰 & 업데이트
        {
            const float spawnRate = 12.f;
            static float emitAcc = 0.f;

            emitAcc += deltaTime * spawnRate;
            int cnt = static_cast<int>(emitAcc);
            emitAcc -= cnt;

            for (int i = 0; i < cnt; ++i) {
                m_dustParticles->SpawnDustParticle(
                    centerPos,
                    /*color*/      D2D1::ColorF(0.6f, 0.6f, 0.6f, 1.f),
                    /*scale*/      7.0f,
                    /*maxLife*/    2.0f,
                    /*spreadRadius*/20.f
                );
            }

            m_dustParticles->UpdateDust(deltaTime, screenW, screenH);
            for (int i = 0; i < cnt; ++i) {
                m_dust2Particles->SpawnDustParticle(
                    centerPos,
                    /*color*/      D2D1::ColorF(0.6f, 0.6f, 0.6f, 1.f),
                    /*scale*/      7.0f,
                    /*maxLife*/    1.0f,
                    /*spreadRadius*/20.f
                );
            }

            m_dust2Particles->UpdateDust(deltaTime, screenW, screenH);
        }

        // 4) 스파클 파티클 스폰 & 업데이트
        {
            // 화면을 벗어나지 않는 랜덤 위치 생성용
            auto randomPos = [&](void)->Vector2F {
                float x = RandomFloat(50.f, screenW / 2);
                float y = RandomFloat(100.f, screenH / 2);
                return { x, y };
                };

            const float sparkleRate = 1.f;
            static float sparkleAcc = 0.f;
            sparkleAcc += deltaTime * sparkleRate;
            int sparkleCount = static_cast<int>(sparkleAcc);
            sparkleAcc -= sparkleCount;

            for (int i = 0; i < sparkleCount; ++i) {
                Vector2F spawnPos = randomPos();
                m_sparkleParticles->SpawnSparkleParticle(
                    spawnPos,
                    /*count=*/1,
                    /*scale=*/10.0f,
                    /*radius=*/0.f,
                    /*maxLife=*/0.5f
                );
            }

            // UpdateSparkle 호출
            m_sparkleParticles->UpdateSparkle(deltaTime);
        }
    }

    void TitleScene::RenderTitleScene(float deltaTime)
    {
        auto camera = D2DRenderer::Instance().GetCamera();

        if (camera)
        {
            D2DRenderer::Instance().SetTransform(camera->GetViewMatrix());
        }
        else
        {
            D2DRenderer::Instance().SetTransform(D2D1::Matrix3x2F::Identity());
        }

        for (auto& obj : m_gameObjects)
        {
            D2DRenderer::Instance().RenderGameObject(*obj, deltaTime);
        }

        for (auto& uiObj : m_uiObjects)
        {
            D2DRenderer::Instance().RenderUIObject(*uiObj);
        }
    }

    void TitleScene::RenderParticle()
    {
        //파티클 그리기
        auto ctx = D2DRenderer::Instance().GetD2DContext();
        D2D1_MATRIX_3X2_F old;
        ctx->GetTransform(&old);
        ctx->SetTransform(D2D1::Matrix3x2F::Identity());

        if (m_mouseParticles) m_mouseParticles->RenderGlow(ctx);
        if (m_sakuraParticles) m_sakuraParticles->Render(ctx);
        if (m_dustParticles) m_dustParticles->RenderDust(ctx);
        if (m_dust2Particles) m_dust2Particles->RenderDust2(ctx);
        if (m_sparkleParticles) m_sparkleParticles->RenderSparkle(ctx);

        ctx->SetTransform(old);
    }
  
    //void TitleScene::ClickUpdate() {

    //    // ImGui가 마우스 입력을 사용 중이면 게임 내 클릭을 무시합니다.
    //    if (ImGui::GetIO().WantCaptureMouse)
    //        return;

    //    InputManager& input = InputManager::Instance();
    //    MouseState state = input.GetMouseState();
    //    // state.leftClicked 또는 state.leftPressed 등 필요한 입력 상태를 사용합니다.
    //    if (state.leftClicked)
    //    {
    //        // (1) 마우스 좌표를 맨 위에서 한 번만 계산해서 재사용합니다.
    //        // UI 클릭 판정에 사용할 스크린 좌표 (D2D 기준: Y 아래가 양수)
    //        Vector2F screenMousePos(
    //            static_cast<float>(state.pos.x),
    //            static_cast<float>(state.pos.y)
    //        );
    //        // 게임 오브젝트 클릭 판정에 사용할 월드 좌표 (Unity 기준: Y 위가 양수)
    //        Vector2F worldMousePos = GetMouseWorldPos(); // 우리가 만든 통일된 함수 사용!

    //        bool clicked = false;

    //        ////////////////////////////////////////////////////////////////////////////////
    //        // 1. UI 클릭 검사 (스크린 좌표계 사용)
    //        ////////////////////////////////////////////////////////////////////////////////
    //        // 이 로직은 스크린 좌표를 사용하므로 기존과 동일하게 올바르게 동작합니다.
    //        for (int i = static_cast<int>(m_uiObjects.size()) - 1; i >= 0; --i)
    //        {
    //            auto& uiObj = m_uiObjects[i];
    //            if (!uiObj || !uiObj->IsActive()) continue;

    //            auto clickable = uiObj->GetComponent<Editor_Clickable>();
    //            // UI의 IsHit 함수에는 '스크린 좌표'를 그대로 넘겨줍니다.
    //            if (clickable && clickable->IsHit(screenMousePos))
    //            {
    //                SetSelectedObject(uiObj.get());
    //                clicked = true;
    //                std::cout << " UI 오브젝트 클릭 함!!!!! ";
    //                break; // UI를 클릭했으면 더 이상 진행 안 함
    //            }
    //        }

    //        ////////////////////////////////////////////////////////////////////////////////
    //        // 2. 게임오브젝트 클릭 검사 (월드 좌표계 사용)
    //        ////////////////////////////////////////////////////////////////////////////////
    //        if (!clicked)
    //        {
    //            // (2) 불필요하고 잘못된 좌표 변환 로직을 모두 제거합니다.
    //            /* Vector2F unityMousePos(mousePos.x, screenHeight - mousePos.y);
    //                Vector2F worldMousePos = camera->ScreenToWorldPoint(unityMousePos);
    //                -> 이 부분은 GetMouseWorldPos()로 대체되었으므로 삭제!
    //            */

    //            for (int i = static_cast<int>(m_gameObjects.size()) - 1; i >= 0; --i)
    //            {
    //                auto& obj = m_gameObjects[i];
    //                if (!obj || !obj->IsActive()) continue;

    //                auto clickable = obj->GetComponent<Editor_Clickable>();
    //                // (3) 게임 오브젝트의 IsHit 함수에는 위에서 계산한 '월드 좌표'를 넘겨줍니다.
    //                if (clickable && clickable->IsHit(worldMousePos))
    //                {
    //                    SetSelectedObject(obj.get());
    //                    clicked = true;
    //                    std::cout << " 게임 오브젝트 클릭 함!!!!! ";
    //                    break;
    //                }
    //            }
    //        }

    //        // 아무것도 클릭되지 않았다면 선택 해제
    //        if (!clicked)
    //        {
    //            
    //            SetSelectedObject(nullptr);
    //        }
    //    }
    //}
}