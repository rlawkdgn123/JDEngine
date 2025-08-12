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

        CreateSelectNationScene();            // 국가 선택 씬 생성
        InitSound();                          // 사운드 초기화
        InitParticle();                       // 파티클 초기화
    }

    void SelectNationScene::OnLeave() {

        FinalizeSelectNationScene();          // 국가 선택 씬 정리
    }

    void SelectNationScene::Update(float deltaTime) {
        SceneBase::Update(deltaTime);

        LogicUpdate();                        // 씬 로직 업데이트
        ClickUpdate();                        // 클릭 업데이트
    }

    void SelectNationScene::FixedUpdate(float fixedDeltaTime) {

        SceneBase::FixedUpdate(fixedDeltaTime);
    }

    void SelectNationScene::LateUpdate(float deltaTime) {

        SceneBase::LateUpdate(deltaTime);
    }

    void SelectNationScene::Render(float deltaTime) {

        RenderSelectNationScene(deltaTime);    // 씬 렌더
    }

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////

    void SelectNationScene::CreateSelectNationScene()
    {
        // UI
        ////////////////////////////////////////////////////////////////////////////////

        //배경
        //auto image = CreateUIObject<Image>(L"Title_Image");
        auto image = CreateUIObject<Image>(L"ART_CH_BACK");
        image->SetTextureName("ART_CH_BACK");
        //image->SetTextureName("SelectCharacter_Exam");

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

        // 뒤로가기 버튼
        auto back_Button = CreateUIObject<Button>(L"Back_Button");
        back_Button->SetTextureName("ART_Back02_mouseout");
        back_Button->SetText(L"");
        back_Button->SetSize({ 66, 60 });
        back_Button->SetPosition({ -887, 450 });


        // 1. OnClick: 클릭하면 실행될 이벤트
        back_Button->AddOnClick("Load TitleScene", []() {
            SceneManager::Instance().ChangeScene("TitleScene");
            });

        // 2. OnEnter: 마우스를 올리면 텍스처 변경
        back_Button->AddOnEnter("Highlight On", [this, back_Button]() {
            // 텍스트 변경
            back_Button->SetTextureName("ART_Back02_mouseover");
            });

        // 3. OnExit: 마우스가 벗어나면 원래 텍스처로 복원
        back_Button->AddOnExit("Highlight Off", [this, back_Button]() {
            // 텍스트 변경
            back_Button->SetTextureName("ART_Back02_mouseout");
            });

        ////////////////////////////////////////////////////////////////////////////////

        // 타이틀 텍스트 이미지
        m_titleTextImage = CreateUIObject<Image>(L"TitleText_Image");
        m_titleTextImage->SetTextureName("ART_Q_1");
        m_titleTextImage->SetSize({ 725, 68 });
        m_titleTextImage->SetPosition({ -400, 456 });

        // 국가 설명 이미지
        m_nationDescImage = CreateUIObject<Image>(L"NationDesc_Image");
        m_nationDescImage->SetTextureName("ART_CHAT");
        m_nationDescImage->SetSize({ 1536, 312 });
        m_nationDescImage->SetPosition({ 0, -340 });

        // 국가 설명 텍스트
        m_nationDescText = CreateUIObject<Text>(L"NationDesc_Text");
        m_nationDescText->SetText(L"국가 설명");
        m_nationDescText->SetTextFormatName("Sebang_58");
        m_nationDescText->SetColor(D2D1::ColorF(0.839f, 0.741f, 0.580f));
        m_nationDescText->SetSize({ 1600, 300 });
        m_nationDescText->SetPosition({ 0, -320 });

        // 국가 효과 텍스트
        m_nationEffectText = CreateUIObject<Text>(L"NationEffect_Text");
        m_nationEffectText->SetText(L"");
        m_nationEffectText->SetTextFormatName("Sebang_40");
        m_nationEffectText->SetColor(D2D1::ColorF(0.839f, 0.741f, 0.580f));
        m_nationEffectText->SetSize({ 1600, 300 });
        m_nationEffectText->SetPosition({ 0, -400 });

        ////////////////////////////////////////////////////////////////////////////////
        // 초임 집사 선택 버튼
        ////////////////////////////////////////////////////////////////////////////////

        // 2025.08.12 튜토리얼을 게임씬에 팝업 형식으로 띄워서
        /*
        m_newCatParentButton = CreateUIObject<Button>(L"newCatParent_Button");
        m_newCatParentButton->SetTextureName("ART_CHAT_mouseout");
        m_newCatParentButton->SetSize({ 1536, 150 });
        m_newCatParentButton->SetPosition({ 0, -260 });

        m_newCatParentButton->SetText(L"초임 집사 입니다.");
        m_newCatParentButton->SetTextFormatName("Sebang_40");
        m_newCatParentButton->SetTextColor(D2D1::ColorF(0.839f, 0.741f, 0.580f));

        // 1. OnClick: 클릭하면 실행될 이벤트
        m_newCatParentButton->AddOnClick("Load GameScene", [this]() {
            // TODO : 게임 씬으로 이동
            ResourceSystem::Instance().SetNation(m_selectedNation);
            SceneManager::Instance().ChangeScene("TutorialScene");
            });

        // 2. OnEnter: 마우스를 올리면 텍스처 변경
        m_newCatParentButton->AddOnEnter("Highlight On", [this]() {
            m_newCatParentButton->SetTextureName("ART_CHAT_mouseover");
            m_newCatParentButton->SetTextColor(D2D1::ColorF(D2D1::ColorF::Black));
            });

        // 3. OnExit: 마우스가 벗어나면 원래 텍스처로 복원
        m_newCatParentButton->AddOnExit("Highlight Off", [this]() {
            m_newCatParentButton->SetTextureName("ART_CHAT_mouseout");
            m_newCatParentButton->SetTextColor(D2D1::ColorF(0.839f, 0.741f, 0.580f));
            });
        */
        


        ////////////////////////////////////////////////////////////////////////////////
        // 경험있는 집사 선택 버튼
        ////////////////////////////////////////////////////////////////////////////////

        m_experiencedCatParentButton = CreateUIObject<Button>(L"experiencedCatParnet_Button");
        m_experiencedCatParentButton->SetTextureName("ART_CHAT_mouseout");
        m_experiencedCatParentButton->SetSize({ 1536, 150 });
        m_experiencedCatParentButton->SetPosition({ 0, -360 });

        // m_experiencedCatParentButton->SetText(L"경험있는 집사 입니다.");
        m_experiencedCatParentButton->SetText(L"게임 시작");
        m_experiencedCatParentButton->SetTextFormatName("Sebang_40");
        m_experiencedCatParentButton->SetTextColor(D2D1::ColorF(0.839f, 0.741f, 0.580f));

        // 1. OnClick: 클릭하면 실행될 이벤트
        m_experiencedCatParentButton->AddOnClick("Load GameScene", [this]() {
            // TODO : 게임 씬으로 이동
            ResourceSystem::Instance().SetNation(m_selectedNation);
            SceneManager::Instance().ChangeScene("GameScene");
            });

        // 2. OnEnter: 마우스를 올리면 텍스처 변경
        m_experiencedCatParentButton->AddOnEnter("Highlight On", [this]() {
            m_experiencedCatParentButton->SetTextureName("ART_CHAT_mouseover");
            m_experiencedCatParentButton->SetTextColor(D2D1::ColorF(D2D1::ColorF::Black));
            });

        // 3. OnExit: 마우스가 벗어나면 원래 텍스처로 복원
        m_experiencedCatParentButton->AddOnExit("Highlight Off", [this]() {
            m_experiencedCatParentButton->SetTextureName("ART_CHAT_mouseout");
            m_experiencedCatParentButton->SetTextColor(D2D1::ColorF(0.839f, 0.741f, 0.580f));
            });


        ////////////////////////////////////////////////////////////////////////////////
        // Navi / Felis / Kone
        ////////////////////////////////////////////////////////////////////////////////

        // 나비 (Navi) 공화국 이미지
        m_naviImageButton = CreateUIObject<Button>(L"Navi_Image");
        m_naviImageButton->SetTextureName("ART_SelectNavi01_mouseout");
        m_naviImageButton->SetText(L"");
        m_naviImageButton->SetSize({ 495, 585 });
        m_naviImageButton->SetPosition({ -505, 123 });
        m_naviImageButton->SetScale({ 1.f, 1.f });

        // 1. OnClick: 클릭하면 실행될 이벤트
        m_naviImageButton->AddOnClick("Load GameScene", []() {

            });

        // 2. OnEnter: 마우스를 올리면 텍스처 변경
        m_naviImageButton->AddOnEnter("Highlight On", [this]()
            {
                if (m_selectedNation == CatType::CatTypeMAX)
                {
                    m_naviImageButton->SetTextureName("ART_SelectNavi01_mouseover");
                    m_hoveredCharacter[static_cast<int>(CatType::Navi)] = true;
                }
            });

        // 3. OnExit: 마우스가 벗어나면 원래 텍스처로 복원
        m_naviImageButton->AddOnExit("Highlight Off", [this]()
            {
                if (m_selectedNation == CatType::CatTypeMAX)
                {
                    m_naviImageButton->SetTextureName("ART_SelectNavi01_mouseout");
                    m_hoveredCharacter[static_cast<int>(CatType::Navi)] = false;
                }
            });

        //// 나비 (Navi) 공화국 텍스트
        //auto navi_Text = CreateUIObject<Text>(L"Navi_Text");
        //navi_Text->SetText(L"나비 공화국");
        //navi_Text->SetSize({ 340, 58 });
        //navi_Text->SetPosition({ 0, 32 });

        ////////////////////////////////////////////////////////////////////////////////

        // 팰리스 (Felis) 연합왕국 이미지
        m_felisImageButton = CreateUIObject<Button>(L"Felis_Image");
        m_felisImageButton->SetTextureName("ART_SelectFelis01_mouseout");
        m_felisImageButton->SetText(L"");
        m_felisImageButton->SetSize({ 495, 585 });
        m_felisImageButton->SetPosition({ 0, 123 });
        m_felisImageButton->SetScale({ 1.f, 1.f });

        // 1. OnClick: 클릭하면 실행될 이벤트
        m_felisImageButton->AddOnClick("Load GameScene", []() {

            });

        // 2. OnEnter: 마우스를 올리면 텍스처 변경
        m_felisImageButton->AddOnEnter("Highlight On", [this]()
            {
                if (m_selectedNation == CatType::CatTypeMAX)
                {
                    m_felisImageButton->SetTextureName("ART_SelectFelis01_mouseover");
                    m_hoveredCharacter[static_cast<int>(CatType::Felis)] = true;
                }
            });

        // 3. OnExit: 마우스가 벗어나면 원래 텍스처로 복원
        m_felisImageButton->AddOnExit("Highlight Off", [this]()
            {
                if (m_selectedNation == CatType::CatTypeMAX)
                {
                    m_felisImageButton->SetTextureName("ART_SelectFelis01_mouseout");
                    m_hoveredCharacter[static_cast<int>(CatType::Felis)] = false;
                }
            });

        //// 팰리스 (Felis) 연합왕국 텍스트
        //auto felis_Text = CreateUIObject<Text>(L"Felis_Text");
        //felis_Text->SetText(L"팰리스 연합왕국");
        //felis_Text->SetSize({ 340, 58 });
        //felis_Text->SetPosition({ -522, 32 });

        ////////////////////////////////////////////////////////////////////////////////

        // 코네 (Kone) 제국 이미지
        m_koneImageButton = CreateUIObject<Button>(L"Kone_Image");
        m_koneImageButton->SetTextureName("ART_SelectKone01_mouseout");
        m_koneImageButton->SetText(L"");
        m_koneImageButton->SetSize({ 495, 585 });
        m_koneImageButton->SetPosition({ 505, 123 });
        m_koneImageButton->SetScale({ 1.f, 1.f });

        // 1. OnClick: 클릭하면 실행될 이벤트
        m_koneImageButton->AddOnClick("Load GameScene", []() {

            });

        // 2. OnEnter: 마우스를 올리면 텍스처 변경
        m_koneImageButton->AddOnEnter("Highlight On", [this]()
            {
                if (m_selectedNation == CatType::CatTypeMAX)
                {
                    m_koneImageButton->SetTextureName("ART_SelectKone01_mouseover");
                    m_hoveredCharacter[static_cast<int>(CatType::Kone)] = true;
                }
            });

        // 3. OnExit: 마우스가 벗어나면 원래 텍스처로 복원
        m_koneImageButton->AddOnExit("Highlight Off", [this]()
            {
                if (m_selectedNation == CatType::CatTypeMAX)
                {
                    m_koneImageButton->SetTextureName("ART_SelectKone01_mouseout");
                    m_hoveredCharacter[static_cast<int>(CatType::Kone)] = false;
                }
            });

        //// 코네 (Felis) 제국 텍스트
        //auto kone_Text = CreateUIObject<Text>(L"Kone_Text");
        //kone_Text->SetText(L"코네 제국");
        //kone_Text->SetSize({ 340, 58 });
        //kone_Text->SetPosition({ -522, 32 });

        ////////////////////////////////////////////////////////////////////////////////
    }

    void SelectNationScene::FinalizeSelectNationScene()
    {
        // 효과음 재생 중이면 정지
        if (m_hoverSfxChannel)
        {
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

        // 나비, 펠리스, 코네
        bool m_hoveredCharacter[static_cast<int>(CatType::CatTypeMAX)] = { false, false, false };

        // 국가 설명 이미지, 텍스트
        Image* m_nationDescImage = nullptr;
        Text* m_nationDescText = nullptr;
        Text* m_nationEffectText = nullptr;

        // 선택된 국가 타이틀 텍스트를 담은 이미지
        Image* m_titleTextImage = nullptr;


        Button* m_naviImageButton = nullptr;        // 나비 이미지 버튼
        Button* m_felisImageButton = nullptr;       // 펠리스 이미지 버튼
        Button* m_koneImageButton = nullptr;        // 코네 이미지 버튼

        // 클릭한 고양이 국가 ( 기본값 )
        CatType m_selectedNation = CatType::CatTypeMAX;

        // 초임 집사, 경험있는 집사 선택 버튼
        Button* m_newCatParentButton = nullptr;
        Button* m_experiencedCatParentButton = nullptr;
    }

    void SelectNationScene::InitSound()
    {

    }

    void SelectNationScene::InitParticle()
    {

    }

    void SelectNationScene::LogicUpdate()
    {
        // 국가 설명
        switch (m_selectedNation)
        {
        case CatType::Navi:
        case CatType::Felis:
        case CatType::Kone:

            // 나비, 펠리스, 코네 국가 선택
            ////////////////////////////////////////////////////////////////////////////////

            //m_newCatParentButton->SetActive(true);
            m_experiencedCatParentButton->SetActive(true);

            m_nationDescImage->SetActive(false);
            m_nationDescText->SetActive(false);
            m_nationEffectText->SetActive(false);

            ////////////////////////////////////////////////////////////////////////////////

            break;

        default:

            // 클릭한 고양이가 없다면 호버로 고양이 국가 설명보기.

            ////////////////////////////////////////////////////////////////////////////////

            //m_newCatParentButton->SetActive(false);
            m_experiencedCatParentButton->SetActive(false);

            m_nationDescImage->SetActive(true);
            m_nationDescText->SetActive(true);
            m_nationEffectText->SetActive(true);

            ////////////////////////////////////////////////////////////////////////////////

            bool isHoveredCharacter = false;
            for (int i = 0; i < static_cast<int>(CatType::CatTypeMAX); i++)
            {
                if (m_hoveredCharacter[i] == true)
                {
                    isHoveredCharacter = true;
                }
            }

            if (isHoveredCharacter)
            {
                // 나비
                if (m_hoveredCharacter[static_cast<int>(CatType::Navi)] == true) {
                    m_naviImageButton->SetTextureName("ART_SelectNavi01_mouseover");
                    m_nationDescText->SetText(
                        L"\"자유와 예술의 나라, 나비에서 새로운 꿈을!\"\n"
                        L"느긋하고 감수성 넘치는 낭만주의 고양이 공화국.\n"
                    );
                    m_nationEffectText->SetText(
                        L"식량 생산량 +10%"
                    );

                }

                // 펠리스
                else if (m_hoveredCharacter[static_cast<int>(CatType::Felis)] == true) {
                    m_felisImageButton->SetTextureName("ART_SelectFelis01_mouseover");
                    m_nationDescText->SetText(
                        L"\"모험심과 긍지가 살아 숨쉬는 펠리스!\"\n"
                        L"활달하고 자신감 넘치는 인상주의 고양이 왕국.\n"
                    );
                    m_nationEffectText->SetText(
                        L"목재 생산량 +10%"
                    );
                }

                // 코네
                else if (m_hoveredCharacter[static_cast<int>(CatType::Kone)] == true) {
                    m_koneImageButton->SetTextureName("ART_SelectKone01_mouseover");
                    m_nationDescText->SetText(
                        L"\"강철 같은 질서, 냉철한 전략의 제국 코네!\"\n"
                        L"엄격하고 규율 잡힌 사실주의 고양이들의 산업 제국.\n"
                    );
                    m_nationEffectText->SetText(
                        L"광물 생산량 +10%"
                    );
                }

                m_nationDescText->SetTextFormatName("Sebang_40");
                m_nationEffectText->SetTextFormatName("Sebang_40");
            }

            else
            {
                m_titleTextImage->SetTextureName("ART_Q_1");

                m_nationDescText->SetText(L"국가 설명");
                m_nationEffectText->SetText(L"");

                m_nationDescText->SetTextFormatName("Sebang_58");
            }

            break;
        }

        // 게임 시작 멘트
        switch (m_selectedNation)
        {
        case CatType::Navi:
            m_experiencedCatParentButton->SetText(L"나비 공화국의 자유로운 신 개척지의 질서를 확립할 행정관입니다.");
            break;
        case CatType::Felis:
            m_experiencedCatParentButton->SetText(L"펠리스 연합왕국의 방패로 변경을 수호할 기사단장입니다.");
            break;
        case CatType::Kone:
            m_experiencedCatParentButton->SetText(L"코네 제국의 강철 같은 규율로 최전선을 지휘할 백부장입니다.");
            break;
        default:
            m_experiencedCatParentButton->SetText(L"게임 시작");
            break;
        }
    }

    void SelectNationScene::ClickUpdate() {

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
                    // std::cout << " UI 오브젝트 클릭 함!!!!! ";

                    ////////////////////////////////////////////////////////////////////////////////
                    // SelectNationScene 로직
                    ////////////////////////////////////////////////////////////////////////////////

                    if (GetSelectedObject() == m_experiencedCatParentButton)
                    {
                        break;
                    }

                    // 나비 이미지가 클릭되었음.
                    if (GetSelectedObject() == m_naviImageButton)
                    {
                        // 또 누름 -> 원래대로 되돌림
                        if (m_selectedNation == CatType::Navi)
                        {
                            m_selectedNation = CatType::CatTypeMAX;

                            m_naviImageButton->SetTextureName("ART_SelectNavi01_mouseout");
                            m_felisImageButton->SetTextureName("ART_SelectFelis01_mouseout");
                            m_koneImageButton->SetTextureName("ART_SelectKone01_mouseout");

                            m_titleTextImage->SetTextureName("ART_Q_1");
                        }

                        else
                        {
                            m_selectedNation = CatType::Navi;

                            m_naviImageButton->SetTextureName("ART_SelectNavi01_mouseover");
                            m_felisImageButton->SetTextureName("ART_SelectFelis01_mouseout");
                            m_koneImageButton->SetTextureName("ART_SelectKone01_mouseout");

                            ClearHoveredCharacter();
                            m_hoveredCharacter[static_cast<int>(CatType::Navi)] = true;

                            m_titleTextImage->SetTextureName("ART_Q_4");
                        }
                    }
                    
                    // 펠릭스 이미지가 클릭되었음.
                    else if (GetSelectedObject() == m_felisImageButton)
                    {
                        // 또 누름 -> 원래대로 되돌림
                        if (m_selectedNation == CatType::Felis)
                        {
                            m_selectedNation = CatType::CatTypeMAX;

                            m_naviImageButton->SetTextureName("ART_SelectNavi01_mouseout");
                            m_felisImageButton->SetTextureName("ART_SelectFelis01_mouseout");
                            m_koneImageButton->SetTextureName("ART_SelectKone01_mouseout");

                            m_titleTextImage->SetTextureName("ART_Q_1");
                        }

                        else
                        {
                            m_selectedNation = CatType::Felis;

                            m_naviImageButton->SetTextureName("ART_SelectNavi01_mouseout");
                            m_felisImageButton->SetTextureName("ART_SelectFelis01_mouseover");
                            m_koneImageButton->SetTextureName("ART_SelectKone01_mouseout");

                            ClearHoveredCharacter();
                            m_hoveredCharacter[static_cast<int>(CatType::Felis)] = true;

                            m_titleTextImage->SetTextureName("ART_Q_3");
                        }
                    }

                    // 코네 이미지가 클릭되었음.
                    else if (GetSelectedObject() == m_koneImageButton)
                    {
                        // 또 누름 -> 원래대로 되돌림
                        if (m_selectedNation == CatType::Kone)
                        {
                            m_selectedNation = CatType::CatTypeMAX;

                            m_naviImageButton->SetTextureName("ART_SelectNavi01_mouseout");
                            m_felisImageButton->SetTextureName("ART_SelectFelis01_mouseout");
                            m_koneImageButton->SetTextureName("ART_SelectKone01_mouseout");

                            m_titleTextImage->SetTextureName("ART_Q_1");
                        }

                        else
                        {
                            m_selectedNation = CatType::Kone;

                            m_naviImageButton->SetTextureName("ART_SelectNavi01_mouseout");
                            m_felisImageButton->SetTextureName("ART_SelectFelis01_mouseout");
                            m_koneImageButton->SetTextureName("ART_SelectKone01_mouseover");

                            ClearHoveredCharacter();
                            m_hoveredCharacter[static_cast<int>(CatType::Kone)] = true;

                            m_titleTextImage->SetTextureName("ART_Q_2");
                        }
                    }

                    else
                    {
                        m_selectedNation = CatType::CatTypeMAX;

                        m_naviImageButton->SetTextureName("ART_SelectNavi01_mouseout");
                        m_felisImageButton->SetTextureName("ART_SelectFelis01_mouseout");
                        m_koneImageButton->SetTextureName("ART_SelectKone01_mouseout");

                        m_nationDescText->SetText(L"국가 설명");
                        m_nationDescText->SetTextFormatName("Sebang_50");

                        ClearHoveredCharacter();
                        m_titleTextImage->SetTextureName("ART_Q_1");
                    }

                    // end _ SelectNationScene 로직
                    ////////////////////////////////////////////////////////////////////////////////

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
                        // std::cout << " 게임 오브젝트 클릭 함!!!!! ";
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

    void SelectNationScene::ParticleUpdate(float deltaTime)
    {
    }

    void SelectNationScene::RenderSelectNationScene(float deltaTime)
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

    void SelectNationScene::RenderParticle()
    {
    }

    void SelectNationScene::ClearHoveredCharacter()
    {
        for (int i = 0; i < static_cast<int>(CatType::CatTypeMAX); i++)
        {
            m_hoveredCharacter[i] = false;
        }
    }
}