#include "pch.h"
#include "framework.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "TestScene.h"

using namespace std;
using namespace JDGameObject::Content;
using JDComponent::AnimationRender;

namespace JDScene {

    // TestScene
    void TestScene::OnEnter() {
        using namespace JDGameObject;
        using namespace JDComponent;

        //cout << "[TestScene] OnEnter()\n";

        CreateGameObject<Player>(L"Player");

        std::shared_ptr<GameObject> testObject = std::make_shared<GameObject>();
        std::shared_ptr<GameObject>  birdObj = std::make_shared<GameObject>();
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
                std::wstring name = L"Box_" + std::to_wstring(col) + L"_" + std::to_wstring(row);

                auto* box = CreateGameObject<Grid>(name.c_str());

                float x = startX + spacingX * col;
                float y = startY + spacingY * row;
                box->GetComponent<Transform>()->SetPosition({ x, y });

                box->AddComponent<BoxCollider>(Vector2F{ 47.0f,47.0f });

            }
        }

        auto* circObj = CreateGameObject<Player>(L"CircleObject");
        circObj->GetComponent<Transform>()->SetPosition({ 200.0f, 100.0f });
        circObj->AddComponent<CircleCollider>(50.0f);

        {//Text�̹���
            auto* boxObj2 = CreateGameObject<Player>(L"BoxObject2");
            boxObj2->GetComponent<Transform>()->SetPosition({ 100.0f, 100.0f });
            boxObj2->AddComponent<BGM>("MainTheme");
            boxObj2->AddComponent<TextureRenderer>("Test", RenderLayerInfo{ SortingLayer::BackGround, 1 });
            auto bitmap = static_cast<ID2D1Bitmap*> (AssetManager::Instance().GetTexture("Test"));
            auto size = bitmap->GetSize();
            boxObj2->AddComponent<BoxCollider>(Vector2F{ size.width / 2.0f, size.height / 2.0f });
        }
        {//Graybird�ִ�
            auto* boxObj3 = CreateGameObject<Player>(L"BoxObject3");
            boxObj3->GetComponent<Transform>()->SetPosition({ 100.0f, 100.0f });
            boxObj3->AddComponent<TextureRenderer>("Test", RenderLayerInfo{ SortingLayer::BackGround, 1 });
            boxObj3->AddComponent<AnimationRender>("GrayBird", 0.5, RenderLayerInfo{ SortingLayer::BackGround, 2 });

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

                // 위치 설정
                float x = startX + spacingX * col;
                float y = startY + spacingY * row;
                box->GetComponent<JDComponent::D2DTM::Transform>()->SetPosition({ x, y });

                // 콜라이더 추가
                box->AddComponent<JDComponent::BoxCollider>(Vector2F{ 47.0f,47.0f });
            }
        }
    }

    void TestScene::OnLeave() {
        //cout << "[TestScene] OnLeave()\n";
    }

    void TestScene::Update(float deltaTime) {
        SceneBase::Update(deltaTime);

        auto& input = InputManager::Instance();
        const float moveSpeed = 300.0f;

        auto tf = m_sceneObjects[0]->GetComponent<Transform>();

        auto pos = tf->GetPosition();
        if (input.IsKeyDown(VK_UP)) pos.y += moveSpeed * deltaTime;
        if (input.IsKeyDown(VK_DOWN)) pos.y -= moveSpeed * deltaTime;
        if (input.IsKeyDown(VK_LEFT)) pos.x -= moveSpeed * deltaTime;
        if (input.IsKeyDown(VK_RIGHT)) pos.x += moveSpeed * deltaTime;
        tf->SetPosition(pos);
        // 1) 마우스 스크린 좌표 얻기
        auto& inputMgr = InputManager::Instance();
        auto  mouseState = inputMgr.GetMouseState().pos;
        Vector2F screenPos{ static_cast<float>(mouseState.x), static_cast<float>(mouseState.y) };

        // 2) 스크린 → 월드 변환
        Vector2F mouseWorld;
        auto camera = D2DRenderer::Instance().GetCamera();
        if (camera) {
            auto invView = camera->GetViewMatrix();
            D2D1InvertMatrix(&invView);
            mouseWorld = m_camera->TransformPoint(invView, { screenPos.x,screenPos.y });
        }
        else {
            mouseWorld = screenPos;
        }

        //HandleMouseHover(mouseWorld);

        // 3) 하이라이트 인덱스 계산
        m_highlightedIndex = -1;

        for (int i = 0; i < (int)m_gameObjects.size(); ++i) {
            auto& obj = m_gameObjects[i];
            if (!obj) continue;

            auto* col = obj->GetComponent<JDComponent::BoxCollider>();
            if (!col) continue;

            auto* tm = obj->GetComponent<JDComponent::D2DTM::Transform>();
            auto  pos = tm->GetPosition();
            auto  hsize = col->GetHalfSize();
            auto  offset = col->GetColliderOffset();

            float left = pos.x + offset.x - hsize.x;
            float top = pos.y + offset.y + hsize.y;
            float right = pos.x + offset.x + hsize.x;
            float bottom = pos.y + offset.y - hsize.y;

            if (mouseWorld.x >= left && mouseWorld.x <= right &&
                mouseWorld.y <= top && mouseWorld.y >= bottom)
            {
                m_highlightedIndex = i;
                break;
            }
        }

        bool leftPressed = inputMgr.GetMouseState().leftPressed;
        if (leftPressed && !m_prevLeftPressed && m_highlightedIndex != -1) {
            std::cout << "[DEBUG] 클릭된 콜라이더 인덱스: "
                << m_highlightedIndex << std::endl;
        }
        m_prevLeftPressed = leftPressed;
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

        // ───────────────────────────────────────────────────────
        // (1) 기존 콜라이더 하이라이트용 DrawRectangle — 그대로!
        for (int i = 0; i < (int)m_gameObjects.size(); ++i) {
            auto& obj = m_gameObjects[i];
            if (!obj) continue;

            auto* col = obj->GetComponent<JDComponent::BoxCollider>();
            if (!col) continue;

            auto* tm = obj->GetComponent<JDComponent::D2DTM::Transform>();
            auto pos = tm->GetPosition();
            auto hsize = col->GetHalfSize();
            auto offset = col->GetColliderOffset();

            float left = pos.x + offset.x - hsize.x;
            float top = pos.y + offset.y + hsize.y;
            float right = pos.x + offset.x + hsize.x;
            float bottom = pos.y + offset.y - hsize.y;

            UINT32 color = (i == m_highlightedIndex)
                ? 0xFFFF0000
                : 0xFF000000;

            D2DRenderer::Instance().DrawRectangle(
                left, top, right, bottom,
                color
            );
        }
        // ───────────────────────────────────────────────────────

        // (2) 렌더 순서 정렬
        std::vector<std::shared_ptr<GameObject>> sorted = m_sceneObjects;

        std::sort(sorted.begin(), sorted.end(),
            [](const std::shared_ptr<GameObject>& a,
                const std::shared_ptr<GameObject>& b) {
                // ① AnimationRender 우선 탐색
                if (auto ar = a->GetComponent<AnimationRender>())
                    if (auto br = b->GetComponent<AnimationRender>())
                        return ar->GetLayerInfo() < br->GetLayerInfo();
                    else
                        return ar->GetLayerInfo() <
                        RenderLayerInfo{ SortingLayer::None, 0 };

                // ② TextureRenderer 비교
                if (auto tr = a->GetComponent<TextureRenderer>())
                    if (auto tb = b->GetComponent<TextureRenderer>())
                        return tr->GetLayerInfo() < tb->GetLayerInfo();
                    else
                        return tr->GetLayerInfo() <
                        RenderLayerInfo{ SortingLayer::None, 0 };

                // 그 외, 기본값
                return false;
            }
        );

        // (3) 게임 오브젝트 렌더 + 추가 콜라이더 드로우
        for (auto obj : sorted) {
            // 애니메이션 컴포넌트가 있으면
            if (auto ar = obj->GetComponent<AnimationRender>()) {
                // world matrix
                auto tf = obj->GetComponent<Transform>();
                D2D1_MATRIX_3X2_F world = tf->GetWorldMatrix();
                if (camera) world = world * camera->GetViewMatrix();
                ar->Render(
                    D2DRenderer::Instance().GetD2DContext(),
                    world
                );
                continue;
            }
            // 아니면 텍스처 렌더
            if (auto tr = obj->GetComponent<TextureRenderer>()) {
                auto tf = obj->GetComponent<Transform>();
                D2D1_MATRIX_3X2_F world = tf->GetWorldMatrix();
                if (camera) world = world * camera->GetViewMatrix();
                tr->Render(
                    D2DRenderer::Instance().GetD2DContext(),
                    world
                );
            }
        }

        // (4) UI 렌더
        for (auto& uiObj : m_UIObjects) {
            D2DRenderer::Instance().RenderUIObject(*uiObj);
        }
    }
*/
