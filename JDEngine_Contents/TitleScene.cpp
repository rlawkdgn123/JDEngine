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

        // UI
        ////////////////////////////////////////////////////////////////////////////////
        
        //타이틀 배경
        //auto image = CreateUIObject<Image>(L"Title_Image");
        auto image = CreateUIObject<Image>(L"Title");
        image->SetTextureName("Title");

        auto cam =  D2DRenderer::Instance().GetCamera();
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
        gameStart->SetSize({ 340, 58 });
        gameStart->SetPosition({ -522, 32 });

        // 1. OnClick: 클릭하면 실행될 이벤트
        gameStart->AddOnClick("Load GameScene", [this]() {
            // SceneManager를 이용해 다음 씬으로 넘어갑니다.
            SceneManager::Instance().ChangeScene("TutorialScene");
            // SceneManager::Instance().ChangeScene("SelectNationScene");
            });

        // 2. OnEnter: 마우스를 올리면 텍스처 변경
        gameStart->AddOnEnter("Highlight On", [this, gameStart]() {
            gameStart->SetTextureName("GAME_START_A");
            if (m_hoverSfxChannel == nullptr)
            {
                // 채널 포인터를 TitleScene의 멤버 변수에 저장
                AudioManager::Instance().PlaySFX("Step", &m_hoverSfxChannel);
            }
            });

        // 3. OnExit: 마우스가 벗어나면 원래 텍스처로 복원
        gameStart->AddOnExit("Highlight Off", [this, gameStart]() {
            gameStart->SetTextureName("GAME_START_B");
            if (m_hoverSfxChannel)
            {
                m_hoverSfxChannel->stop();        // 사운드 정지
                m_hoverSfxChannel = nullptr;      // 포인터를 다시 nullptr로 초기화 (중요!)
            }
            });


        ////////////////////////////////////////////////////////////////////////////////

        // Setting 버튼
        auto setting = CreateUIObject<Button>(L"Setting_Button");
        setting->SetTextureName("SETTING_B");
        setting->SetText(L"");
        setting->SetSize({ 340, 58 });
        setting->SetPosition({ -522, 115 });

        // 1. OnClick: 클릭하면 실행될 이벤트
        setting->AddOnClick("OpenSettingUI", []() {
            
            });

        // 2. OnEnter: 마우스를 올리면 텍스처 변경
        setting->AddOnEnter("Highlight On", [this, setting]() {
            setting->SetTextureName("SETTING_A");
            if (m_hoverSfxChannel == nullptr)
            {
                // 채널 포인터를 TitleScene의 멤버 변수에 저장
                AudioManager::Instance().PlaySFX("Step", &m_hoverSfxChannel);
            }
            });

        // 3. OnExit: 마우스가 벗어나면 원래 텍스처로 복원
        setting->AddOnExit("Highlight Off", [this, setting]() {
            setting->SetTextureName("SETTING_B");
            if (m_hoverSfxChannel)
            {
                m_hoverSfxChannel->stop();        // 사운드 정지
                m_hoverSfxChannel = nullptr;      // 포인터를 다시 nullptr로 초기화 (중요!)
            }
            });
        
        //////////////////////////////////////////////////////////////////////////////////

        // Quit Game 버튼
        auto quitGame = CreateUIObject<Button>(L"QuitGame_Button");
        quitGame->SetTextureName("QUIT_GAME_B");
        quitGame->SetText(L"");
        quitGame->SetSize({ 340, 58 });
        quitGame->SetPosition({ -522, 198 });

        // 1. OnClick: 클릭하면 실행될 이벤트
        quitGame->AddOnClick("Quit Game", []() {

            // 1. 메인 윈도우의 핸들(HWND)을 가져옵니다.
            HWND mainWindowHandle = JDGlobal::Window::WindowSize::Instance().GetHWND();

            // 2. PostMessage를 사용해 WM_CLOSE 메시지를 보냅니다.
            if (mainWindowHandle)
            {
                PostMessage(mainWindowHandle, WM_CLOSE, 0, 0);
            }
            });

        // 2. OnEnter: 마우스를 올리면 텍스처 변경
        quitGame->AddOnEnter("Highlight On", [this, quitGame]() {
            quitGame->SetTextureName("QUIT_GAME_A");
            if (m_hoverSfxChannel == nullptr)
            {
                // 채널 포인터를 TitleScene의 멤버 변수에 저장
                AudioManager::Instance().PlaySFX("Step", &m_hoverSfxChannel);
            }
            });

        // 3. OnExit: 마우스가 벗어나면 원래 텍스처로 복원
        quitGame->AddOnExit("Highlight Off", [this, quitGame]() {
            quitGame->SetTextureName("QUIT_GAME_B");
            if (m_hoverSfxChannel)
            {
                m_hoverSfxChannel->stop();        // 사운드 정지
                m_hoverSfxChannel = nullptr;      // 포인터를 다시 nullptr로 초기화 (중요!)
            }
            });


        m_lightParticles = std::make_unique<ParticleSystem>(
            D2DRenderer::Instance().GetD2DContext()
        );

        //////////////////////////////////////////////////////////////////////////////////

        // 옵션창
        auto optionUI = CreateUIObject<Image>(L"Option_Popup");
        optionUI->SetTextureName("Title");

        //////////////////////////////////////////////////////////////////////////////////

        // BGM 볼륨 조절 슬라이더
        auto bgmSlider = CreateUIObject<Slider>(L"BGM_Slider");
        bgmSlider->Assemble(this); // 씬의 도움을 받아 슬라이더 자식들을 조립합니다.

        // 이미지 개별 설정
        bgmSlider->SetBackgroundImage("VOLUME_LINE_2");
        bgmSlider->SetFillImage("VOLUME_LINE_1");
        bgmSlider->SetHandleImage("VOLUME_CAT_1");

        // 1. 초기값 설정: 현재 오디오 매니저의 BGM 볼륨 값으로 설정합니다.
        bgmSlider->SetValue(AudioManager::Instance().GetMusicVolume());

        // 2. OnValueChanged: 슬라이더 값이 바뀔 때마다 BGM 볼륨을 조절하도록 연결합니다.
        bgmSlider->AddOnValueChanged("Set BGM Volume", [](float newValue) {
            AudioManager::Instance().SetMusicVolume(newValue);
            });

        //////////////////////////////////////////////////////////////////////////////////

        //// SFX 볼륨 조절 슬라이더
        //auto sfxSlider = CreateUIObject<Slider>(L"SFX_Slider");
        //sfxSlider->Assemble(this);
        //sfxSlider->SetPosition({ -522, 320 }); // BGM 슬라이더 아래

        //// 이미지 개별 설정
        //bgmSlider->SetBackgroundImage("VOLUME_LINE_1");
        //bgmSlider->SetFillImage("VOLUME_LINE_2");
        //bgmSlider->SetHandleImage("VOLUME_CAT_2");

        //// 1. 초기값 설정: 현재 오디오 매니저의 SFX 볼륨 값으로 설정합니다.
        //sfxSlider->SetValue(AudioManager::Instance().GetSFXVolume());

        //// 2. OnValueChanged: 슬라이더 값이 바뀔 때마다 SFX 볼륨을 조절하도록 연결합니다.
        //sfxSlider->AddOnValueChanged("Set SFX Volume", [](float newValue) {
        //    AudioManager::Instance().SetSFXVolume(newValue);
        //    });

    }

    void TitleScene::OnLeave() {

        // 효과음 재생 중이면 정지
        if (m_hoverSfxChannel)
        {
            m_hoverSfxChannel->stop();        // 사운드 정지
            m_hoverSfxChannel = nullptr;      // 포인터를 다시 nullptr로 초기화 (중요!)
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
    }

    void TitleScene::Update(float deltaTime) {
        SceneBase::Update(deltaTime);

        MouseState state = InputManager::Instance().GetMouseState();
        float mouseX = static_cast<float>(state.pos.x);
        float mouseY = static_cast<float>(state.pos.y);
        if (m_lightParticles) {
            m_lightParticles->Update(deltaTime, Vector2F{ mouseX, mouseY });
        }

        ClickUpdate();
    }

    void TitleScene::FixedUpdate(float fixedDeltaTime) {
        SceneBase::FixedUpdate(fixedDeltaTime);
    }

    void TitleScene::LateUpdate(float deltaTime) {
        SceneBase::LateUpdate(deltaTime);
    }

    void TitleScene::Render(float dt) {
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
            D2DRenderer::Instance().RenderGameObject(*obj, dt);
        }

        for (auto& uiObj : m_uiObjects)
        {
            D2DRenderer::Instance().RenderUIObject(*uiObj);
        }

        if (m_lightParticles) {
            // 스크린 좌표로 바로 그릴 거면 Transform 초기화 후
            auto ctx = D2DRenderer::Instance().GetD2DContext();
            D2D1_MATRIX_3X2_F old;
            ctx->GetTransform(&old);
            ctx->SetTransform(D2D1::Matrix3x2F::Identity());

            m_lightParticles->Render(ctx);

            ctx->SetTransform(old);
        }
    }

    void TitleScene::ClickUpdate() {

        // ImGui가 마우스 입력을 사용 중이면 게임 내 클릭을 무시합니다.
        if (ImGui::GetIO().WantCaptureMouse)
            return;

        InputManager& input = InputManager::Instance();
        MouseState state = input.GetMouseState();

        // state.leftClicked 또는 state.leftPressed 등 필요한 입력 상태를 사용합니다.
        if (state.leftPressed)
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