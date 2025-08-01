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
        circObj->AddComponent<Editor_Clickable>();

        {//Text�̹���
            auto* boxObj2 = CreateGameObject<Player>(L"BoxObject2");
            boxObj2->GetComponent<Transform>()->SetPosition({ 100.0f, 100.0f });
            boxObj2->AddComponent<BGM>("MainTheme");
            boxObj2->AddComponent<TextureRenderer>("Test", RenderLayerInfo{ SortingLayer::BackGround, 1 });
            boxObj2->AddComponent<Editor_Clickable>();
            auto bitmap = static_cast<ID2D1Bitmap*> (AssetManager::Instance().GetTexture("Test"));
            auto size = bitmap->GetSize();
            boxObj2->AddComponent<BoxCollider>(Vector2F{ size.width / 2.0f, size.height / 2.0f });
        }

        {//Graybird�ִ�
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
        bool leftPressed = InputManager::Instance().GetMouseState().leftPressed;
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
