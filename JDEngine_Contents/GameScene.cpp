#include "pch.h"
#include "framework.h"
#include "SceneList.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "CombatSystem.h"
#include "WaveManager.h"

using namespace std;
using namespace JDGameObject::Content;
using JDComponent::AnimationRender;

namespace JDScene {

    // GameScene
    void GameScene::OnEnter() {
        using namespace JDGameObject;
        using namespace JDComponent;
        //======================================================
        m_showedDays.clear();
        m_elapsedTime = 0.0f;
        WaveManager::Instance().LoadStageWaves();

        // �ڿ� ����.
        auto& rs = ResourceSystem::Instance();
        rs.SetTotalResource(Resource(10000, 10000, 10000));
        rs.SetMaxPopulation(500);
        rs.SetCurPopulation(100);

        // ¡�� ��ư.
        auto noviceBtn = CreateUIObject<Button>(L"noviceBtn");
        noviceBtn->SetTextureName("GAME_START_B");
        noviceBtn->SetSizeToOriginal();
        noviceBtn->SetPosition({ 100.0f, 50.0f });
        noviceBtn->AddOnClick("Recruit Novice", [this]() {
            player.RecruitUnits(JDGameSystem::UnitType::Novice);
            std::cout << "Player Power: " << player.CalculateTotalPower() << std::endl;
            });

        //auto expertBtn = CreateUIObject<Button>(L"expertBtn");
        //expertBtn->SetTextureName("GAME_START_B");
        //expertBtn->SetSizeToOriginal();
        //expertBtn->SetPosition({ 100.0f, 150.0f });
        //expertBtn->AddOnClick("Recruit Expert", [this]() {
        //    player.RecruitUnits(JDGameSystem::UnitType::Expert);
        //    std::cout << "Player Power: " << player.CalculateTotalPower() << std::endl;
        //    });

        // ����.
        auto* barracksObj = CreateGameObject<Player>(L"barracksObj");

        barracksObj->SetTag(JDGlobal::Base::GameTag::Barracks);
        barracksObj->AddComponent<Editor_Clickable>();
        barracksObj->AddComponent<JDComponent::TextureRenderer>("Test", RenderLayerInfo{ SortingLayer::BackGround, 1 });
        auto bitmap = static_cast<ID2D1Bitmap*> (AssetManager::Instance().GetTexture("Test"));
        auto size = bitmap->GetSize();
        barracksObj->AddComponent<JDComponent::BoxCollider>(Vector2F{ size.width / 2.0f, size.height / 2.0f });
        barracksObj->GetComponent<JDComponent::BoxCollider>()->SetOpen(true);
        barracksObj->GetComponent<Transform>()->SetPosition({ -500.0f, 100.0f });

        m_barracksObject = barracksObj;
        //======================================================
    }

    void GameScene::OnLeave() {
        //cout << "[GameScene] OnLeave()\n";
    }

    void GameScene::Update(float deltaTime) {
        SceneBase::Update(deltaTime);

        // ��¥ ���� Ÿ�̸�
        m_elapsedTime += deltaTime;

        if (m_elapsedTime >= m_dayTime) {
            m_elapsedTime = 0.0f;
            WaveManager::Instance().AdvanceDay();

            const int currDay = WaveManager::Instance().GetCurrDay();
            const int warningDay = currDay + 2;

            std::cout << "[GameScene] ��¥ ����: " << currDay << std::endl;

            // ���̺갡 �ִ� ���, ���� ȭ�鿡 ��Ÿ���� �ʾ����� ����.
            if (WaveManager::Instance().GetWave(warningDay) && !IsEnemySpawned(warningDay)) {
                SpawnWaveEnemy();
                AddEnemyDay(warningDay);
                std::cout << "[GameScene] �� ������." << std::endl;
            }
        }
        ClickUpdate();
        MoveEnemyObjects(deltaTime);
        MovePlayerObjects(deltaTime);

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
                // ��Ŭ��: ���� �ݶ��̴��� ����
            }
        }
    }

    void GameScene::FixedUpdate(float fixedDeltaTime) {
        SceneBase::FixedUpdate(fixedDeltaTime);
        //cout << "[GameScene] FixedUpdate() - dt: " << fixedDeltaTime << "\n";
    }

    void GameScene::LateUpdate(float deltaTime) {
        SceneBase::LateUpdate(deltaTime);
    }

    void GameScene::Render(float deltaTime) {
        SceneBase::Render(deltaTime);
        auto camera = D2DRenderer::Instance().GetCamera();

        if (camera)
            D2DRenderer::Instance().SetTransform(camera->GetViewMatrix());
        else
            D2DRenderer::Instance().SetTransform(D2D1::Matrix3x2F::Identity());

        //���� ������Ʈ ����

        for (auto& obj : m_gameObjects) {
            D2DRenderer::Instance().RenderGameObject(*obj, deltaTime);
        }

        DrawColider();

        for (auto& uiObj : m_uiObjects)

        {
            D2DRenderer::Instance().RenderUIObject(*uiObj);
        }
    }

    void GameScene::SpawnWaveEnemy() {

        auto* enemyObj = CreateGameObject<Player>(L"enemyObj");

        enemyObj->SetTag(JDGlobal::Base::GameTag::EnemyMove);
        enemyObj->AddComponent<Editor_Clickable>();
        enemyObj->AddComponent<JDComponent::TextureRenderer>("Test", RenderLayerInfo{ SortingLayer::BackGround, 1 });
        auto bitmap = static_cast<ID2D1Bitmap*> (AssetManager::Instance().GetTexture("Test"));
        auto size = bitmap->GetSize();
        enemyObj->AddComponent<JDComponent::BoxCollider>(Vector2F{ size.width / 2.0f, size.height / 2.0f });
        enemyObj->GetComponent<JDComponent::BoxCollider>()->SetOpen(true);
        float windowWidth = JDGlobal::Window::WindowSize::Instance().GetWidth();
        enemyObj->GetComponent<Transform>()->SetPosition({ (windowWidth + size.width) / 2.0f, 100.0f });
        enemyObj->AddComponent<JDComponent::SFX>("Step");
    }

    void GameScene::SpawnPlayerArmy() {
        auto* playerObj = CreateGameObject<Player>(L"playerObj");
        playerObj->SetTag(JDGlobal::Base::GameTag::PlayerMove);
        playerObj->AddComponent<JDComponent::TextureRenderer>("Test", RenderLayerInfo{ SortingLayer::BackGround, 1 });

        auto bitmap = static_cast<ID2D1Bitmap*> (AssetManager::Instance().GetTexture("Test"));
        auto size = bitmap->GetSize();

        playerObj->AddComponent<JDComponent::BoxCollider>(Vector2F{ size.width / 2.0f, size.height / 2.0f });
        playerObj->GetComponent<JDComponent::BoxCollider>()->SetOpen(true);
        playerObj->GetComponent<Transform>()->SetPosition({ -400.0f, 100.0f });
        playerObj->AddComponent<JDComponent::SFX>("Step");

        if (!m_playerObject) m_playerObject = playerObj;
    }

    void GameScene::MoveEnemyObjects(float deltaTime)
    {
        const float speed = -100.0f; // �ȼ�/��
        const float stopX = -300.0f; // ���⼭ ����
        const Vector2F delta{ speed * deltaTime, 0.f };

        for (auto& objPtr : m_gameObjects)
        {
            if (!objPtr || objPtr->GetTag() != JDGlobal::Base::GameTag::EnemyMove)
                continue;

            auto* transform = objPtr->GetComponent<Transform>();
            auto* collider = objPtr->GetComponent<JDComponent::ColliderBase>();
            auto* sfx = objPtr->GetComponent<JDComponent::SFX>();
            if (!transform || !collider) continue;

            if (m_targetEnemy && objPtr.get() != m_targetEnemy)
            {
                if (m_playerObject->GetComponent<JDComponent::ColliderBase>()->Intersect(collider)) {
                    continue;
                }
            }

            Vector2F pos = transform->GetPosition();

            if (pos.x <= stopX)
            {
                objPtr->SetTag(JDGlobal::Base::GameTag::Enemy); 
                continue;
            }

            // �̵� ����
            Vector2F newPos = pos + delta;
            transform->SetPosition(newPos);

            if (sfx) sfx->Play();  // �̵� �� ȿ���� ���
        }
    }

    void GameScene::MovePlayerObjects(float deltaTime)
    {
        const float speed = 100.0f;
        const float stopX = 600.0f;
        const Vector2F delta{ speed * deltaTime, 0.f };

        for (auto& objPtr : m_gameObjects)
        {
            if (!objPtr || objPtr->GetTag() != JDGlobal::Base::GameTag::PlayerMove)
                continue;

            auto* transform = objPtr->GetComponent<Transform>();
            auto* collider = objPtr->GetComponent<JDComponent::ColliderBase>();
            auto* sfx = objPtr->GetComponent<JDComponent::SFX>();
            if (!transform || !collider) continue;

            // ���� �Ʊ��� �ƴ� �ٸ� �Ʊ��� ���� ���� �浹�ϸ� ����
            if (m_playerObject && objPtr.get() != m_playerObject)
            {
                if (m_targetEnemy->GetComponent<JDComponent::ColliderBase>()->Intersect(collider)) {
                    continue;
                }
            }

            // ���� �Ʊ��� ���, Ÿ�� ������ �浹 üũ
            if (objPtr.get() == m_playerObject && m_targetEnemy)
            {
                auto* enemyCollider = m_targetEnemy->GetComponent<JDComponent::ColliderBase>();
                if (enemyCollider && collider->Intersect(enemyCollider))
                {
                    m_playerObject->SetTag(JDGlobal::Base::GameTag::Player);
                    m_targetEnemy->SetTag(JDGlobal::Base::GameTag::Enemy);
                    return;
                }
            }

            Vector2F pos = transform->GetPosition();
            if (objPtr.get() == m_playerObject && pos.x >= stopX)
            {
                m_playerObject->SetTag(JDGlobal::Base::GameTag::Enemy);
                return;
            }

            Vector2F newPos = pos + delta;
            transform->SetPosition(newPos);

            if (sfx) sfx->Play();
        }
    }

    void GameScene::ClickUpdate()
    {
        auto camera = D2DRenderer::Instance().GetCamera();
        if (!camera) return;

        InputManager& input = InputManager::Instance();
        MouseState state = input.GetMouseState();

        if (ImGui::GetIO().WantCaptureMouse || !state.leftClicked)
            return;

        // ��ǥ ��ȯ
        float screenHeight = static_cast<float>(camera->GetScreenHeight());
        Vector2F screenMousePos(state.pos.x, state.pos.y);
        Vector2F unityMousePos(screenMousePos.x, screenHeight - screenMousePos.y);
        Vector2F worldMousePos = camera->ScreenToWorldPoint(unityMousePos);

        GameObjectBase* clickedObj = nullptr;

        // 1. UI ������Ʈ �˻� (��ũ�� ��ǥ��)
        for (int i = static_cast<int>(m_uiObjects.size()) - 1; i >= 0; --i)
        {
            auto& uiObj = m_uiObjects[i];
            if (!uiObj) continue;

            auto clickable = uiObj->GetComponent<Editor_Clickable>();
            if (clickable && clickable->IsHit(screenMousePos)) {
                clickedObj = uiObj.get();
                break;
            }
        }

        // 2. ���� ������Ʈ �˻� (���� ��ǥ��)
        if (!clickedObj)
        {
            for (int i = static_cast<int>(m_gameObjects.size()) - 1; i >= 0; --i)
            {
                auto& obj = m_gameObjects[i];
                if (!obj) continue;

                auto clickable = obj->GetComponent<Editor_Clickable>();
                if (clickable && clickable->IsHit(worldMousePos)) {
                    clickedObj = obj.get();
                    break;
                }
            }
        }

        SetSelectedObject(clickedObj);

        // Ŭ���� ����� ���ٸ�
        if (!clickedObj) {
            return;
        }

        // �±� ��� ó��
        auto* gameObj = dynamic_cast<GameObject*>(clickedObj);  
        if (!gameObj) return;

        auto tag = gameObj->GetTag();
        bool isEnemyTag = (tag == JDGlobal::Base::GameTag::Enemy || tag == JDGlobal::Base::GameTag::EnemyMove);
        
        if (tag == JDGlobal::Base::GameTag::Barracks) {
            m_isBarracksSelected = !m_isBarracksSelected;
            std::cout << "[GameScene] ���� Ŭ��. ���� ����: " << m_isBarracksSelected << std::endl;
        }
        else if (m_isBarracksSelected && ((isEnemyTag && !m_targetEnemy) || gameObj == m_targetEnemy)) {
            if (!m_targetEnemy) m_targetEnemy = gameObj;

            SpawnPlayerArmy();
            m_isBarracksSelected = false;
            std::cout << "[GameScene] ���� Ŭ�� -> �� Ŭ��." << std::endl;
        }
        else { m_isBarracksSelected = false; }
    }
}
