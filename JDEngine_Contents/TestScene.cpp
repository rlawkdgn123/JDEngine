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

        {//Text
            auto* boxObj2 = CreateGameObject<Player>(L"BoxObject2");

            boxObj2->SetTag(JDGlobal::Base::GameTag::Mover);

            boxObj2->GetComponent<Transform>()->SetPosition({ 100.0f, 100.0f });
            
            boxObj2->AddComponent<TextureRenderer>("Test", RenderLayerInfo{ SortingLayer::BackGround, 1 });
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

            auto frames = AssetManager::Instance().GetAnimationRender("GrayBird");
            if (frames && !frames->frames.empty()) {
                auto first = frames->frames[0].srcRect;
                float width = first.right - first.left;
                float height = first.bottom - first.top;
                Vector2F halfSize{ width * 0.5f, height * 0.5f };
                birdObj->AddComponent<BoxCollider>(halfSize);
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
