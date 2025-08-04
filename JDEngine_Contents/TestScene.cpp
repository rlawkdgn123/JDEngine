#include "pch.h"
#include "framework.h"
#include "SceneList.h"
#include "BoxCollider.h"
#include "CircleCollider.h"

using namespace std;
using namespace JDGameObject::Content;
using JDComponent::AnimationRender;

namespace JDScene {

    // TestScene
    void TestScene::OnEnter() {
        using namespace JDGameObject;
        using namespace JDComponent;

        //cout << "[TestScene] OnEnter()\n";

        CreateGameObject<Player>(L"Plyaer");

        std::shared_ptr<GameObject> testObject = std::make_shared<GameObject>();
        std::shared_ptr<GameObject> birdObj = std::make_shared<GameObject>();

        // UI
        std::shared_ptr<UIObject> testUIObject = std::make_shared<UIObject>();

        auto rtf = testUIObject->GetComponent<RectTransform>();
        rtf->SetPosition({ 0.f, 0.f });
        rtf->SetScale({ 1.f, 1.f });

        testUIObject->AddComponent<UI_ImageComponent>("Test");
        auto imageComponent = testUIObject->GetComponent<UI_ImageComponent>();

        m_UIObjects.push_back(testUIObject);

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

                // 4) 콜라이더 추가 및 인덱스 부여
                auto* collider = box->AddComponent<BoxCollider>(Vector2F{ 47,47 });
                int idx = col * m_totalRows + row;         // 0부터 (5*7-1) = 34 까지
                collider->SetIndex(idx);
            }
        }

        auto* circObj = CreateGameObject<Player>(L"CircleObject");
        circObj->GetComponent<Transform>()->SetPosition({ 200.0f, 100.0f });
        circObj->AddComponent<CircleCollider>(50.0f);
        circObj->AddComponent<Editor_Clickable>();

        {//Text
            auto* boxObj2 = CreateGameObject<Player>(L"BoxObject2");

            boxObj2->SetTag(JDGlobal::Base::GameTag::Mover);

            boxObj2->GetComponent<Transform>()->SetPosition({ 100.0f, 100.0f });
            
            boxObj2->AddComponent<TextureRenderer>("Test", RenderLayerInfo{ SortingLayer::BackGround, 1 });
            boxObj2->AddComponent<Editor_Clickable>();
            auto bitmap = static_cast<ID2D1Bitmap*> (AssetManager::Instance().GetTexture("Test"));
            auto size = bitmap->GetSize();
            boxObj2->AddComponent<BoxCollider>(Vector2F{ size.width / 2.0f, size.height / 2.0f });

            boxObj2->AddComponent<SFX>("Step");
        }


        {//Graybird

            auto* boxObj3 = CreateGameObject<Player>(L"BoxObject3");
            boxObj3->GetComponent<Transform>()->SetPosition({ 100.0f, 100.0f });
            boxObj3->AddComponent<TextureRenderer>("Test", RenderLayerInfo{ SortingLayer::BackGround, 1 });
            boxObj3->AddComponent<AnimationRender>("GrayBird", 0.5, RenderLayerInfo{ SortingLayer::BackGround, 2 });
            boxObj3->AddComponent<Editor_Clickable>();

            auto frames = AssetManager::Instance().GetAnimationRender("GrayBird");
            if (frames && !frames->frames.empty()) {
                auto first = frames->frames[0].srcRect;
                float width = first.right - first.left;
                float height = first.bottom - first.top;
                Vector2F halfSize{ width * 0.5f, height * 0.5f };
                birdObj->AddComponent<BoxCollider>(halfSize);
                
            }
        }

    }

    void TestScene::OnLeave() {
        //cout << "[TestScene] OnLeave()\n";
    }

    void TestScene::Update(float deltaTime) {
        SceneBase::Update(deltaTime);

        ClickUpdate();

        //bool leftPressed = InputManager::Instance().GetKeyPressed(VK_LBUTTON);
        bool leftPressed = InputManager::Instance().GetMouseState().leftClicked;
        //bool leftPressed = InputManager::Instance().GetMouseState().leftPressed;
        size_t n = m_gameObjects.size();
        for (size_t i = 0; i < n; ++i) {
            auto* obj = m_gameObjects[i].get();
            if (!obj) continue;

            auto* col = obj->GetComponent<JDComponent::ColliderBase>();
            if (!col) continue;

            if (col->IsMouseOver(GetMouseWorldPos()) && leftPressed) {
                std::cout << "[DEBUG] 클릭된 콜라이더 인덱스: "
                    << i << std::endl;
            }
        }
    }

    void TestScene::FixedUpdate(float fixedDeltaTime) {
        SceneBase::FixedUpdate(fixedDeltaTime);
        //cout << "[TestScene] FixedUpdate() - dt: " << fixedDeltaTime << "\n";
    }

    void TestScene::LateUpdate(float deltaTime) {
        InputManager::Instance().SetMouseState()->leftClicked = false; // Clicked 다음 프레임에 체크
        InputManager::Instance().SetMouseState()->rightClicked = false;
        SceneBase::LateUpdate(deltaTime);
        //cout << "[TestScene] LateUpdate() - dt: " << deltaTime << "\n";
    }

    void TestScene::Render(float deltaTime) {
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
    void TestScene::ClickUpdate()
    {
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
/*
    // TestScene
    void TestScene::OnEnter() {
        using namespace JDGameObject;
        using namespace JDComponent;

        //cout << "[TestScene] OnEnter()\n";
        CreateGameObject<Player>(L"ㅁㄴㅇ");

        {//Test 텍스처 이미지 게임오브젝트 생성
            std::shared_ptr<GameObject> testObject = std::make_shared<GameObject>();
            auto tf = testObject->GetComponent<Transform>();
            tf->SetPosition({ 0.f, 0.f });
            tf->SetScale({ 1.f, 1.f });

            testObject->AddComponent<TextureRenderer>("Test", RenderLayerInfo{ SortingLayer::BackGround, 1 });

            auto bitmap = static_cast<ID2D1Bitmap*>(
                AssetManager::Instance().GetTexture("Test"));
            std::cout << "GetTexture(\"Test\") returned: " << (bitmap ? "OK" : "NULL") << std::endl;

            if (bitmap) {
                D2D1_SIZE_F sz = bitmap->GetSize();
                Vector2F halfSize{ sz.width * 0.5f, sz.height * 0.5f };
                testObject->AddComponent<BoxCollider>(halfSize);
            }
            else {
                Vector2F halfSize{ 50.f, 50.f };
                testObject->AddComponent<BoxCollider>(halfSize);
            }

            m_sceneObjects.push_back(testObject);
        }

        {//새 애니메이션 게임오브젝트 생성
            std::shared_ptr<GameObject>  birdObj = std::make_shared<GameObject>();
            auto birdTf = birdObj->GetComponent<Transform>();
            birdTf->SetPosition({ 100.f, 50.f });
            birdTf->SetScale({ 1.0f, 1.0f });

            birdObj->AddComponent<TextureRenderer>("GrayBird");
            birdObj->AddComponent<AnimationRender>("GrayBird", 0.5, RenderLayerInfo{ SortingLayer::BackGround, 2 });// 뒤에 값은 speed

            auto frames = AssetManager::Instance().GetAnimationRender("GrayBird");
            if (frames && !frames->frames.empty()) {
                auto first = frames->frames[0].srcRect;
                float width = first.right - first.left;
                float height = first.bottom - first.top;
                Vector2F halfSize{ width * 0.5f, height * 0.5f };
                birdObj->AddComponent<BoxCollider>(halfSize);
            }

            m_sceneObjects.push_back(birdObj);
        }

        // UI
        std::shared_ptr<UIObject> testUIObject = std::make_shared<UIObject>();

        auto rtf = testUIObject->GetComponent<RectTransform>();
        rtf->SetPosition({ 0.f, 0.f });
        rtf->SetScale({ 1.f, 1.f });

        testUIObject->AddComponent<UI_ImageComponent>("Test");
        auto imageComponent = testUIObject->GetComponent<UI_ImageComponent>();

        m_UIObjects.push_back(testUIObject);


        const float startX = -500.0f;
        const float startY = 300.0f;
        const float spacingX = 100.0f;
        const float spacingY = -100.0f;

        for (int col = 0; col < 5; ++col) {
            for (int row = 0; row < 7; ++row) {
                // 고유 이름 생성 (원하면 동일명으로 해도 됩니다)
                std::wstring name = L"Box_" + std::to_wstring(col) + L"_" + std::to_wstring(row);

                // GameObject 생성
                auto* box = CreateGameObject<Grid>(name.c_str());


        int startCol = 1, startRow = 2;
        int regionW = 3, regionH = 3;

        for (int c = startCol; c < startCol + regionW; ++c) {
            for (int r = startRow; r < startRow + regionH; ++r) {
                int idx = c * m_totalRows + r;
                m_gameObjects[idx]->GetComponent<ColliderBase>()->SetOpen(true);
            }
        }
    }

    void TestScene::OnLeave() {
        //cout << "[TestScene] OnLeave()\n";
    }

    void TestScene::Update(float deltaTime) {
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
                sfx->Play();

                // 디버그: 소리 재생 로그
                /*std::wcout
                    << L"[DEBUG] SFX Play at mover: key="
                    << L", from (" << oldPos.x << L"," << oldPos.y
                    << L") to (" << newPos.x << L"," << newPos.y
                    << L")\n";*/
            }
        }

        bool left = InputManager::Instance().GetMouseState().leftPressed;
        bool right = InputManager::Instance().GetMouseState().rightPressed;
        auto mousePos = GetMouseWorldPos();

        for (auto& objPtr : m_gameObjects) {
            auto* gridPtr = dynamic_cast<Grid*>(objPtr.get());
            if (!gridPtr)
                continue;  // Grid 가 아니면 건너뛴다

            // 2) Grid 안에 있는 콜라이더만 꺼내서 처리
            auto* collider = gridPtr->GetComponent<JDComponent::ColliderBase>();
            if (!collider)
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
                
                // 새로운 UI오브젝트 생성하기
                // 
                // 우클릭: 열린 콜라이더만 동작
            }
        }
    }

    void TestScene::FixedUpdate(float fixedDeltaTime) {
        SceneBase::FixedUpdate(fixedDeltaTime);
        //cout << "[TestScene] FixedUpdate() - dt: " << fixedDeltaTime << "\n";
    }

    void TestScene::LateUpdate(float deltaTime) {
        SceneBase::LateUpdate(deltaTime);
        //cout << "[TestScene] LateUpdate() - dt: " << deltaTime << "\n";
    }

    void TestScene::Render(float deltaTime) {
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
}
