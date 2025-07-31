#include "pch.h"
#include "framework.h"
#include "SceneList.h"
#include "BoxCollider.h"
#include "CircleCollider.h"

using namespace std;
using namespace JDGameObject::InGame;

namespace JDScene {

    // TestScene
    void TestScene::OnEnter() {
        using namespace JDGameObject;
        using namespace JDComponent;

        //cout << "[TestScene] OnEnter()\n";
        CreateGameObject<Player>(L"������");


        std::shared_ptr<GameObject> testObject = std::make_shared<GameObject>();
        std::shared_ptr<GameObject>  birdObj = std::make_shared<GameObject>();

        {//Test �ؽ�ó �̹��� ���ӿ�����Ʈ ����
            auto tf = testObject->GetComponent<Transform>();
            tf->SetPosition({ 0.f, 0.f });
            tf->SetScale({ 1.f, 1.f });

            testObject->AddComponent<SpriteRenderer>("Test");

            m_sceneObjects.push_back(testObject);
        }

        {//�� �ִϸ��̼� ���ӿ�����Ʈ ����
            auto birdTf = birdObj->GetComponent<Transform>();
            birdTf->SetPosition({ 100.f, 50.f });
            birdTf->SetScale({ 1.0f, 1.0f });

            birdObj->AddComponent<SpriteRenderer>("GrayBird");
            birdObj->AddComponent<Animation>("GrayBird", 0.5);// �ڿ� ���� speed

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
                // ���� �̸� ���� (���ϸ� ���ϸ����� �ص� �˴ϴ�)
                std::wstring name = L"Box_" + std::to_wstring(col) + L"_" + std::to_wstring(row);

                // GameObject ����
                auto* box = CreateGameObject<Grid>(name.c_str());

                // ��ġ ����
                float x = startX + spacingX * col;
                float y = startY + spacingY * row;
                box->GetComponent<JDComponent::D2DTM::Transform>()->SetPosition({ x, y });

                // �ݶ��̴� �߰�
                box->AddComponent<JDComponent::BoxCollider>(JDGlobal::Math::Vector2F{ 47.0f,47.0f });
            }
        }

        //�׽�Ʈ�� ���̶� �ڽ� �ݶ��̴� ���� ������Ʈ
        auto* circObj = CreateGameObject<Player>(L"CircleObject");
        circObj->GetComponent<JDComponent::D2DTM::Transform>()->SetPosition({ 200.0f, 100.0f });
        circObj->AddComponent<JDComponent::CircleCollider>(50.0f);

        auto* boxObj2 = CreateGameObject<Player>(L"BoxObject2");
        boxObj2->GetComponent<JDComponent::D2DTM::Transform>()->SetPosition({ 100.0f, 100.0f });
        boxObj2->AddComponent<SpriteRenderer>("Test");
        auto bitmap = static_cast<ID2D1Bitmap*> (AssetManager::Instance().GetTexture("Test"));
        auto size = bitmap->GetSize();
        boxObj2->AddComponent<JDComponent::BoxCollider>(JDGlobal::Math::Vector2F{ size.width / 2.0f, size.height / 2.0f });
    }

    void TestScene::OnLeave() {
        //cout << "[TestScene] OnLeave()\n";
    }

    void TestScene::Update(float deltaTime) {
        SceneBase::Update(deltaTime);
        //m_gameObjects.back()->GetComponent<JDComponent::D2DTM::Transform>()->Translate(1, 0);
        // 1) ���콺 ��ũ�� ��ǥ ���
        //auto& inputMgr = InputManager::Instance();
        //auto  mouseState = inputMgr.GetMouseState().pos;   
        //JDGlobal::Math::Vector2F screenPos{ static_cast<float>(mouseState.x), static_cast<float>(mouseState.y) };

        //// 2) ��ũ�� �� ���� ��ȯ
        //JDGlobal::Math::Vector2F mouseWorld;
        //auto camera = D2DRenderer::Instance().GetCamera();
        //if (camera) {
        //    auto invView = camera->GetViewMatrix();
        //    D2D1InvertMatrix(&invView); 
        //    mouseWorld = m_camera->TransformPoint(invView, { screenPos.x,screenPos.y });
        //}
        //else {
        //    mouseWorld = screenPos;
        //}

        ////HandleMouseHover(mouseWorld);

        //// 3) ���̶���Ʈ �ε��� ���
        //m_highlightedIndex = -1;

        //for (int i = 0; i < (int)m_gameObjects.size(); ++i) {
        //    auto& obj = m_gameObjects[i];
        //    if (!obj) continue;

        //    auto* col = obj->GetComponent<JDComponent::BoxCollider>();
        //    if (!col) continue;

        //    auto* tm = obj->GetComponent<JDComponent::D2DTM::Transform>();
        //    auto  pos = tm->GetPosition();
        //    auto  hsize = col->GetHalfSize();
        //    auto  offset = col->GetColliderOffset();

        //    float left = pos.x + offset.x - hsize.x;
        //    float top = pos.y + offset.y + hsize.y;
        //    float right = pos.x + offset.x + hsize.x;
        //    float bottom = pos.y + offset.y - hsize.y;
        //    
        //    if (mouseWorld.x >= left && mouseWorld.x <= right &&
        //        mouseWorld.y <= top && mouseWorld.y >= bottom)
        //    {
        //        m_highlightedIndex = i;
        //        break;
        //    }
        //}

        //bool leftPressed = inputMgr.GetMouseState().leftPressed;
        //if (leftPressed && !m_prevLeftPressed && m_highlightedIndex != -1) {
        //    std::cout << "[DEBUG] Ŭ���� �ݶ��̴� �ε���: "
        //        << m_highlightedIndex << std::endl;
        //}
        //m_prevLeftPressed = leftPressed;
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

        DrawColider();

        //���� ������Ʈ ����
       //for (auto& obj : m_sceneObjects) {
       //    D2DRenderer::Instance().RenderGameObject(*obj, deltaTime);
       //}
        for (auto& obj : m_gameObjects) {
            D2DRenderer::Instance().RenderGameObject(*obj, deltaTime);
        }

        //for (auto& uiObj : m_UIObjects)
        //{
        //    D2DRenderer::Instance().RenderUIObject(*uiObj);
        //}

        for (auto& uiObj : m_gameUiObjects)
        {
            D2DRenderer::Instance().RenderUIObject(*uiObj);
        }
    }
}
