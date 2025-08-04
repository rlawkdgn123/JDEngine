#include "pch.h"
#include "framework.h"
#include "TutorialScene.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "RectTransform.h"
#include "D2DTransform.h"
#include "Camera.h"
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

        CreateGameObject<Player>(L"Plyaer");

        std::shared_ptr<GameObject> testObject = std::make_shared<GameObject>();
        std::shared_ptr<GameObject>  birdObj = std::make_shared<GameObject>();

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
        {//Graybird�ִ�
            auto* boxObj3 = CreateGameObject<Player>(L"BoxObject3");
            boxObj3->GetComponent<Transform>()->SetPosition({ 100.0f, 100.0f });
            boxObj3->AddComponent<Editor_Clickable>();
            boxObj3->AddComponent<TextureRenderer>("Test", RenderLayerInfo{ SortingLayer::BackGround, 1 });
            boxObj3->AddComponent<AnimationRender>("GrayBird", 0.5, RenderLayerInfo{ SortingLayer::BackGround, 2 });

            auto frames = AssetManager::Instance().GetAnimationRender("GrayBird");
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

        // 1) 배경
        m_emptyMenu = CreateUIObject<Image>(L"MenuBackground");
        m_emptyMenu->SetTextureName("Menu");
        m_emptyMenu->SetSizeToOriginal();
        m_emptyMenu->SetActive(false);

        auto* bgRect = m_emptyMenu->GetComponent<RectTransform>();
        bgRect->SetPivot({ 0.f,0.f });
        bgRect->SetAnchor({ 0.f,0.f });


        std::vector<std::pair<std::wstring, std::string>> buttons = {
        { L"house1", "house" },
        { L"house2", "house2" },
        // 필요하다면 더 추가…
        };

        // 2) 버튼
        float yOffset = 75.f;
        float xOffset = 30.f;
        for (auto& info : buttons) {
            auto* btn = CreateUIObject<Button>(info.first);
            btn->SetTextureName(info.second);
            btn->SetText(L"");

            auto* btnRect = btn->GetComponent<RectTransform>();
            btnRect->SetPivot({ 0.f,0.f });
            btnRect->SetAnchor({ 0.f,0.f });
            btnRect->SetSize({ 50.f,50.f });

            btnRect->DetachFromParent();
            btnRect->SetParent(bgRect);
            btnRect->SetPosition({ xOffset, yOffset });

            m_emptyMenu->AddChild(btn);
            btn->SetActive(false);
            m_menuButtons.push_back(btn);

            xOffset += 50.f + 10.f; // width + gap
        }
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

                if (!m_emptyMenu) {
                    std::cerr << "[ERROR] m_emptyMenu is nullptr!\n";
                    return;
                }
                m_selectedCollider = collider;

                m_emptyMenu->SetActive(true);

                // 버튼들도 함께 보이기
                for (auto* btn : m_menuButtons) {
                    if (btn) {
                        btn->SetActive(true);
                    }
                    else {
                        std::cerr << "[WARNING] Found nullptr in m_menuButtons\n";
                    }
                }
            }
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
        auto camera = D2DRenderer::Instance().GetCamera();

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
            std::cout << "이거 실행됨?";

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

                    if (uiObj.get()->GetParent() == m_emptyMenu) {
                        m_selectedTool = static_cast<Button*>(uiObj.get());
                        std::wcout << L"선택된 툴: " << m_selectedTool->GetName() << std::endl;

                        if (m_selectedCollider) {
                            // 1) 콜라이더 월드 위치
                            Vector2F worldPos = m_selectedCollider->GetOwner()
                                ->GetComponent<Transform>()->GetPosition();

                            // 2) Building 생성
                            auto* building = CreateGameObject<Building>(
                                L"Building_" + m_selectedTool->GetName()
                            );
                          
                            // 3) 버튼(UIObject)의 이미지 컴포넌트에서 텍스처 키 가져오기
                            auto* uiImage = m_selectedTool->GetComponent<JDComponent::UI_ImageComponent>();
                            std::string texKey = uiImage
                                ? uiImage->GetTextureName()
                                : std::string("DefaultTexture");

                            // 4) Building에 TextureRenderer 컴포넌트 붙이고 텍스처 설정
                            auto* tr = building->AddComponent<TextureRenderer>(
                                texKey,
                                RenderLayerInfo{ SortingLayer::BackGround, 0 }
                            );
                            tr->SetTextureName(texKey);

                            // 5) Building 월드 위치 배치
                            building->GetComponent<Transform>()->SetPosition(worldPos);

                            // 6) 메뉴 닫기 & 버튼 숨기기
                            m_emptyMenu->SetActive(false);
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
