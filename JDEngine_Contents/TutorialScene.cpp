#include "pch.h"
#include "framework.h"
#include "TutorialScene.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "RectTransform.h"
#include "D2DTransform.h"
#include "Texture.h"


using namespace std;
using namespace JDGameObject::Content;
using JDComponent::AnimationRender;
using JDComponent::D2DTM::RectTransform;
using JDComponent::TextureRenderer;
namespace JDScene {

    // TestScene
    void TutorialScene::OnEnter() {
        using namespace JDGameObject;
        using namespace JDComponent;

        //cout << "[TestScene] OnEnter()\n";


        CreateGameObject<House>();
        CreateGameObject<FishingSpot>();
        CreateGameObject<Mine>();
        CreateGameObject<LumberMill>();

        CreateGameObject<Player>(L"Plyaer");

        std::shared_ptr<GameObject> testObject = std::make_shared<GameObject>();
        std::shared_ptr<GameObject> birdObj = std::make_shared<GameObject>();

        // =====================================================================
        // 게임 오브젝트 생성 (World Space)
        // =====================================================================

        m_fader.FadeIn(0.1f, 0.5f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f));
        m_fader.FadeIn(1.0f, 0.0f, D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));

        const float startX = -500.0f;
        const float startY = 300.0f;
        const float spacingX = 100.0f;
        const float spacingY = -100.0f;

        for (int col = 0; col < m_totalCols; ++col) {
            for (int row = 0; row < m_totalRows; ++row) {

                // 1) 이름 생성
                std::wstring name = L"Box_" + std::to_wstring(col) + L"_" + std::to_wstring(row);

                // 2) 생성
                auto* box = CreateGameObject<Grid>(name.c_str());

                // 3) 위치 세팅
                float x = startX + spacingX * col;
                float y = startY + spacingY * row;
                box->GetComponent<Transform>()->SetPosition({ x, y });
                box->AddComponent<Editor_Clickable>();

                // 4) 콜라이더 추가 및 인덱스 부여
                auto* collider = box->AddComponent<BoxCollider>(Vector2F{ 47,47 });
                int idx = col * m_totalRows + row;         // 0부터 (5*7-1) = 34 까지
                collider->SetIndex(idx);
            }
        }

        auto* circObj = CreateGameObject<Player>(L"CircleObject");
        circObj->GetComponent<Transform>()->SetPosition({ 200.0f, 100.0f });
        circObj->AddComponent<Editor_Clickable>();
        circObj->AddComponent<CircleCollider>(50.0f);

        {//Text
            auto* boxObj2 = CreateGameObject<Player>(L"BoxObject2");

            boxObj2->SetTag(JDGlobal::Base::GameTag::Mover);

            boxObj2->GetComponent<Transform>()->SetPosition({ 100.0f, 100.0f });
            boxObj2->AddComponent<Editor_Clickable>();
            boxObj2->AddComponent<TextureRenderer>("Test", RenderLayerInfo{ SortingLayer::BackGround, 1 });
            auto bitmap = static_cast<ID2D1Bitmap*> (AssetManager::Instance().GetTexture("Test"));
            auto size = bitmap->GetSize();
            boxObj2->AddComponent<BoxCollider>(Vector2F{ size.width / 2.0f, size.height / 2.0f });

            boxObj2->AddComponent<SFX>("Step");
        }
        {//Text
            auto* boxObj4 = CreateGameObject<Player>(L"BoxObject4");

            //boxObj4->SetTag(JDGlobal::Base::GameTag::Mover);

            boxObj4->GetComponent<Transform>()->SetPosition({ 100.0f, 100.0f });
            //boxObj4->AddComponent<Editor_Clickable>();
            boxObj4->AddComponent<TextureRenderer>("ATest", RenderLayerInfo{ SortingLayer::BackGround, 3 });
            auto bitmap = static_cast<ID2D1Bitmap*> (AssetManager::Instance().GetTexture("ATest"));
            auto size = bitmap->GetSize();
            boxObj4->AddComponent<BoxCollider>(Vector2F{ size.width / 2.0f, size.height / 2.0f });

            //boxObj4->AddComponent<SFX>("Step");
        }
        {//Graybird
            auto* boxObj3 = CreateGameObject<Player>(L"BoxObject3");
            boxObj3->GetComponent<Transform>()->SetPosition({ 100.0f, 100.0f });
            boxObj3->AddComponent<Editor_Clickable>();
            boxObj3->AddComponent<TextureRenderer>("Test", RenderLayerInfo{ SortingLayer::BackGround, 1 });
            boxObj3->AddComponent<AnimationRender>("Norway", 0.5, RenderLayerInfo{ SortingLayer::BackGround, 2 });

            // LEGACY
            //auto frames = AssetManager::Instance().GetAnimationRender("GrayBird");
            //if (frames && !frames->frames.empty()) {
            //    auto first = frames->frames[0].srcRect;
            //    float width = first.right - first.left;
            //    float height = first.bottom - first.top;
            //    Vector2F halfSize{ width * 0.5f, height * 0.5f };
            //    birdObj->AddComponent<BoxCollider>(halfSize);
            //    birdObj->AddComponent<Editor_Clickable>();

            auto frames = AssetManager::Instance().GetAnimationRender("Norway");
            if (frames && !frames->frames.empty()) {
                auto first = frames->frames[0].srcRect;
                float width = first.right - first.left;
                float height = first.bottom - first.top;
                Vector2F halfSize{ width * 0.5f, height * 0.5f };
                birdObj->AddComponent<BoxCollider>(halfSize);
                birdObj->AddComponent<Editor_Clickable>();
            }
        }

        int startCol = 1, startRow = 2;
        int regionW = 3, regionH = 3;

        for (int c = startCol; c < startCol + regionW; ++c) {
            for (int r = startRow; r < startRow + regionH; ++r) {
                int idx = c * m_totalRows + r;
                m_gameObjects[idx]->GetComponent<ColliderBase>()->SetOpen(true);
            }
        }

        // =====================================================================
        // UI 오브젝트 생성 (Screen Space)
        // =====================================================================

        // 1) 배경
        m_Menu = CreateUIObject<Image>(L"MenuBackground");

        auto& uptr = m_uiObjects.back();
        UIObject* rawPtr = uptr.get();
        m_TutorialUIObjects.push_back(rawPtr);

        m_Menu->SetTextureName("Menu");
        m_Menu->SetSizeToOriginal();
        //m_Menu->SetSize({});
        m_Menu->SetActive(false);

        auto* bgRect = m_Menu->GetComponent<RectTransform>();
        bgRect->SetPivot({ 0.f,0.f });
        bgRect->SetAnchor({ 0.f,0.f });
        bgRect->SetPosition({ 50, 50 });

        //각버튼 키 목록
        std::vector<std::pair<std::wstring, std::string>> EmptyButtons = {
        { L"FishingSpot", "house" },
        { L"LumberMill", "house2" },
        { L"Mine", "house2" },
        { L"House", "house2" },
        // 필요하다면 더 추가…
        }; 
        std::vector<std::pair<std::wstring, std::string>> FilledButtons = {
        { L"CAT", "CAT" },
        // 필요하다면 더 추가…
        };

        // 공통 초기 오프셋
        float xOffset = 100.f;
        float yOffset = 150.f;
        float gap = 100.f;

        // 2-1) 빈 메뉴용 버튼 생성
        {
            float x = xOffset;
            for (auto& info : EmptyButtons) {
                auto* btn = CreateUIObject<Button>(info.first);
                // 튜토리얼용 보관
                m_TutorialUIObjects.push_back(m_uiObjects.back().get());
                m_emptyButtons.push_back(btn);

                btn->SetTextureName(info.second);
                btn->SetText(L"");
                auto* rt = btn->GetComponent<RectTransform>();

                // [수정] RectTransform의 SetParent로만 부모-자식 관계를 설정합니다.
                rt->SetParent(bgRect);

                // [수정] 피벗을 중앙으로 설정하여 위치를 잡기 쉽게 합니다.
                rt->SetPivot({ 0.5f, 0.5f });
                rt->SetAnchor({ 0.0f, 0.0f });
                rt->SetSize({ 50, 50 });

                // 이제 이 Position은 버튼의 '중앙'을 부모의 (x, yOffset)으로 이동시킵니다.
                rt->SetPosition({ x, yOffset });


                // LEGACY : 과거 코드
                /*
                //rt->SetPivot({ 0,0 }); 
                //rt->SetAnchor({ 0,0 });
                //rt->SetSize({ 50,50 });
                //rt->SetParent(bgRect);
                //rt->SetPosition({ x, yOffset });
                */
                
                m_Menu->AddChild(btn);
                btn->SetActive(false);
                m_menuButtons.push_back(btn);

                x += gap;
            }
        }

        // 2-2) 채운 메뉴용 버튼 생성
        {
            float x = xOffset;
            for (auto& info : FilledButtons) {
                auto* btn = CreateUIObject<Button>(info.first);
                m_TutorialUIObjects.push_back(m_uiObjects.back().get());
                m_filledButtons.push_back(btn);

                btn->SetTextureName(info.second);
                btn->SetText(L"");
                auto* rt = btn->GetComponent<RectTransform>();

                // [수정] RectTransform의 SetParent로만 부모-자식 관계를 설정합니다.
                rt->SetParent(bgRect);

                // [수정] 피벗을 중앙으로 설정하여 위치를 잡기 쉽게 합니다.
                rt->SetPivot({ 0.5f, 0.5f });
                rt->SetAnchor({ 0.0f, 0.0f });
                rt->SetSize({ 50, 50 });

                // 이제 이 Position은 버튼의 '중앙'을 부모의 (x, yOffset)으로 이동시킵니다.
                rt->SetPosition({ x, yOffset });

                // LEGACY : 과거 코드
                /*
                //rt->SetPivot({ 0,0 }); 
                //rt->SetAnchor({ 0,0 });
                //rt->SetSize({ 50,50 });
                //rt->SetParent(bgRect);
                //rt->SetPosition({ x, xOffset });
                */

                m_Menu->AddChild(btn);
                btn->SetActive(false);
                m_menuButtons.push_back(btn);

                x += gap;
            }
        }

        auto camera = D2DRenderer::Instance().GetCamera();
        float screenW = static_cast<float>(camera->GetScreenWidth());
        float screenH = static_cast<float>(camera->GetScreenHeight());

        // 파티클 시스템 생성
        m_lightParticles = std::make_unique<ParticleSystem>(
            D2DRenderer::Instance().GetD2DContext()
        );

        // 이뮬러 위치를 원하는 곳으로 (예: 화면 중앙)
    }

    void TutorialScene::OnLeave() {
        //cout << "[TestScene] OnLeave()\n";
    }

    void TutorialScene::Update(float deltaTime) {
        SceneBase::Update(deltaTime);

        float speed = 100.f;  // 픽셀/초
        Vector2F delta{ speed * deltaTime, 0.f };

        for (auto& objPtr : m_gameObjects) {

            auto* sfx = objPtr->GetComponent<JDComponent::SFX>();
            if (!sfx)
                continue;

            auto* tm = objPtr->GetComponent<Transform>();
            if (!tm)
                continue;

            if (objPtr->GetTag() != JDGlobal::Base::GameTag::Mover)
                continue;

            Vector2F oldPos = tm->GetPosition();
            Vector2F newPos{ oldPos.x + delta.x, oldPos.y + delta.y };
            tm->SetPosition(newPos);

            // 5) 디버그: SFX 가진 녀석인지, 이동 로그 출력
            if (newPos.x != oldPos.x || newPos.y != oldPos.y)
            {
                //sfx->Play();

                // 디버그: 소리 재생 로그
                /*std::wcout
                    << L"[DEBUG] SFX Play at mover: key="
                    << L", from (" << oldPos.x << L"," << oldPos.y
                    << L") to (" << newPos.x << L"," << newPos.y
                    << L")\n";*/
            }
        }

        bool left = InputManager::Instance().GetMouseState().leftClicked;
        bool right = InputManager::Instance().GetMouseState().rightClicked;
        auto mousePos = GetMouseWorldPos();

        for (auto& objPtr : m_gameObjects) {
            auto* collider = objPtr->GetComponent<JDComponent::ColliderBase>();
            if (!collider) continue;
            
            auto* grid = dynamic_cast<Grid*>(objPtr.get());
            if (!grid)
                continue;

            int id = collider->GetIndex();

            if (!collider->IsMouseOver(mousePos))
                continue;

            if (left) {
                collider->SetOpen(true);
                std::cout << "[DEBUG] left ID: " << id << std::endl;
            }
            else if (right && collider->IsOpen()) {
                std::cout << "[DEBUG] right ID: " << id << std::endl;
                // 우클릭: 열린 콜라이더만 동작

                if (!m_Menu) {
                    std::cerr << "[ERROR] m_emptyMenu is nullptr!\n";
                    return;
                }
                m_fader.FadeIn(0.1f, 0.5f, D2D1::ColorF(1.0f, 0.0f, 0.0f, 1.0f));
            

                m_selectedCollider = collider;

                auto* boxCol = static_cast<JDComponent::BoxCollider*>(collider);
                if (boxCol->HasBuilding()) {
                    ShowFilledMenu();
                }
                else {
                    ShowEmptyMenu();
                }
            }
        }

        MouseState state = InputManager::Instance().GetMouseState();
        float mouseX = static_cast<float>(state.pos.x);
        float mouseY = static_cast<float>(state.pos.y);
        if (m_lightParticles) {
            m_lightParticles->Update(deltaTime, Vector2F{ mouseX, mouseY });
        }

        ClickUpdate();
    }

    void TutorialScene::FixedUpdate(float fixedDeltaTime) {
        SceneBase::FixedUpdate(fixedDeltaTime);
        //cout << "[TestScene] FixedUpdate() - dt: " << fixedDeltaTime << "\n";
    }

    void TutorialScene::LateUpdate(float deltaTime) {
        SceneBase::LateUpdate(deltaTime);
        //cout << "[TestScene] LateUpdate() - dt: " << deltaTime << "\n";
    }

    void TutorialScene::Render(float deltaTime) {
        SceneBase::Render(deltaTime);

        m_fader.Update(deltaTime);

        auto camera = D2DRenderer::Instance().GetCamera();
        float screenW = static_cast<float>(camera->GetScreenWidth());
        float screenH = static_cast<float>(camera->GetScreenHeight());

        if (camera)
            D2DRenderer::Instance().SetTransform(camera->GetViewMatrix());
        else
            D2DRenderer::Instance().SetTransform(D2D1::Matrix3x2F::Identity());

        //게임 오브젝트 렌더

        for (auto& obj : m_gameObjects) {
            D2DRenderer::Instance().RenderGameObject(*obj, deltaTime);
        }

        DrawColider();

        for (auto& uiObj : m_uiObjects)
        {
            D2DRenderer::Instance().RenderUIObject(*uiObj);
        }
        // 카메라 뷰가 걸려 있는 상태이므로, 
        // 페이드를 스크린 공간에서 그리려면 Transform을 초기화
        auto& renderer = D2DRenderer::Instance();
        auto ctx = renderer.GetD2DContext();

        // 현재 Transform(=카메라 뷰)을 잠시 저장
        D2D1_MATRIX_3X2_F old;
        ctx->GetTransform(&old);

        // 스크린 좌표계(Identity) 로 바꿔서 페이드만 그리기
        ctx->SetTransform(D2D1::Matrix3x2F::Identity());
        m_fader.Render(ctx, { (float)screenW, (float)screenH });

        // 원래 Transform(카메라 뷰) 복원
        ctx->SetTransform(old);

        for (auto& tutorialObj : m_TutorialUIObjects)
        {
            D2DRenderer::Instance().RenderUIObject(*tutorialObj);
        }

        for (auto& tutorialObj : m_TutorialObjects)
        {
            D2DRenderer::Instance().RenderGameObject(*tutorialObj, deltaTime);
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
    void TutorialScene::ClickUpdate()
    {
        auto camera = D2DRenderer::Instance().GetCamera();
        if (!camera) return;

        InputManager& input = InputManager::Instance();
        MouseState state = input.GetMouseState();

        if (ImGui::GetIO().WantCaptureMouse)
            return;

        if (state.leftClicked)
        {

            // (1) 마우스 좌표를 한 번만 계산해서 재사용합니다.
            // UI 클릭 판정에 사용할 스크린 좌표 (D2D 기준: Y 아래가 양수)
            Vector2F screenMousePos(static_cast<float>(state.pos.x), static_cast<float>(state.pos.y));
            // 게임 오브젝트 클릭 판정에 사용할 월드 좌표 (Unity 기준: Y 위가 양수)
            Vector2F worldMousePos = GetMouseWorldPos(); // 우리가 만든 통일된 함수 사용!

            bool clicked = false;

            ////////////////////////////////////////////////////////////////////////////////
            // 1. UI 클릭 검사 ( 스크린 좌표계 사용 )
            ////////////////////////////////////////////////////////////////////////////////

            for (int i = static_cast<int>(m_uiObjects.size()) - 1; i >= 0; --i)
            {
                auto& uiObj = m_uiObjects[i];
                if (!uiObj) continue;

                auto clickable = uiObj->GetComponent<Editor_Clickable>();
                if (clickable && clickable->IsHit(screenMousePos))
                {
                    SetSelectedObject(uiObj.get());
                    clicked = true;
                    //std::cout << " UI 오브젝트 클릭";

                    if (uiObj.get()->GetParent() == m_Menu) {
                        m_selectedTool = static_cast<Button*>(uiObj.get());
                        std::wcout << L"선택된 툴: " << m_selectedTool->GetName() << std::endl;

                        if (m_selectedCollider) {
                            auto* boxCol = static_cast<JDComponent::BoxCollider*>(m_selectedCollider);
                            //건물유무
                            boxCol->SetHasBuilding(true);
                            //선택된 콜라이더 사이즈
                            Vector2F halfSize = boxCol->GetHalfSize();
                            Vector2F fullSize = { halfSize.x * 2.f, halfSize.y * 2.f };

                            // 1) 콜라이더 월드 위치
                            Vector2F tileWorldPos = m_selectedCollider->GetOwner()
                                ->GetComponent<Transform>()->GetWorldPosition();

                            // 2) Building 생성
                            auto* building = CreateGameObject<Building>(
                                L"Building_" + m_selectedTool->GetName()
                            );
                            auto& uptr = m_gameObjects.back();
                            GameObject* rawPtr = uptr.get();
                            m_TutorialObjects.push_back(rawPtr);

                            // 3) 버튼(UIObject)의 이미지 컴포넌트에서 텍스처 키 가져오기
                            auto* uiImage = m_selectedTool->GetComponent<JDComponent::UI_ImageComponent>();
                            std::string texKey = uiImage
                                ? uiImage->GetTextureName()
                                : std::string("DefaultTexture");

                            auto* bitmap = static_cast<ID2D1Bitmap*>(
                                AssetManager::Instance().GetTexture(texKey));
                            auto bmpSize = bitmap->GetSize();

                            Vector2F scale = {
                                fullSize.x / bmpSize.width,
                                fullSize.y / bmpSize.height
                            };
                            building->GetComponent<Transform>()->SetScale(scale);

                            // 4) Building에 TextureRenderer 컴포넌트 붙이고 텍스처 설정
                            auto* tr = building->AddComponent<TextureRenderer>(
                                texKey,
                                RenderLayerInfo{ SortingLayer::BackGround, 0 }
                            );
                            tr->SetTextureName(texKey);

                            // 5) Building 월드 위치 배치
                            building->GetComponent<Transform>()->SetPosition(tileWorldPos);
                            
                            // 6) 메뉴 닫기 & 버튼 숨기기
                            m_Menu->SetActive(false);
                            for (auto* btn : m_menuButtons)
                                if (btn) btn->SetActive(false);

                            // 7) 선택 초기화
                            m_selectedTool = nullptr;
                            m_selectedCollider = nullptr;
                        }
                    }
                    break;
                }
            }

            ////////////////////////////////////////////////////////////////////////////////
            // 2. 게임오브젝트 클릭 검사 ( 월드 좌표계 사용 )
            ////////////////////////////////////////////////////////////////////////////////

            if (!clicked)
            {
                for (int i = static_cast<int>(m_gameObjects.size()) - 1; i >= 0; --i)
                {
                    auto& obj = m_gameObjects[i];
                    if (!obj || !obj->IsActive()) continue;

                    auto clickable = obj->GetComponent<Editor_Clickable>();
                    // 게임 오브젝트의 IsHit 함수에는 '월드 좌표'를 넘겨줍니다.
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

    void TutorialScene::ShowEmptyMenu() {
        m_Menu->SetActive(true);
        // 빈 메뉴 버튼들만 켜고
        for (auto* btn : m_emptyButtons)   btn->SetActive(true);
        // 채운 메뉴 버튼들은 끄기
        for (auto* btn : m_filledButtons)  btn->SetActive(false);
    }

    void TutorialScene::ShowFilledMenu() {
        m_Menu->SetActive(true);
        // 빈 메뉴 버튼들 끄고
        for (auto* btn : m_emptyButtons)   btn->SetActive(false);
        // 채운 메뉴 버튼들 켜기
        for (auto* btn : m_filledButtons)  btn->SetActive(true);
    }
}
