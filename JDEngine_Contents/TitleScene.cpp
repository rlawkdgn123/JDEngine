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

        // GameObject
        ////////////////////////////////////////////////////////////////////////////////
        // CreateGameObject<Player>(L"Plyaer");

        //std::shared_ptr<GameObject> testObject = std::make_shared<GameObject>();

        //const float startX = -500.0f;
        //const float startY = 300.0f;
        //const float spacingX = 100.0f;
        //const float spacingY = -100.0f;

        //for (int col = 0; col < 5; ++col) {
        //    for (int row = 0; row < 7; ++row) {
        //        std::wstring name = L"Box_" + std::to_wstring(col) + L"_" + std::to_wstring(row);

        //        auto* box = CreateGameObject<Grid>(name.c_str());

        //        float x = startX + spacingX * col;
        //        float y = startY + spacingY * row;
        //        box->GetComponent<Transform>()->SetPosition({ x, y });

        //        box->AddComponent<BoxCollider>(Vector2F{ 47.0f,47.0f });

        //    }
        //}

        //{ 
        //    auto* boxObj2 = CreateGameObject<Player>(L"BoxObject2");
        //    boxObj2->GetComponent<Transform>()->SetPosition({ 100.0f, 100.0f });
        //    boxObj2->AddComponent<Editor_Clickable>();
        //    boxObj2->AddComponent<BGM>("MainTheme");
        //    boxObj2->AddComponent<TextureRenderer>("Test", RenderLayerInfo{ SortingLayer::BackGround, 1 });
        //    auto bitmap = static_cast<ID2D1Bitmap*> (AssetManager::Instance().GetTexture("Test"));
        //    auto size = bitmap->GetSize();
        //    boxObj2->AddComponent<BoxCollider>(Vector2F{ size.width / 2.0f, size.height / 2.0f });
        //}

        //{//새 애니메이션 게임오브젝트 생성
        //    auto birdTf = birdObj->GetComponent<Transform>();
        //    birdTf->SetPosition({ 100.f, 50.f });
        //    birdTf->SetScale({ 1.0f, 1.0f });

        //    birdObj->AddComponent<TextureRenderer>("GrayBird");
        //    birdObj->AddComponent<AnimationRender>("GrayBird", 0.5);// 뒤에 값은 speed
        //    birdObj->AddComponent<Editor_Clickable>();

        //    m_gameObjects.push_back(std::move(birdObj));
        //}

        // UI
        ////////////////////////////////////////////////////////////////////////////////
        
        //타이틀 배경
        //auto image = CreateUIObject<Image>(L"Title_Image");
        auto image = CreateUIObject<Image>(L"Title_Exam");
        image->SetTextureName("Title_Exam");

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
        gameStart->SetSizeToOriginal();

        // 1. OnClick: 클릭하면 InGameScene으로 전환
        gameStart->AddOnClick("Load GameScene", []() {
            // SceneManager를 이용해 다음 씬으로 넘어갑니다.
            SceneManager::Instance().ChangeScene("TutorialScene");
            });

        // 2. OnEnter: 마우스를 올리면 텍스처 변경 (예: 밝은 버전)
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
            //if (m_hoverSfxChannel)
            //{
            //    m_hoverSfxChannel->stop();        // 사운드 정지
            //    m_hoverSfxChannel = nullptr;      // 포인터를 다시 nullptr로 초기화 (중요!)
            //}
            });


        ////////////////////////////////////////////////////////////////////////////////

        // Setting 버튼
        auto setting = CreateUIObject<Button>(L"Setting_Button");
        setting->SetTextureName("SETTING_A");
        setting->SetSizeToOriginal();
        
        ////////////////////////////////////////////////////////////////////////////////

        //// Quit Game 버튼
        //auto quitGame = CreateUIObject<Button>(L"QuitGame_Button");
        //quitGame->SetTextureName("QUITGAME_A");
        //quitGame->SetSizeToOriginal();
    }

    void TitleScene::OnLeave() {
    }

    void TitleScene::Update(float deltaTime) {
        SceneBase::Update(deltaTime);

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
    }

    void TitleScene::ClickUpdate() {
        auto camera = D2DRenderer::Instance().GetCamera();
        if (!camera) return;

        InputManager& input = InputManager::Instance();
        MouseState state = input.GetMouseState();

        if (ImGui::GetIO().WantCaptureMouse)
            return;

        if (state.leftPressed)
        {
            // 1. 마우스 스크린 좌표
            float screenHeight = static_cast<float>(camera.get()->GetScreenHeight());

            Vector2F mousePos(
                static_cast<float>(state.pos.x),
                static_cast<float>(state.pos.y)
            );

            //디버그용 출력문
            std::cout << "마우스: (" << mousePos.x << ", " << mousePos.y << ")" << std::endl;
            std::cout << "화면 높이: " << screenHeight << std::endl;
            

            // 클릭 여부
            bool clicked = false;

            ////////////////////////////////////////////////////////////////////////////////
            // 1. UI 클릭 검사 ( 스크린 좌표계 사용 )
            ////////////////////////////////////////////////////////////////////////////////

            for (int i = static_cast<int>(m_uiObjects.size()) - 1; i >= 0; --i)
            {
                auto& uiObj = m_uiObjects[i];
                if (!uiObj) continue;

                auto clickable = uiObj->GetComponent<Editor_Clickable>();
                if (clickable && clickable->IsHit(mousePos))
                {
                    SetSelectedObject(uiObj.get());
                    clicked = true;
                    std::cout << " UI 오브젝트 클릭 함!!!!! ";
                    break;
                }
            }

            ////////////////////////////////////////////////////////////////////////////////
            // 2. 게임오브젝트 클릭 검사 ( 월드 좌표계 사용 )
            ////////////////////////////////////////////////////////////////////////////////

            if (!clicked)
            {
                // 게임오브젝트용으로만 Unity 좌표 변환
                Vector2F unityMousePos(mousePos.x, screenHeight - mousePos.y);
                // 스크린 좌표를 월드 좌표로 변환
                Vector2F worldMousePos = camera->ScreenToWorldPoint(unityMousePos);

                //디버그용 출력문
                std::cout << "Unity 마우스: (" << unityMousePos.x << ", " << unityMousePos.y << ")" << std::endl;
                std::cout << "월드 마우스: (" << worldMousePos.x << ", " << worldMousePos.y << ")" << std::endl;
                

                for (int i = static_cast<int>(m_gameObjects.size()) - 1; i >= 0; --i)
                {
                    auto& obj = m_gameObjects[i];
                    if (!obj) continue;

                    auto clickable = obj->GetComponent<Editor_Clickable>();
                    if (clickable && clickable->IsHit(worldMousePos))
                    {
                        SetSelectedObject(obj.get());
                        clicked = true;
                        std::cout << " 게임 오브젝트 클릭 함!!!!! ";
                        break;
                    }
                }
            }

            if (!clicked)
            {
                SetSelectedObject(nullptr);
            }
        }
    }
}