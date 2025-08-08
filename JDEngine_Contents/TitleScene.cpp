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

        CreateTitleScene();     // 타이틀 씬 생성
        InitSound();            // 사운드 초기화
        InitParticle();         // 파티클 초기화
      
        ////////////////////////////////////////////////////////////////////////////////

        //// 1. 초기값 설정: 현재 오디오 매니저의 SFX 볼륨 값으로 설정합니다.
        //sfxSlider->SetValue(AudioManager::Instance().GetSFXVolume());

        //// 2. OnValueChanged: 슬라이더 값이 바뀔 때마다 SFX 볼륨을 조절하도록 연결합니다.
        //sfxSlider->AddOnValueChanged("Set SFX Volume", [](float newValue) {
        //    AudioManager::Instance().SetSFXVolume(newValue);
        //    });
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
        /////////////////////////////////////////////////////////////////////////////
        AudioManager::Instance().PlayBGM("BGM_Title", &bgmChannel);
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

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////

    void TitleScene::CreateTitleScene()
    {

        ShowCursor(FALSE);
        // UI
        ////////////////////////////////////////////////////////////////////////////////

        //타이틀 배경
        //auto image = CreateUIObject<Image>(L"Title_Image");
        auto image = CreateUIObject<Image>(L"Title");
        image->SetTextureName("Title");

        auto cam = D2DRenderer::Instance().GetCamera();
        if (cam)
        {
            float screenWidth = static_cast<float>(cam->GetScreenWidth());
            float screenHeight = static_cast<float>(cam->GetScreenHeight());

            // 화면 크기로 설정
            image->SetSize(Vector2F{ screenWidth, screenHeight });
            image->SetPosition({ 0.0f,0.0f });
            image->SetPivot({ 0.5f, 0.5f });
        }

        ////////////////////////////////////////////////////////////////////////////////

        // GameStart 버튼
        auto gameStart = CreateUIObject<Button>(L"GameStart_Button");
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
        gameStart->AddOnEnter("Highlight On", [this, gameStart]()
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
        gameStart->AddOnExit("Highlight Off", [this, gameStart]()
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
        auto setting = CreateUIObject<Button>(L"Setting_Button");
        setting->SetTextureName("SETTING_B");
        setting->SetText(L"");
        setting->SetSize({ 360, 58 });
        setting->SetPosition({ -522, -115 });

        // 1. OnClick: 클릭하면 실행될 이벤트
        setting->AddOnClick("OpenSettingUI", [this, setting]()
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
        setting->AddOnEnter("Highlight On", [this, setting]()
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
        setting->AddOnExit("Highlight Off", [this, setting]()
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
        auto quitGame = CreateUIObject<Button>(L"QuitGame_Button");
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
        quitGame->AddOnEnter("Highlight On", [this, quitGame]()
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
        quitGame->AddOnExit("Highlight Off", [this, quitGame]()
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

    void TitleScene::FinalizeTitleScene()
    {
        
    }

    void TitleScene::OnLeave() {
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

    void TitleScene::InitParticle()
    {
        //// 1. 초기값 설정: 현재 오디오 매니저의 SFX 볼륨 값으로 설정합니다.
        //sfxSlider->SetValue(AudioManager::Instance().GetSFXVolume());

        //// 2. OnValueChanged: 슬라이더 값이 바뀔 때마다 SFX 볼륨을 조절하도록 연결합니다.
        //sfxSlider->AddOnValueChanged("Set SFX Volume", [](float newValue) {
        //    AudioManager::Instance().SetSFXVolume(newValue);
        //    });
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
}