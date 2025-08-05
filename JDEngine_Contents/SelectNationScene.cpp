#include "pch.h"
#include "framework.h"
#include "SelectNationScene.h"
#include "SceneList.h"
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

    // SelectNationScene
    void SelectNationScene::OnEnter() {

        // Init
        // 초기화 코드 필요 ( 오브젝트가 계속 생성되는 문제 있음. )

        // UI
        ////////////////////////////////////////////////////////////////////////////////
        
        // 뒤로가기 버튼
        auto back_Button = CreateUIObject<Button>(L"Back_Button");
        back_Button->SetTextureName("Back_Button");
        back_Button->SetText(L"");
        back_Button->SetSize({ 100, 100 });
        back_Button->SetPosition({ -890, -480 });


        // 1. OnClick: 클릭하면 실행될 이벤트
        back_Button->AddOnClick("Load TitleScene", []() {
            SceneManager::Instance().ChangeScene("TitleScene");
            });

        // 2. OnEnter: 마우스를 올리면 텍스처 변경
        back_Button->AddOnEnter("Highlight On", [this, back_Button]() {
            // 텍스트 변경
            });

        // 3. OnExit: 마우스가 벗어나면 원래 텍스처로 복원
        back_Button->AddOnExit("Highlight Off", [this, back_Button]() {
            // 텍스트 변경
            });

        ////////////////////////////////////////////////////////////////////////////////

        // 타이틀 텍스트
        auto title_Text = CreateUIObject<Text>(L"Title_Text");
        title_Text->SetText(L"고양이 세계 펠리니아의 승자가 될 국가는?");
        title_Text->SetTextFormatName("MalgunGothic_70");
        title_Text->SetSize({ 1600, 150 });
        title_Text->SetPosition({ 0, -430 });

        // 국가 설명 텍스트
        auto nationDesc_Text = CreateUIObject<Text>(L"NationDesc_Text");
        nationDesc_Text->SetText(L"국가 설명");
        nationDesc_Text->SetTextFormatName("MalgunGothic_90");
        nationDesc_Text->SetSize({ 1600, 150 });
        nationDesc_Text->SetPosition({ 0, 330 });


        // Felis / Navi / Kone
        ////////////////////////////////////////////////////////////////////////////////

        // 팰리스 (Felis) 연합왕국 이미지
        auto felis_Image = CreateUIObject<Button>(L"Felis_Image");
        felis_Image->SetTextureName("CAT");
        felis_Image->SetText(L"");
        felis_Image->SetSizeToOriginal();
        felis_Image->SetPosition({ 0, -60 });
        felis_Image->SetScale({ 1.5f, 1.5f });
        
        // 1. OnClick: 클릭하면 실행될 이벤트
        felis_Image->AddOnClick("Load GameScene", []() {
            
            });

        // 2. OnEnter: 마우스를 올리면 텍스처 변경
        felis_Image->AddOnEnter("Highlight On", [this, felis_Image, nationDesc_Text]() {
            nationDesc_Text->SetText(
                L"\"모험심과 긍지가 살아 숨쉬는 펠리스!\"\n"
                L"활달하고 자신감 넘치는 털보 고양이 왕국.\n"
                L"털보 고양이 60% / 목재 생산량 +10%"
            );
            nationDesc_Text->SetTextFormatName("MalgunGothic_60");
            });

        // 3. OnExit: 마우스가 벗어나면 원래 텍스처로 복원
        felis_Image->AddOnExit("Highlight Off", [this, felis_Image, nationDesc_Text]() {
            nationDesc_Text->SetText(L"국가 설명");
            nationDesc_Text->SetTextFormatName("MalgunGothic_90");
            });

        // 팰리스 (Felis) 연합왕국 텍스트
        auto felis_Text = CreateUIObject<Text>(L"Felis_Text");
        felis_Text->SetText(L"팰리스 연합왕국");
        felis_Text->SetSize({ 340, 58 });
        felis_Text->SetPosition({ 0, 32 });

        ////////////////////////////////////////////////////////////////////////////////

        // 나비 (Navi) 공화국 이미지
        auto navi_Image = CreateUIObject<Button>(L"Navi_Image");
        navi_Image->SetTextureName("CAT");
        navi_Image->SetText(L"");
        navi_Image->SetSizeToOriginal();
        navi_Image->SetPosition({ 522, -60 });
        navi_Image->SetScale({ 1.5f, 1.5f });

        // 1. OnClick: 클릭하면 실행될 이벤트
        navi_Image->AddOnClick("Load GameScene", []() {

            });

        // 2. OnEnter: 마우스를 올리면 텍스처 변경
        navi_Image->AddOnEnter("Highlight On", [this, navi_Image, nationDesc_Text]() {
            nationDesc_Text->SetText(
                L"\"자유와 예술의 나라, 나비에서 새로운 꿈을!\"\n"
                L"느긋하고 감수성 넘치는 줄무늬 고양이 왕국\n"
                L"줄무늬 고양이 60% / 식량 생산량 +10%"
            );
            nationDesc_Text->SetTextFormatName("MalgunGothic_60");
            });

        // 3. OnExit: 마우스가 벗어나면 원래 텍스처로 복원
        navi_Image->AddOnExit("Highlight Off", [this, navi_Image, nationDesc_Text]() {
            nationDesc_Text->SetText(L"국가 설명");
            nationDesc_Text->SetTextFormatName("MalgunGothic_90");
            });

        // 나비 (Navi) 공화국 텍스트
        auto navi_Text = CreateUIObject<Text>(L"Navi_Text");
        navi_Text->SetText(L"나비 공화국");
        navi_Text->SetSize({ 340, 58 });
        navi_Text->SetPosition({ -522, 32 });

        ////////////////////////////////////////////////////////////////////////////////

        // 코네 (Kone) 제국 이미지
        auto kone_Image = CreateUIObject<Button>(L"Kone_Image");
        kone_Image->SetTextureName("CAT");
        kone_Image->SetText(L"");
        kone_Image->SetSizeToOriginal();
        kone_Image->SetPosition({ -522, -60 });
        kone_Image->SetScale({ 1.5f, 1.5f });

        // 1. OnClick: 클릭하면 실행될 이벤트
        kone_Image->AddOnClick("Load GameScene", []() {

            });

        // 2. OnEnter: 마우스를 올리면 텍스처 변경
        kone_Image->AddOnEnter("Highlight On", [this, kone_Image, nationDesc_Text]() {
            nationDesc_Text->SetText(
                L"\"강철 같은 질서, 냉철한 전략의 제국 코네!\"\n"
                L"엄격하고 규율 잡힌 검정 고양이들의 산업 제국.\n"
                L"검정 고양이 60% / 광물 생산량 +10%"
            );
            nationDesc_Text->SetTextFormatName("MalgunGothic_60");
            });

        // 3. OnExit: 마우스가 벗어나면 원래 텍스처로 복원
        kone_Image->AddOnExit("Highlight Off", [this, kone_Image, nationDesc_Text]() {
            nationDesc_Text->SetText(L"국가 설명");
            nationDesc_Text->SetTextFormatName("MalgunGothic_90");
            });

        // 팰리스 (Felis) 제국 텍스트
        auto kone_Text = CreateUIObject<Text>(L"Kone_Text");
        kone_Text->SetText(L"코네 제국");
        kone_Text->SetSize({ 340, 58 });
        kone_Text->SetPosition({ -522, 32 });

        ////////////////////////////////////////////////////////////////////////////////
    }

    void SelectNationScene::OnLeave() {
    }

    void SelectNationScene::Update(float deltaTime) {
        SceneBase::Update(deltaTime);

        ClickUpdate();
    }

    void SelectNationScene::FixedUpdate(float fixedDeltaTime) {
        SceneBase::FixedUpdate(fixedDeltaTime);
    }

    void SelectNationScene::LateUpdate(float deltaTime) {
        SceneBase::LateUpdate(deltaTime);
    }

    void SelectNationScene::Render(float dt) {
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

    void SelectNationScene::ClickUpdate() {
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