#include "pch.h"
#include "framework.h"
#include "SceneList.h"
#include "TitleScene.h"

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
        //std::unique_ptr<GameObject> testObject = std::make_unique<GameObject>();
        //std::unique_ptr<GameObject> birdObj = std::make_unique<GameObject>();

        //{//Test 텍스처 이미지 게임오브젝트 생성
        //    auto tf = testObject->GetComponent<Transform>();
        //    tf->SetPosition({ 300.f, 200.f });
        //    tf->SetScale({ 1.f, 1.f });

        //    testObject->AddComponent<TextureRenderer>("Test");
        //    testObject->AddComponent<Editor_Clickable>();

        //    m_gameObjects.push_back(std::move(testObject));
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
        
        // 타이틀 배경
        auto image = CreateUIObject<Image>(L"Title_Image");
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

        // GameStart 버튼
        auto gameStart = CreateUIObject<Button>(L"GameStart_Button");
        gameStart->SetTextureName("GAME_START_A");
        gameStart->SetSizeToOriginal();

        //// Setting 버튼
        //auto setting = CreateUIObject<Button>(L"Setting_Button");
        //setting->SetTextureName("SETTING_A");
        //setting->SetSizeToOriginal();
        //
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
            // 1. 마우스 스크린 좌표 (Unity 스타일: 좌측 하단이 원점, Y축 위쪽이 양수)
            float screenHeight = static_cast<float>(camera.get()->GetScreenHeight());

            Vector2F mousePos(
                static_cast<float>(state.pos.x),
                static_cast<float>(state.pos.y)
            );

            /*  디버그용 출력문
            std::cout << "마우스: (" << mousePos.x << ", " << mousePos.y << ")" << std::endl;
            std::cout << "화면 높이: " << screenHeight << std::endl;
            */

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

                /*  디버그용 출력문
                std::cout << "Unity 마우스: (" << unityMousePos.x << ", " << unityMousePos.y << ")" << std::endl;
                std::cout << "월드 마우스: (" << worldMousePos.x << ", " << worldMousePos.y << ")" << std::endl;
                */

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