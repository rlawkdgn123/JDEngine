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
using JDComponent::D2DTM::RectTransform;
using JDComponent::TextureRenderer;

namespace JDScene {

    // GameScene
    void GameScene::OnEnter() {
        using namespace JDGameObject;
        using namespace JDComponent;

        //////////////////////////////////////////////////
        CreateMap();
        m_defaultUI->SetActive(false);
        //////////////////////////////////////////////////

        m_showedDays.clear();
        m_elapsedTime = 0.0f;
        WaveManager::Instance().LoadStageWaves();

        // �ڿ� ����.
        auto& rs = ResourceSystem::Instance();
        rs.SetTotalResource(Resource(10000, 10000, 10000));
        rs.SetMaxPopulation(500);
        rs.SetCurPopulation(100);

        m_playerArmy.OverrideUnitCounts({100, 100});

        // ¡�� ��ư.
        CreateUIButton( L"noviceBtn", { 0.0f, -300.0f }, "GAME_START_B", "Recruit Novice",
            [this]() { m_playerArmy.RecruitUnits(JDGameSystem::UnitType::Novice);
                std::cout << "Player Power: " << m_playerArmy.CalculateTotalPower() << std::endl; });

        CreateUIButton( L"expertBtn", { 0.0f, -400.0f }, "GAME_START_B", "Recruit Expert",
            [this]() { m_playerArmy.RecruitUnits(JDGameSystem::UnitType::Expert);
                std::cout << "Player Power: " << m_playerArmy.CalculateTotalPower() << std::endl; });


        // ����.
        m_barracksObject = CreateStructure(L"barracksObj", JDGlobal::Base::GameTag::Barracks, { -500.0f, 100.0f }, "house");

        // ����.
        m_wallObject = CreateStructure(L"wallObj", JDGlobal::Base::GameTag::Wall, { -800.0f, 100.0f }, "house");

        { // �׽�Ʈ. UI�� �ʿ��� ������ ���Ӿ����� ��� ����������.

            // 1. �� ������ �ڿ�, ������ ����.
            const JDGlobal::Contents::UnitTypeData& noviceData = m_playerArmy.GetUnitTypes()[0];
            const JDGlobal::Contents::UnitTypeData& expertData = m_playerArmy.GetUnitTypes()[1]; 

            Resource noviceCost = noviceData.GetRecruitCost(); // novice ���.
            int novicePower = noviceData.GetPower();           // novice ������.

            Resource expertCost = expertData.GetRecruitCost(); // expert ���.
            int expertPower = expertData.GetPower();           // expert ������.

            // 2. ���� Ŭ�� ����.
            if (m_isBarracksSelected) {
                // �̷��� ���� �� ��.
            }

            // 3. ��¥ ����. 
            int currDay = WaveManager::Instance().GetCurrDay(); // ���� ��¥.

            // 4. ���̺� ����.
            const WaveData* nextWave = WaveManager::Instance().GetWave(1); // �̷������� �ش� ��¥�� ������ ������ �� ����.

            // 5. ����� ��ġ�� ������ ǥ��. 
            /*for (auto& objPtr : m_gameObjects)
            {
                if (!objPtr || objPtr->GetTag() != JDGlobal::Base::GameTag::Player &&
                    objPtr->GetTag() != JDGlobal::Base::GameTag::Enemy)
                    continue;
                // �Ʊ��� ���� ��ġ ��������.
                auto* transform = objPtr->GetComponent<Transform>();
                if (!transform) continue;
                Vector2F position = transform->GetPosition();

                // ������ ��������.
                auto* soldier = dynamic_cast<Soldier*>(objPtr.get());
                if (soldier) {
                    int power = soldier->GetPower();
                }
            }*/

        }
        /////////////////////////////////////////////////////////////////////////////
        AudioManager::Instance().PlayBGM("BGM_Battle", &bgmChannel);
    }

    void GameScene::OnLeave() {
        //cout << "[GameScene] OnLeave()\n";
        if (bgmChannel) {
            bgmChannel->stop(); // FMOD���� ä���� ����
            bgmChannel = nullptr; // �����͵� �ʱ�ȭ (����)
        }
    }

    void GameScene::Update(float deltaTime) {
        SceneBase::Update(deltaTime);

        ProcessDayTimer(deltaTime); // ��¥ �� ���̺� ����.
        ProcessBattle(deltaTime); // ���� ���ӽð� �� ���� ��� ����. 

        ClickUpdate();
        MoveEnemyObjects(deltaTime); // �� �̵�.
        MovePlayerObjects(deltaTime); // �Ʊ� �̵�.

        AttackWall(deltaTime); // ���� �� �����ϴ� �� ����.

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

    void GameScene::ProcessDayTimer(float deltaTime) {
        m_elapsedTime += deltaTime;
        if (m_battleObject) m_btlElapsedTime += deltaTime;

        if (m_elapsedTime >= m_dayTime) {
            m_elapsedTime = 0.0f;
            WaveManager::Instance().AdvanceDay();

            const int currDay = WaveManager::Instance().GetCurrDay();
            //const int warningDay = currDay + 2;
            const int warningDay = currDay; // ���Ͽ� ȭ�鿡 ��Ÿ����.

            std::cout << "[GameScene] ��¥ ����: " << currDay << std::endl;

            // ���̺갡 �ִ� ���, ���� ȭ�鿡 ��Ÿ���� �ʾ����� ����.
            if (WaveManager::Instance().GetWave(warningDay) && !IsEnemySpawned(warningDay)) {
                m_enemyArmy.OverrideUnitCounts(WaveManager::Instance().GetWave(warningDay)->enemyUnits);
                float windowWidth = JDGlobal::Window::WindowSize::Instance().GetWidth();
                SpawnWaveEnemy({ (windowWidth + 100.0F) / 2.0f, 100.0f });
                AddEnemyDay(warningDay);
                std::cout << "[GameScene] �� ������." << std::endl;
            }
        }
    }

    void GameScene::ProcessBattle(float deltaTime) {

        if (m_btlElapsedTime < m_battleTime) return;
        
        JDGameSystem::CombatSystem combat;
        JDGameSystem::UnitCounts playerResult;
        JDGameSystem::UnitCounts enemyResult;

        combat.ResolveBattle(
            m_playerBattleArmy,
            m_playerTotalPower,
            m_enemyBattleArmy,
            m_enemyTotalPower,
            playerResult,
            enemyResult
        );

        m_playerBattleArmy = { 0 , 0 };
        m_playerTotalPower = 0;
        m_enemyBattleArmy = { 0 , 0 };
        m_enemyTotalPower = 0;

        if (playerResult.Total() > 0) {
            auto* playerObj = CreateSoldierUnit(playerResult, JDGlobal::Base::GameTag::Player, 
                JDGlobal::Contents::State::Back, m_battleObject->GetComponent<Transform>()->GetPosition(), "f1");
        }
        if (enemyResult.Total() > 0) {
            auto* enemyObj = CreateSoldierUnit(enemyResult, JDGlobal::Base::GameTag::Enemy, 
                JDGlobal::Contents::State::Move, m_battleObject->GetComponent<Transform>()->GetPosition(), "f1");

        }

        m_btlElapsedTime = 0.0f;
        SafeDestroy(m_battleObject);
        std::cout << "[GameScene] ���� ��." << std::endl;
        

    }

    void GameScene::SpawnWaveEnemy(const Vector2F& pos) {
        auto* enemyObj = CreateSoldierUnit(m_enemyArmy.GetUnitCounts(), JDGlobal::Base::GameTag::Enemy, 
            JDGlobal::Contents::State::Move, pos, "f1");

        m_enemyArmy.OverrideUnitCounts({ 0, 0 });
    }

    void GameScene::SpawnPlayerArmy(const Vector2F& pos) {
        auto* playerObj = CreateSoldierUnit(m_playerArmy.GetUnitCounts(), JDGlobal::Base::GameTag::Player, 
            JDGlobal::Contents::State::Move, pos, "f1");
        m_playerArmy.OverrideUnitCounts({ 0, 0 });
        if (!m_playerObject && !m_battleObject) m_playerObject = playerObj;
    }

    void GameScene::SpawnBattleObject(const Vector2F& pos)
    {
        auto* battleObj = CreateGameObject<Player>(L"battleObj");
        battleObj->SetTag(JDGlobal::Base::GameTag::BattleAnim);
        battleObj->AddComponent<Editor_Clickable>();
        battleObj->AddComponent<JDComponent::TextureRenderer>("Test", RenderLayerInfo{ SortingLayer::BackGround, 1 });

        auto bitmap = static_cast<ID2D1Bitmap*> (AssetManager::Instance().GetTexture("Test"));
        auto size = bitmap->GetSize();

        battleObj->AddComponent<JDComponent::BoxCollider>(Vector2F{ size.width / 2.0f, size.height / 2.0f });
        battleObj->GetComponent<JDComponent::BoxCollider>()->SetOpen(true);
        battleObj->GetComponent<Transform>()->SetPosition(pos);

        if (!m_battleObject) {
            m_battleObject = battleObj;
        }
    }

    void GameScene::MoveEnemyObjects(float deltaTime)
    {
        const float speed = 100.0f; // �ȼ�/��
        Vector2F wallDir = { -1.0f, 0.0f }; // �⺻ ����. (������ ���ٸ�)

        for (auto& objPtr : m_gameObjects)
        {
            if (!objPtr || objPtr->GetTag() != JDGlobal::Base::GameTag::Enemy || 
                objPtr->GetState() != JDGlobal::Contents::State::Move) continue;

            auto* transform = objPtr->GetComponent<Transform>();
            auto* sfx = objPtr->GetComponent<JDComponent::SFX>();
            if (!transform) continue;

            // ���� ���� ������Ʈ�� �浹�ϸ� ���� �ð� �ʱ�ȭ�ϰ� ����.
            if (m_battleObject) {
                auto* battleTm = m_battleObject->GetComponent<Transform>();
                if (!battleTm)  continue;
                Vector2F diffPos = battleTm->GetPosition() - transform->GetPosition();

                if (diffPos.Length() <= 10.0f) {
                    auto* soldier = dynamic_cast<JDGameObject::Content::Soldier*>(objPtr.get());
                    if (soldier) {
                        m_enemyBattleArmy += soldier->GetUnitCounts();
                        m_enemyTotalPower += soldier->GetPower();
                        std::cout << "[GameScene] �� ���� �߰�." << std::endl;
                    }
                    SafeDestroy(objPtr.get());
                    m_btlElapsedTime = 0.0f;
                    continue;
                }
            }

            // �����̶� �浹�ϸ� ���߰� ������ ������.
            if (m_wallObject) {
                auto* wallTm = m_wallObject->GetComponent<Transform>();
                if (!wallTm)  continue;
                Vector2F diffPos = wallTm->GetPosition() - transform->GetPosition();

                if (diffPos.Length() <= 10.0f) {
                    objPtr->SetState(JDGlobal::Contents::State::Idle);

                    auto it = std::find_if(m_attackers.begin(), m_attackers.end(),
                       [&](auto& a) { return a.enemy == objPtr.get(); });
                    if (it == m_attackers.end()) { m_attackers.push_back({ objPtr.get(), 0.0f }); }
                    continue;
                }
                wallDir = diffPos.Normalized();
            }

            // �̵� ����.
            Vector2F delta = wallDir * (speed * deltaTime);
            Vector2F newPos = transform->GetPosition() + delta;
            transform->SetPosition(newPos);

            if (sfx) sfx->Play();  // �̵� �� ȿ���� ���
        }
    }

    void GameScene::MovePlayerObjects(float deltaTime) {
        const float speed = 100.0f;
        const float backSpeed = 100.0f;
        Vector2F delta;

        for (auto& objPtr : m_gameObjects)
        {
            if (!objPtr || objPtr->GetTag() != JDGlobal::Base::GameTag::Player ||
                (objPtr->GetState() != JDGlobal::Contents::State::Move &&
                    objPtr->GetState() != JDGlobal::Contents::State::Back))
                continue;

            auto* transform = objPtr->GetComponent<Transform>();
            auto* sfx = objPtr->GetComponent<JDComponent::SFX>();
            if (!transform) continue;

            // �Ʊ��� ���� ������Ʈ�� �浹�ϸ� ���� �ð� �ʱ�ȭ�ϰ� ����.
            if (m_battleObject) {
                auto* battleTm = m_battleObject->GetComponent<Transform>();
                if (!battleTm) continue;
                Vector2F diffPos = battleTm->GetPosition() - transform->GetPosition();

                if (diffPos.Length() <= 10.0f) {
                    auto* soldier = dynamic_cast<JDGameObject::Content::Soldier*>(objPtr.get());
                    if (soldier) {
                        m_playerBattleArmy += soldier->GetUnitCounts();
                        m_playerTotalPower += soldier->GetPower();
                        std::cout << "[GameScene] �Ʊ� ���� �߰�." << std::endl;
                    }
                    SafeDestroy(objPtr.get());
                    m_btlElapsedTime = 0.0f;
                    continue;
                }
            }

            // ���� �Ʊ��� ���, Ÿ�� ������ �浹 üũ. �浹�ϸ� ����.
            if (objPtr.get() == m_playerObject && m_targetEnemy && m_playerObject) {
                auto* enemyTm = m_targetEnemy->GetComponent<Transform>();
                if (!enemyTm) continue;
                Vector2F diffPos = enemyTm->GetPosition() - transform->GetPosition();

                if (diffPos.Length() <= 10.0f) {
                    Vector2F pPos = transform->GetPosition();
                    Vector2F ePos = enemyTm->GetPosition();
                    Vector2F mid = { (pPos.x + ePos.x) * 0.5f, (pPos.y + ePos.y) * 0.5f };
                    auto* player = dynamic_cast<JDGameObject::Content::Soldier*>(objPtr.get());
                    if (player) {
                        m_playerBattleArmy += player->GetUnitCounts();
                        m_playerTotalPower += player->GetPower();
                        std::cout << "[GameScene] �Ʊ� ���� �߰�." << std::endl;
                    }
                    auto* enemy = dynamic_cast<JDGameObject::Content::Soldier*>(m_targetEnemy);
                    if (enemy) {
                        m_enemyBattleArmy += enemy->GetUnitCounts();
                        m_enemyTotalPower += enemy->GetPower();
                        std::cout << "[GameScene] �� ���� �߰�." << std::endl;
                    }

                    SafeDestroy(m_playerObject);
                    SafeDestroy(m_targetEnemy);

                    SpawnBattleObject(mid);
                    return;
                }
            }

            // ���� ������Ʈ�� ����, �Ϲ� �Ʊ��̸� ���� ���·� ��ȯ
            if (!m_battleObject && objPtr.get() != m_playerObject) {
                if (!m_barracksObject) continue;
                auto* texRenderer = objPtr->GetComponent<TextureRenderer>();
                if (texRenderer) {
                    texRenderer->SetFlipX(true); // �¿� ����!
                }
                auto* barracksTm = m_barracksObject->GetComponent<Transform>();
                if (!barracksTm) continue;
                Vector2F diffPos = barracksTm->GetPosition() - transform->GetPosition();

                if (diffPos.Length() <= 10.0f) {
                    auto* soldier = dynamic_cast<JDGameObject::Content::Soldier*>(objPtr.get());
                    if (soldier) {
                        JDGameSystem::UnitCounts updated = soldier->GetUnitCounts();
                        updated += m_playerArmy.GetUnitCounts();
                        m_playerArmy.OverrideUnitCounts(updated);
                        std::cout << "[GameScene] ���� �շ�." << std::endl;
                    }
                    SafeDestroy(objPtr.get());
                    continue;
                }

                objPtr->SetState(JDGlobal::Contents::State::Back);
            }

            Vector2F direction = { 1.0f, 0.0f }; // �⺻��.
            float moveSpeed = speed;

            if (objPtr->GetState() == JDGlobal::Contents::State::Back) {
                // ���� �������� ����
                if (m_barracksObject) {
                    auto* barracksTm = m_barracksObject->GetComponent<Transform>();
                    if (barracksTm) {
                        Vector2F diff = barracksTm->GetPosition() - transform->GetPosition();
                        direction = diff.Normalized();
                        moveSpeed = backSpeed;
                    }
                }
            }
            else if (objPtr.get() == m_playerObject && m_targetEnemy) {
                // �� ���� �������� �̵�
                auto* enemyTm = m_targetEnemy->GetComponent<Transform>();
                if (enemyTm) {
                    Vector2F diff = enemyTm->GetPosition() - transform->GetPosition();
                    direction = diff.Normalized();
                }
            }
            else if (m_battleObject) {
                // ���� ������Ʈ �������� �̵�
                auto* battleTm = m_battleObject->GetComponent<Transform>();
                if (battleTm) {
                    Vector2F diff = battleTm->GetPosition() - transform->GetPosition();
                    direction = diff.Normalized();
                }
            }

            delta = direction * (moveSpeed * deltaTime);
            Vector2F newPos = transform->GetPosition() + delta;
            transform->SetPosition(newPos);

            if (sfx) sfx->Play();
        }
    }

    void GameScene::AttackWall(float deltaTime) {
        for (auto it = m_attackers.begin(); it != m_attackers.end();) {
            it->timer += deltaTime;
            if (it->enemy && it->timer >= m_battleTime) {
                auto* soldier = dynamic_cast<JDGameObject::Content::Soldier*>(it->enemy);
                if (soldier) {
                    int dmg = soldier->GetPower();
                    m_wallHealth = std::max(0, m_wallHealth - dmg);
                    std::cout << "[GameScene] ���� ���� ü��: " << m_wallHealth << std::endl;
                    it->timer -= m_battleTime;
                }
            }

            // ��Ȱ��ȭ�� ���� ��Ͽ��� ����
            if (!it->enemy || !it->enemy->IsActive()) {
                it = m_attackers.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    void GameScene::ClickUpdate()
    {
        // ImGui�� ���콺 �Է��� ��� ���̸� ���� �� Ŭ���� �����մϴ�.
        if (ImGui::GetIO().WantCaptureMouse)
            return;

        InputManager& input = InputManager::Instance();
        MouseState state = input.GetMouseState();

        // state.leftClicked �Ǵ� state.leftPressed �� �ʿ��� �Է� ���¸� ����մϴ�.
        if (state.leftClicked)
        {
            // (1) ���콺 ��ǥ�� �� ������ �� ���� ����ؼ� �����մϴ�.
            // UI Ŭ�� ������ ����� ��ũ�� ��ǥ (D2D ����: Y �Ʒ��� ���)
            Vector2F screenMousePos(
                static_cast<float>(state.pos.x),
                static_cast<float>(state.pos.y)
            );
            // ���� ������Ʈ Ŭ�� ������ ����� ���� ��ǥ (Unity ����: Y ���� ���)
            Vector2F worldMousePos = GetMouseWorldPos(); // �츮�� ���� ���ϵ� �Լ� ���!

            bool clicked = false;

            ////////////////////////////////////////////////////////////////////////////////
            // 1. UI Ŭ�� �˻� (��ũ�� ��ǥ�� ���)
            ////////////////////////////////////////////////////////////////////////////////
            // �� ������ ��ũ�� ��ǥ�� ����ϹǷ� ������ �����ϰ� �ùٸ��� �����մϴ�.
            for (int i = static_cast<int>(m_uiObjects.size()) - 1; i >= 0; --i)
            {
                auto& uiObj = m_uiObjects[i];
                if (!uiObj || !uiObj->IsActive()) continue;

                auto clickable = uiObj->GetComponent<Editor_Clickable>();
                // UI�� IsHit �Լ����� '��ũ�� ��ǥ'�� �״�� �Ѱ��ݴϴ�.
                if (clickable && clickable->IsHit(screenMousePos))
                {
                    SetSelectedObject(uiObj.get());
                    clicked = true;
                    std::cout << " UI ������Ʈ Ŭ�� ��!!!!! ";
                    break; // UI�� Ŭ�������� �� �̻� ���� �� ��
                }
            }

            ////////////////////////////////////////////////////////////////////////////////
            // 2. ���ӿ�����Ʈ Ŭ�� �˻� (���� ��ǥ�� ���)
            ////////////////////////////////////////////////////////////////////////////////
            if (!clicked)
            {
                // (2) ���ʿ��ϰ� �߸��� ��ǥ ��ȯ ������ ��� �����մϴ�.
                /* Vector2F unityMousePos(mousePos.x, screenHeight - mousePos.y);
                    Vector2F worldMousePos = camera->ScreenToWorldPoint(unityMousePos);
                    -> �� �κ��� GetMouseWorldPos()�� ��ü�Ǿ����Ƿ� ����!
                */

                for (int i = static_cast<int>(m_gameObjects.size()) - 1; i >= 0; --i)
                {
                    auto& obj = m_gameObjects[i];
                    if (!obj || !obj->IsActive()) continue;

                    auto clickable = obj->GetComponent<Editor_Clickable>();
                    // (3) ���� ������Ʈ�� IsHit �Լ����� ������ ����� '���� ��ǥ'�� �Ѱ��ݴϴ�.
                    if (clickable && clickable->IsHit(worldMousePos))
                    {
                        SetSelectedObject(obj.get());
                        clicked = true;
                        std::cout << " ���� ������Ʈ Ŭ�� ��!!!!! ";

                        // �±� ��� ó��
                        auto* gameObj = GetSelectedObject();
                        if (!gameObj) return;

                        auto tag = gameObj->GetTag();
                        bool isEnemyTag = (tag == JDGlobal::Base::GameTag::Enemy);

                        if (tag == JDGlobal::Base::GameTag::Barracks) {
                            m_isBarracksSelected = !m_isBarracksSelected;
                            std::cout << "[GameScene] ���� Ŭ��. ���� ����: " << m_isBarracksSelected << std::endl;
                        }
                        else if (m_isBarracksSelected && !m_battleObject && (isEnemyTag && !m_targetEnemy) && m_playerArmy.GetTotalUnits() > 0) {
                            m_targetEnemy = gameObj;
                            SpawnPlayerArmy({ -400.0f, 100.0f });
                            //m_isBarracksSelected = false;
                            std::cout << "[GameScene] ���� Ŭ�� -> �� Ŭ��." << std::endl;
                        }
                        else if (m_isBarracksSelected && tag == JDGlobal::Base::GameTag::BattleAnim && m_playerArmy.GetTotalUnits() > 0) {
                            SpawnPlayerArmy({ -400.0f, 100.0f });
                            //m_isBarracksSelected = false;
                            std::cout << "[GameScene] ���� Ŭ�� -> ������ Ŭ��." << std::endl;
                        }
                        else {
                            //m_isBarracksSelected = false; 
                            std::cout << "[GameScene] �̻��� �� Ŭ��." << std::endl;
                        }
                        break;
                    }
                }
            }

            // �ƹ��͵� Ŭ������ �ʾҴٸ� ���� ����
            if (!clicked)
            {
                SetSelectedObject(nullptr);
            }
        }
    }

    void GameScene::SafeDestroy(GameObjectBase* obj)
    {
        if (!obj) return;

        if (m_targetEnemy == obj) m_targetEnemy = nullptr;
        if (m_playerObject == obj) m_playerObject = nullptr;
        if (m_barracksObject == obj) m_barracksObject = nullptr;
        if (m_battleObject == obj) m_battleObject = nullptr;
        if (m_wallObject == obj) m_wallObject = nullptr;
        if (GetSelectedObject() == obj) SetSelectedObject(nullptr);

        m_attackers.erase(
            std::remove_if(m_attackers.begin(), m_attackers.end(),
                [&](auto& a) { return a.enemy == obj; }),
            m_attackers.end()
        );

        SceneBase::DestroyObject(obj);
    }

    // ���� ���� ���� �Լ�
    JDGameObject::GameObjectBase* GameScene::CreateSoldierUnit( const JDGameSystem::UnitCounts& units, JDGlobal::Base::GameTag tag,
        JDGlobal::Contents::State state, const Vector2F& pos, const std::string& textureName) {
        auto* obj = CreateGameObject<Soldier>((tag == JDGlobal::Base::GameTag::Player) ? L"playerObj" : L"enemyObj");
        obj->SetUnitCounts(units);

        JDGameSystem::ArmySystem army;
        army.OverrideUnitCounts(units);
        obj->SetPower(army.CalculateTotalPower());

        obj->SetTag(tag);
        obj->SetState(state);
        obj->AddComponent<JDComponent::TextureRenderer>(textureName, RenderLayerInfo{ SortingLayer::BackGround, 1 });

        auto bitmap = static_cast<ID2D1Bitmap*>(AssetManager::Instance().GetTexture(textureName));
        Vector2F size = { bitmap->GetSize().width / 2.0f, bitmap->GetSize().height / 2.0f };

        obj->AddComponent<JDComponent::BoxCollider>(size);
        obj->GetComponent<JDComponent::BoxCollider>()->SetOpen(true);
        obj->GetComponent<Transform>()->SetPosition(pos);
        obj->AddComponent<JDComponent::SFX>("Step");

        if (tag == JDGlobal::Base::GameTag::Enemy) {
            obj->AddComponent<Editor_Clickable>();
            auto* texRenderer = obj->GetComponent<TextureRenderer>();
            if (texRenderer) {
                texRenderer->SetFlipX(true); // �¿� ����!
            }
        }

        return obj;
    }

    // ���� ������ ���� �Լ�
    JDGameObject::GameObjectBase* GameScene::CreateStructure( const std::wstring& name, JDGlobal::Base::GameTag tag, 
        const Vector2F& pos, const std::string& textureName) {
        auto* obj = CreateGameObject<GameObject>(name);
        obj->SetTag(tag);
        obj->AddComponent<Editor_Clickable>();
        obj->AddComponent<JDComponent::TextureRenderer>(textureName, RenderLayerInfo{ SortingLayer::BackGround, 1 });

        auto bitmap = static_cast<ID2D1Bitmap*>(AssetManager::Instance().GetTexture(textureName));
        Vector2F size = { bitmap->GetSize().width / 2.0f, bitmap->GetSize().height / 2.0f };

        obj->AddComponent<JDComponent::BoxCollider>(size);
        obj->GetComponent<JDComponent::BoxCollider>()->SetOpen(true);
        obj->GetComponent<Transform>()->SetPosition(pos);

        return obj;
    }

    // ���� ��ư ���� �Լ�
    JDGameObject::GameObjectBase* GameScene::CreateUIButton( const std::wstring& name, const Vector2F& pos,
        const std::string& textureName, const std::string& clickEventName, std::function<void()> callback) {
        auto* btn = CreateUIObject<Button>(name);
        btn->SetTextureName(textureName);
        btn->SetSizeToOriginal();
        btn->SetPosition(pos);
        btn->AddOnClick(clickEventName, callback);

        return btn;
    }

    void GameScene::CreateMap()
    {
        using namespace JDGameObject;
        using namespace JDComponent;
        using JDScene::TutorialScene;

        // =====================================================================
        // ���� ������Ʈ ���� (World Space)
        // =====================================================================

        // ���
        auto* map_backGround = CreateGameObject<GameObject>(L"Map_Background");
        map_backGround->AddComponent<Transform>()->SetPosition(Vector2F{ 0.f, 0.f });
        auto map_backGroundTexture = map_backGround->AddComponent<TextureRenderer>("BACK_GROUND_OPACITY", RenderLayerInfo{ SortingLayer::BackGround, 1 });
        map_backGroundTexture->SetSize({ 3840, 2160 });

        // ��� Ǯ ��
        auto* map_battle = CreateGameObject<GameObject>(L"Map_Field");
        map_battle->AddComponent<Transform>()->SetPosition(Vector2F{ 0.f, 0.f });
        auto map_battleTexture = map_battle->AddComponent<TextureRenderer>("ART_BattleMap01", RenderLayerInfo{ SortingLayer::BackGround, 1 });
        // auto map_battleTexture = map_battle->AddComponent<TextureRenderer>("BATTLE_MAP(old)", RenderLayerInfo{ SortingLayer::BackGround, 1 });
        map_battleTexture->SetSize({ JDGlobal::Window::WindowSize::Instance().GetSize() });

        // Ǯ �� _ ���̾� 1
        auto* map_battleLayer01 = CreateGameObject<GameObject>(L"Map_FieldLayer01");
        map_battleLayer01->AddComponent<Transform>()->SetPosition(Vector2F{ 0.f, 0.f });
        auto map_battleLayer01Texture = map_battleLayer01->AddComponent<TextureRenderer>("ART_BattleMapLayer01", RenderLayerInfo{ SortingLayer::BackGround, 1 });
        map_battleLayer01Texture->SetSize({ JDGlobal::Window::WindowSize::Instance().GetSize() });

        // ����
        auto* map_castle = CreateGameObject<GameObject>(L"Map_Castle");
        map_castle->AddComponent<Transform>();
        auto map_castleTexture = map_castle->AddComponent<TextureRenderer>("ART_Rampart01", RenderLayerInfo{ SortingLayer::BackGround, 1 });
        map_castleTexture->SetSize({ 715, 960 });
        auto map_castleTR = map_castle->GetComponent<Transform>();
        map_castleTR->SetPosition(Vector2F{ -556.f, -13.f });

        // �跰 01
        auto* map_barrack01 = CreateGameObject<GameObject>(L"Map_Barrack01");
        map_barrack01->AddComponent<Transform>();
        auto map_barrack01Texture = map_barrack01->AddComponent<TextureRenderer>("ART_Barracks01", RenderLayerInfo{ SortingLayer::BackGround, 1 });
        map_barrack01Texture->SetSize({ 87.5f, 80.f });
        auto map_barrack01TR = map_barrack01->GetComponent<Transform>();
        map_barrack01TR->SetPosition(Vector2F{ -135.f, 190.f });

        // �跰 02
        auto* map_barrack02 = CreateGameObject<GameObject>(L"Map_Barrack02");
        map_barrack02->AddComponent<Transform>();
        auto map_barrack02Texture = map_barrack02->AddComponent<TextureRenderer>("ART_Barracks02", RenderLayerInfo{ SortingLayer::BackGround, 1 });
        map_barrack02Texture->SetSize({ 175, 280 });
        auto map_barrack02TR = map_barrack02->GetComponent<Transform>();
        map_barrack02TR->SetPosition(Vector2F{ -195.f, 80.f });

        //// �׸���
        //// =====================================================================
        //// �׸��� ��ġ ����
        //// =====================================================================

        //m_buildSystem->CreateGrid(this);

        //// �׸��带 ��ġ�� ��ü ������ ũ��
        //const float areaWidth = 515.0f;
        //const float areaHeight = 760.0f;

        //// �׸��带 ��ġ�� ������ ���� ��� ���� ��ǥ
        //const Vector2F areaTopLeft = { -850.0f, 350.0f };

        //// �׸����� ��� �� ����
        //m_totalCols = 4;
        //m_totalRows = 6;

        //// ���� �� ������ ���� (����)
        //const float padding = 10.0f;

        //// =====================================================================
        //// �׸��� ���� ����
        //// =====================================================================

        //// 1. ���� ������ ��ġ�� �� �ִ� ���� ��� (��ü ���� - �� ����)
        //float contentWidth = areaWidth - (padding * (m_totalCols - 1));
        //float contentHeight = areaHeight - (padding * (m_totalRows - 1));

        //// 2. ���簢�� ���� �� ���� �ִ� ũ�� ���
        //float cellWidthPossible = contentWidth / m_totalCols;
        //float cellHeightPossible = contentHeight / m_totalRows;

        //// 3. ���簢�� ������ ���� ������ ũ�� �� �� ���� ���� ���� �� ũ��� ����
        //const float squareCellSize = std::min(cellWidthPossible, cellHeightPossible);

        //// 4. ���� �׸����� ��ü ũ�� ���
        //float finalGridWidth = (squareCellSize * m_totalCols) + (padding * (m_totalCols - 1));
        //float finalGridHeight = (squareCellSize * m_totalRows) + (padding * (m_totalRows - 1));

        //// 5. ��ü �׸��带 ���� ������ �߾ӿ� ��ġ�ϱ� ���� ���� ������ ���
        //float offsetX = (areaWidth - finalGridWidth) / 2.0f;
        //float offsetY = (areaHeight - finalGridHeight) / 2.0f;


        //for (int col = 0; col < m_totalCols; ++col) {
        //    for (int row = 0; row < m_totalRows; ++row) {

        //        std::wstring name = L"Box_" + std::to_wstring(col) + L"_" + std::to_wstring(row);
        //        auto* box = CreateGameObject<Grid>(name.c_str());

        //        // 6. �� ���� ���� ��ġ ���
        //        //    ������ + �߾� ������ + (�� ũ�� + ����) * �ε��� + �� �߾� ����
        //        float x = areaTopLeft.x + offsetX + (col * (squareCellSize + padding)) + (squareCellSize / 2.0f);
        //        float y = areaTopLeft.y - offsetY - (row * (squareCellSize + padding)) - (squareCellSize / 2.0f);

        //        box->GetComponent<Transform>()->SetPosition({ x, y });
        //        box->AddComponent<Editor_Clickable>();

        //        // TextureRenderer�� ���� �߰��մϴ�. (�̶� �ؽ�ó �̸��� ����Ӵϴ�)
        //        auto* textureRenderer = box->AddComponent<TextureRenderer>("");

        //        // ==========================================================
        //        // �ڽ� �ؽ�ó �̸� ���� ���� �� ����
        //        // ==========================================================
        //        int idx = col * m_totalRows + row; // 0 ~ 23
        //        int textureNumber = idx + 1;       // 1 ~ 24

        //        // stringstream�� ����� "ART_Tile" + "0" + "����" ���·� ����
        //        std::stringstream ss;
        //        ss << "ART_Tile" << std::setw(2) << std::setfill('0') << textureNumber;

        //        // �ϼ��� �ؽ�ó Ű("ART_Tile01", "ART_Tile02" ��)�� ����
        //        textureRenderer->SetTextureName(ss.str());
        //        // ==========================================================

        //        // ũ�� ���� �� �ݶ��̴� �߰�
        //        textureRenderer->SetSize({ squareCellSize, squareCellSize });

        //        auto* collider = box->AddComponent<BoxCollider>(Vector2F{ squareCellSize / 2.0f, squareCellSize / 2.0f });
        //        collider->SetIndex(idx);
        //    }
        //}

        // =====================================================================
        // UI ������Ʈ ���� (Screen Space)
        // =====================================================================

        auto cam = D2DRenderer::Instance().GetCamera();

        // 1) [���] �ڿ�
        ////////////////////////////////////////////////////////////////////////////////

        // �α�
        m_resPop = CreateUIObject<Button>(L"UI_ResPop");
        m_resPop->SetText(L"");
        m_resPop->SetTextureName("ART_RESPop01");
        m_resPop->SetSize({ 155.0f, 65.f });
        m_resPop->SetPosition({ -858.f, 492.f });
        m_resPop->SetAnchor({ 0.0f, 1.0f });

        // �α� �ؽ�Ʈ
        m_resPopText = CreateUIObject<Text>(L"UI_ResPopText");
        m_resPopText->SetText(L"1234");
        m_resPopText->SetTextFormatName("Sebang_24");
        m_resPopText->SetColor(D2D1::ColorF(0x69512C));
        m_resPopText->SetSize({ 300, 100 });
        m_resPopText->SetPosition({ -835, 489 });

        // �α� ���ʽ�
        m_resPopBonusText = CreateUIObject<Text>(L"UI_ResPopBonusText");
        m_resPopBonusText->SetText(L"+1234");
        m_resPopBonusText->SetTextFormatName("Sebang_16");
        m_resPopBonusText->SetColor(D2D1::ColorF(0.839f, 0.741f, 0.580f));
        m_resPopBonusText->SetSize({ 300, 100 });
        m_resPopBonusText->SetPosition({ -776, 515 });

        ////////////////////////////////////////////////////////////////////////////////

        // �ķ�
        m_resFood = CreateUIObject<Button>(L"UI_ResFood");
        m_resFood->SetText(L"");
        m_resFood->SetTextureName("ART_RESFood01");
        m_resFood->SetSize({ 165.0f, 65.f });
        m_resFood->SetPosition({ -660.f, 492.f });
        m_resFood->SetAnchor({ 0.0f, 1.0f });

        // �ķ� �ؽ�Ʈ
        m_resFoodText = CreateUIObject<Text>(L"UI_ResFoodText");
        m_resFoodText->SetText(L"1234");
        m_resFoodText->SetTextFormatName("Sebang_24");
        m_resFoodText->SetColor(D2D1::ColorF(0x69512C));
        m_resFoodText->SetSize({ 300, 100 });
        m_resFoodText->SetPosition({ -630, 489 });

        // �ķ� ���ʽ�
        m_resFoodBonusText = CreateUIObject<Text>(L"UI_ResFoodBonusText");
        m_resFoodBonusText->SetText(L"+1234");
        m_resFoodBonusText->SetTextFormatName("Sebang_16");
        m_resFoodBonusText->SetColor(D2D1::ColorF(0.839f, 0.741f, 0.580f));
        m_resFoodBonusText->SetSize({ 300, 100 });
        m_resFoodBonusText->SetPosition({ -570, 515 });

        ////////////////////////////////////////////////////////////////////////////////

        // ����
        m_resWood = CreateUIObject<Button>(L"UI_ResWood");
        m_resWood->SetText(L"");
        m_resWood->SetTextureName("ART_RESWood01");
        m_resWood->SetSize({ 165.0f, 68.f });
        m_resWood->SetPosition({ -450.f, 492.f });
        m_resWood->SetAnchor({ 0.0f, 1.0f });

        // ���� �ؽ�Ʈ
        m_resWoodText = CreateUIObject<Text>(L"UI_ResWoodText");
        m_resWoodText->SetText(L"1234");
        m_resWoodText->SetTextFormatName("Sebang_24");
        m_resWoodText->SetColor(D2D1::ColorF(0x69512C));
        m_resWoodText->SetSize({ 300, 100 });
        m_resWoodText->SetPosition({ -420, 489 });

        // ���� ���ʽ�
        m_resWoodBonusText = CreateUIObject<Text>(L"UI_ResWoodBonusText");
        m_resWoodBonusText->SetText(L"+1234");
        m_resWoodBonusText->SetTextFormatName("Sebang_16");
        m_resWoodBonusText->SetColor(D2D1::ColorF(0.839f, 0.741f, 0.580f));
        m_resWoodBonusText->SetSize({ 300, 100 });
        m_resWoodBonusText->SetPosition({ -358, 515 });

        ////////////////////////////////////////////////////////////////////////////////

        // ����
        m_resMineral = CreateUIObject<Button>(L"UI_ResMineral");
        m_resMineral->SetText(L"");
        m_resMineral->SetTextureName("ART_RESMineral01");
        m_resMineral->SetSize({ 165.0f, 64.f });
        m_resMineral->SetPosition({ -252.f, 489.f });
        m_resMineral->SetAnchor({ 0.0f, 1.0f });

        // ���� �ؽ�Ʈ
        m_resMineralText = CreateUIObject<Text>(L"UI_ResMineralText");
        m_resMineralText->SetText(L"1234");
        m_resMineralText->SetTextFormatName("Sebang_24");
        m_resMineralText->SetColor(D2D1::ColorF(0x69512C));
        m_resMineralText->SetSize({ 300, 100 });
        m_resMineralText->SetPosition({ -220, 489 });

        // ���� ���ʽ�
        m_resMineralBonusText = CreateUIObject<Text>(L"UI_ResMineralBonusText");
        m_resMineralBonusText->SetText(L"+1234");
        m_resMineralBonusText->SetTextFormatName("Sebang_16");
        m_resMineralBonusText->SetColor(D2D1::ColorF(0.839f, 0.741f, 0.580f));
        m_resMineralBonusText->SetSize({ 300, 100 });
        m_resMineralBonusText->SetPosition({ -157, 515 });

        ////////////////////////////////////////////////////////////////////////////////

        // 1) [���] ���� ���̺�
        m_monsterWaveBackground = CreateUIObject<Image>(L"UI_MonWaveBackground");
        m_monsterWaveBackground->SetTextureName("ART_UIWaveGauge01");
        m_monsterWaveBackground->SetSize({ 500.0f, 74.f });
        m_monsterWaveBackground->SetPosition({ 330.f, 485.f });
        m_monsterWaveBackground->SetAnchor({ 1.0f, 1.0f });

        ////////////////////////////////////////////////////////////////////////////////

        // 1) [���] ��¥ �� �ɼ� �г� ( �Ͻ�����, ���, �������, �ɼ� )
        m_datePanel = CreateUIObject<Image>(L"UI_ART_UIDate01");
        m_datePanel->SetTextureName("ART_UIDate01");
        m_datePanel->SetSize({ 350.0f, 166.f });
        m_datePanel->SetPosition({ 785.f, 447.f });
        m_datePanel->SetAnchor({ 1.0f, 1.0f });

        // �� (Year) �ؽ�Ʈ
        m_yearText = CreateUIObject<Text>(L"UI_YearText");
        m_yearText->SetText(L"1995");
        m_yearText->SetTextFormatName("Sebang_24");
        m_yearText->SetColor(D2D1::ColorF(0x69512C));
        m_yearText->SetSize({ 300, 100 });
        m_yearText->SetPosition({ 690, 390 });

        // �� (Month) �ؽ�Ʈ
        m_monthText = CreateUIObject<Text>(L"UI_MonthText");
        m_monthText->SetText(L"12");
        m_monthText->SetTextFormatName("Sebang_24");
        m_monthText->SetColor(D2D1::ColorF(0x69512C));
        m_monthText->SetSize({ 300, 100 });
        m_monthText->SetPosition({ 785, 390 });

        // �� (Day) �ؽ�Ʈ
        m_dayText = CreateUIObject<Text>(L"UI_DayText");
        m_dayText->SetText(L"18");
        m_dayText->SetTextFormatName("Sebang_24");
        m_dayText->SetColor(D2D1::ColorF(0x69512C));
        m_dayText->SetSize({ 300, 100 });
        m_dayText->SetPosition({ 880, 390 });

        ////////////////////////////////////////////////////////////////////////////////

        // ���� ��ư
        m_stopButton = CreateUIObject<Button>(L"UI_StopButton");
        m_stopButton->SetTextureName("ART_Pause01");
        m_stopButton->SetText(L"");
        m_stopButton->SetSize({ 40, 47 });
        m_stopButton->SetPosition({ 679.5f, 475.f });

        // ���� ��ư Ŭ���ϸ� ����� �̺�Ʈ
        m_stopButton->AddOnClick("Quit Game", [this]()
            {

            });

        // ���� ��ư ���콺�� �ø��� ����� �̺�Ʈ
        m_stopButton->AddOnEnter("Highlight On", [this]()
            {

            });

        // ���� ��ư ���콺�� ����� ����� �̺�Ʈ
        m_stopButton->AddOnExit("Highlight Off", [this]()
            {

            });

        ////////////////////////////////////////////////////////////////////////////////

        // ��� ��ư
        m_playButton = CreateUIObject<Button>(L"UI_PlayButton");
        m_playButton->SetTextureName("ART_Play01");
        m_playButton->SetText(L"");
        m_playButton->SetSize({ 31.5f, 45.f });
        m_playButton->SetPosition({ 762.f, 475.f });

        // ��� ��ư Ŭ���ϸ� ����� �̺�Ʈ
        m_playButton->AddOnClick("Quit Game", [this]()
            {

            });

        // ��� ��ư ���콺�� �ø��� ����� �̺�Ʈ
        m_playButton->AddOnEnter("Highlight On", [this]()
            {

            });

        // ��� ��ư ���콺�� ����� ����� �̺�Ʈ
        m_playButton->AddOnExit("Highlight Off", [this]()
            {

            });

        ////////////////////////////////////////////////////////////////////////////////

        // ���ǵ� ��ư
        m_speedButton = CreateUIObject<Button>(L"UI_SpeedButton");
        m_speedButton->SetTextureName("ART_Fast01");
        m_speedButton->SetText(L"");
        m_speedButton->SetSize({ 45.f, 45.f });
        m_speedButton->SetPosition({ 842.f, 475.f });

        // ���ǵ� ��ư Ŭ���ϸ� ����� �̺�Ʈ
        m_speedButton->AddOnClick("Quit Game", [this]()
            {

            });

        // ���ǵ� ��ư ���콺�� �ø��� ����� �̺�Ʈ
        m_speedButton->AddOnEnter("Highlight On", [this]()
            {

            });

        // ���ǵ� ��ư ���콺�� ����� ����� �̺�Ʈ
        m_speedButton->AddOnExit("Highlight Off", [this]()
            {

            });

        ////////////////////////////////////////////////////////////////////////////////

        // �ɼ� ��ư
        m_optionButton = CreateUIObject<Button>(L"UI_OptionButton");
        m_optionButton->SetTextureName("ART_SettingIcon01");
        m_optionButton->SetText(L"");
        m_optionButton->SetSize({ 48.f, 50.5f });
        m_optionButton->SetPosition({ 919, 483 });
        m_optionButton->SetScale({ 0.85f, 0.85f });

        // �ɼ� ��ư Ŭ���ϸ� ����� �̺�Ʈ
        m_optionButton->AddOnClick("Quit Game", [this]()
            {

            });

        // ���ǵ� ��ư ���콺�� �ø��� ����� �̺�Ʈ
        m_optionButton->AddOnEnter("Highlight On", [this]()
            {

            });

        // ���ǵ� ��ư ���콺�� ����� ����� �̺�Ʈ
        m_optionButton->AddOnExit("Highlight Off", [this]()
            {

            });

        ////////////////////////////////////////////////////////////////////////////////

        // 0) �⺻ UI
        // TODO : ���ҽ��� ���� ����.
        m_defaultUI = CreateUIObject<Image>(L"UI_Default");
        m_defaultUI->SetTextureName("ART_UIBuilding01");
        if (cam)
        {
            float screenWidth = static_cast<float>(cam->GetScreenWidth());
            float screenHeight = static_cast<float>(cam->GetScreenHeight());

            // ȭ�� ũ��� ����
            m_defaultUI->SetSize(Vector2F{ screenWidth, screenHeight });
            m_defaultUI->SetPosition({ 0.0f, 0.0f });
            m_defaultUI->SetPivot({ 0.5f, 0.5f });
            m_defaultUI->SetAnchor({ 1.0f, 0.0f });
        }

        ////////////////////////////////////////////////////////////////////////////////

        // 1) �Ǽ� UI
        m_buildUI = CreateUIObject<Image>(L"UI_Build");
        m_buildUI->SetTextureName("ART_UIBuilding01");
        if (cam)
        {
            float screenWidth = static_cast<float>(cam->GetScreenWidth());
            float screenHeight = static_cast<float>(cam->GetScreenHeight());

            // ȭ�� ũ��� ����
            m_buildUI->SetSize(Vector2F{ screenWidth, screenHeight });
            m_buildUI->SetPosition({ 0.0f, 0.0f });
            m_buildUI->SetPivot({ 0.5f, 0.5f });
            m_buildUI->SetAnchor({ 1.0f, 0.0f });
        }

        ////////////////////////////////////////
        // �Ǽ� UI _ Info
        ////////////////////////////////////////

        // �Ǽ� Ÿ�� �ؽ�Ʈ
        m_buildTypeText = CreateUIObject<Text>(L"UI_BuildTypeText");
        m_buildTypeText->SetText(L"����");
        m_buildTypeText->SetTextFormatName("Sebang_Bold_24");
        m_buildTypeText->SetColor(D2D1::ColorF(0x69512C));
        m_buildTypeText->SetSize({ 300, 100 });
        m_buildTypeText->SetPosition({ -64, -390 });

        // �ڽ�Ʈ �ؽ�Ʈ
        m_costInfoText = CreateUIObject<Text>(L"UI_BuildCostInfoText");
        m_costInfoText->SetText(L"�ڽ�Ʈ");
        m_costInfoText->SetTextFormatName("Sebang_16");
        m_costInfoText->SetColor(D2D1::ColorF(0x69512C));
        m_costInfoText->SetSize({ 300, 100 });
        m_costInfoText->SetPosition({ -120, -428 });

        m_costText = CreateUIObject<Text>(L"UI_BuildCostText");
        m_costText->SetText(L"50");
        m_costText->SetTextFormatName("Sebang_16");
        m_costText->SetColor(D2D1::ColorF(0x69512C));
        m_costText->SetSize({ 300, 100 });
        m_costText->SetPosition({ 0, -428 });

        // �ڽ�Ʈ �̹���
        m_costImage = CreateUIObject<Image>(L"UI_BuildCostImage");
        m_costImage->SetTextureName("ART_CostWood01");
        m_costImage->SetSize({ 40, 31 });
        m_costImage->SetPosition({ -46, -428 });
        m_costImage->SetAnchor({ 1.0f, 0.0f });

        // ȿ�� �ؽ�Ʈ
        m_effectInfoText = CreateUIObject<Text>(L"UI_BuildEffectInfoText");
        m_effectInfoText->SetText(L"ȿ��");
        m_effectInfoText->SetTextFormatName("Sebang_16");
        m_effectInfoText->SetColor(D2D1::ColorF(0x69512C));
        m_effectInfoText->SetSize({ 300, 100 });
        m_effectInfoText->SetPosition({ -113, -470 });

        m_effectText = CreateUIObject<Text>(L"UI_BuildEffectText");
        m_effectText->SetText(L"1/��");
        m_effectText->SetTextFormatName("Sebang_16");
        m_effectText->SetColor(D2D1::ColorF(0x69512C));
        m_effectText->SetSize({ 300, 100 });
        m_effectText->SetPosition({ 0, -470 });

        // ȿ�� �̹���
        m_effctImage = CreateUIObject<Image>(L"UI_BuildEffctImage");
        m_effctImage->SetTextureName("ART_CostFood01");
        m_effctImage->SetSize({ 40, 31 });
        m_effctImage->SetPosition({ -46, -470 });
        m_effctImage->SetAnchor({ 1.0f, 0.0f });

        ////////////////////////////////////////
        // �Ǽ� UI _ �ְ���
        ////////////////////////////////////////
        m_buildHouse = CreateUIObject<Button>(L"UI_Build_House");
        m_buildHouse->SetTextureName("ART_BuildCabin01");
        m_buildHouse->SetText(L"");
        m_buildHouse->SetSize({ 166.f, 166.f });
        m_buildHouse->SetPosition({ 200.f, -430.f });

        // �ְ��� ��ư Ŭ���ϸ� ����� �̺�Ʈ
        m_buildHouse->AddOnClick("Quit Game", [this]()
            {

            });

        // �ְ��� ��ư ���콺�� �ø��� ����� �̺�Ʈ
        m_buildHouse->AddOnEnter("Highlight On", [this]()
            {
                ShowBuildInfo(JDGlobal::Contents::BuildingType::House, "50", "1/��");
            });

        // �ְ��� ��ư ���콺�� ����� ����� �̺�Ʈ
        m_buildHouse->AddOnExit("Highlight Off", [this]()
            {
                CloseBuildInfo();
            });


        ////////////////////////////////////////
        // �Ǽ� UI _ ������
        ////////////////////////////////////////
        m_buildFishingspot = CreateUIObject<Button>(L"UI_Build_Fishing");
        m_buildFishingspot->SetTextureName("ART_BuildFishing01");
        m_buildFishingspot->SetText(L"");
        m_buildFishingspot->SetSize({ 166.f, 166.f });
        m_buildFishingspot->SetPosition({ 400.f, -430.f });

        // ������ ��ư Ŭ���ϸ� ����� �̺�Ʈ
        m_buildFishingspot->AddOnClick("Quit Game", [this]()
            {

            });

        // ������ ��ư ���콺�� �ø��� ����� �̺�Ʈ
        m_buildFishingspot->AddOnEnter("Highlight On", [this]()
            {
                ShowBuildInfo(JDGlobal::Contents::BuildingType::FishingSpot, "40", "2/��");
            });

        // ������ ��ư ���콺�� ����� ����� �̺�Ʈ
        m_buildFishingspot->AddOnExit("Highlight Off", [this]()
            {
                CloseBuildInfo();
            });


        ////////////////////////////////////////
        // �Ǽ� UI _ �����
        ////////////////////////////////////////
        m_buildRumbermill = CreateUIObject<Button>(L"UI_Build_Lumbermill");
        m_buildRumbermill->SetTextureName("ART_BuildLumbermill01");
        m_buildRumbermill->SetText(L"");
        m_buildRumbermill->SetSize({ 166.f, 166.f });
        m_buildRumbermill->SetPosition({ 600.f, -430.f });

        // ����� ��ư Ŭ���ϸ� ����� �̺�Ʈ
        m_buildRumbermill->AddOnClick("Quit Game", [this]()
            {

            });

        // ����� ��ư ���콺�� �ø��� ����� �̺�Ʈ
        m_buildRumbermill->AddOnEnter("Highlight On", [this]()
            {
                ShowBuildInfo(JDGlobal::Contents::BuildingType::LumberMill, "30", "3/��");
            });

        // ����� ��ư ���콺�� ����� ����� �̺�Ʈ
        m_buildRumbermill->AddOnExit("Highlight Off", [this]()
            {
                CloseBuildInfo();
            });


        ////////////////////////////////////////
        // �Ǽ� UI _ ����
        ////////////////////////////////////////
        m_buildMine = CreateUIObject<Button>(L"UI_Build_Mine");
        m_buildMine->SetTextureName("ART_BuildMine01");
        m_buildMine->SetText(L"");
        m_buildMine->SetSize({ 166.f, 166.f });
        m_buildMine->SetPosition({ 800.f, -430.f });

        // ���� ��ư Ŭ���ϸ� ����� �̺�Ʈ
        m_buildMine->AddOnClick("Quit Game", [this]()
            {

            });

        // ���� ��ư ���콺�� �ø��� ����� �̺�Ʈ
        m_buildMine->AddOnEnter("Highlight On", [this]()
            {
                ShowBuildInfo(JDGlobal::Contents::BuildingType::Mine, "20", "4/��");
            });

        // ���� ��ư ���콺�� ����� ����� �̺�Ʈ
        m_buildMine->AddOnExit("Highlight Off", [this]()
            {
                CloseBuildInfo();
            });

        ////////////////////////////////////////////////////////////////////////////////

        // 2) �ǹ� ���׷���Ʈ �� ����� ��ġ UI
        m_upgradeUI = CreateUIObject<Image>(L"UI_Upgrade");
        m_upgradeUI->SetTextureName("ART_UICharSelect01");
        if (cam)
        {
            float screenWidth = static_cast<float>(cam->GetScreenWidth());
            float screenHeight = static_cast<float>(cam->GetScreenHeight());

            // ȭ�� ũ��� ����
            m_upgradeUI->SetSize(Vector2F{ screenWidth, screenHeight });
            m_upgradeUI->SetPosition({ 0.0f, 0.0f });
            m_upgradeUI->SetPivot({ 0.5f, 0.5f });
            m_upgradeUI->SetAnchor({ 1.0f, 0.0f });
        }

        // 3) ¡�� �� ���� UI
        m_expeditionUI = CreateUIObject<Image>(L"UI_Expedition");
        m_expeditionUI->SetTextureName("ART_UIRecruit01");
        if (cam)
        {
            float screenWidth = static_cast<float>(cam->GetScreenWidth());
            float screenHeight = static_cast<float>(cam->GetScreenHeight());

            // ȭ�� ũ��� ����
            m_expeditionUI->SetSize(Vector2F{ screenWidth, screenHeight });
            m_expeditionUI->SetPosition({ 0.0f, 0.0f });
            m_expeditionUI->SetPivot({ 0.5f, 0.5f });
            m_expeditionUI->SetAnchor({ 1.0f, 0.0f });
        }

        //// 2) ����
        //m_fillter = CreateUIObject<Image>(L"Fillter_Image");
        //m_fillter->SetTextureName("BACK_GROUND_OPACITY");
        //m_fillter->SetColor(D2D1::ColorF(D2D1::ColorF::White, 0.65f));

        //if (cam)
        //{
        //    float screenWidth = static_cast<float>(cam->GetScreenWidth());
        //    float screenHeight = static_cast<float>(cam->GetScreenHeight());

        //    // ȭ�� ũ��� ����
        //    m_fillter->SetSize(Vector2F{ screenWidth, screenHeight });
        //    m_fillter->SetPosition({ 0.0f,0.0f });
        //    m_fillter->SetPivot({ 0.5f, 0.5f });
        //}

        //// 3) ���
        //m_narration = CreateUIObject<Text>(L"Narration_Text");
        //// Ʃ�丮�� �̺�Ʈ�� �Ѱ���.
        ///*m_narration->SetText(
        //    L"\"�� ���ο� ����� ������ ���� ȯ���մϴ�.\"\n"
        //    L"����� �ǹ��� ���� ������ Ű��� ���ø� ���ѳ��� �մϴ�.\n"
        //);*/
        //m_narration->SetTextFormatName("Sebang_40");
        //m_narration->SetColor(D2D1::ColorF(0.839f, 0.741f, 0.580f));
        //m_narration->SetSize({ 1600, 300 });
        //m_narration->SetPosition({ 0, 320 });


        //// 4) �׸��� Ŭ���ϸ� ������ �޴�
        //m_Menu = CreateUIObject<Image>(L"MenuBackground");

        //auto& uptr = m_uiObjects.back();
        //UIObject* rawPtr = uptr.get();
        //m_TutorialUIObjects.push_back(rawPtr);

        //m_Menu->SetTextureName("Menu");
        //m_Menu->SetSizeToOriginal();
        ////m_Menu->SetSize({});
        //m_Menu->SetActive(false);

        //auto* bgRect = m_Menu->GetComponent<RectTransform>();
        //bgRect->SetPivot({ 0.f,0.f });
        //bgRect->SetAnchor({ 0.f,0.f });
        //bgRect->SetPosition({ 50, 50 });

        ////����ư Ű ���
        //std::vector<std::pair<std::wstring, std::string>> EmptyButtons = {
        //{ L"FishingSpot", "fishing" },
        //{ L"LumberMill", "lumbermill" },
        //{ L"Mine", "mine" },
        //{ L"House", "cabin" },
        //// �ʿ��ϴٸ� �� �߰���
        //};
        //std::vector<std::pair<std::wstring, std::string>> FilledButtons = {
        //{ L"CAT", "CAT" },
        //// �ʿ��ϴٸ� �� �߰���
        //};

        //// ���� �ʱ� ������
        //float xOffset = 0.f;
        //float yOffset = -150.f;
        //float gap = 100.f;

        //// 2-1) �� �޴��� ��ư ����
        //{
        //    float x = xOffset;
        //    float y = yOffset;
        //    for (auto& info : EmptyButtons) {
        //        auto* btn = CreateUIObject<Button>(info.first);
        //        // Ʃ�丮��� ����
        //        m_TutorialUIObjects.push_back(m_uiObjects.back().get());
        //        m_emptyButtons.push_back(btn);

        //        btn->SetTextureName(info.second);
        //        btn->SetText(L"");
        //        auto* rt = btn->GetComponent<RectTransform>();

        //        // [����] RectTransform�� SetParent�θ� �θ�-�ڽ� ���踦 �����մϴ�.
        //        rt->SetParent(bgRect);

        //        // [����] �ǹ��� �߾����� �����Ͽ� ��ġ�� ��� ���� �մϴ�.
        //        rt->SetPivot({ 0.0f, 0.0f });
        //        rt->SetAnchor({ 0.0f, 0.0f });
        //        rt->SetSize({ 50, 50 });

        //        // ���� �� Position�� ��ư�� '�߾�'�� �θ��� (x, yOffset)���� �̵���ŵ�ϴ�.
        //        rt->SetPosition({ x, y });


        //        // LEGACY : ���� �ڵ�
        //        /*
        //        //rt->SetPivot({ 0,0 });
        //        //rt->SetAnchor({ 0,0 });
        //        //rt->SetSize({ 50,50 });
        //        //rt->SetParent(bgRect);
        //        //rt->SetPosition({ x, yOffset });
        //        */

        //        m_Menu->AddChild(btn);
        //        btn->SetActive(false);
        //        m_menuButtons.push_back(btn);

        //        x += gap;
        //    }
        //}

        //// 2-2) ä�� �޴��� ��ư ����
        //{
        //    float x = xOffset;
        //    for (auto& info : FilledButtons) {
        //        auto* btn = CreateUIObject<Button>(info.first);
        //        m_TutorialUIObjects.push_back(m_uiObjects.back().get());
        //        m_filledButtons.push_back(btn);

        //        btn->SetTextureName(info.second);
        //        btn->SetText(L"");
        //        auto* rt = btn->GetComponent<RectTransform>();

        //        // [����] RectTransform�� SetParent�θ� �θ�-�ڽ� ���踦 �����մϴ�.
        //        rt->SetParent(bgRect);

        //        // [����] �ǹ��� �߾����� �����Ͽ� ��ġ�� ��� ���� �մϴ�.
        //        rt->SetPivot({ 0.5f, 0.5f });
        //        rt->SetAnchor({ 0.0f, 0.0f });
        //        rt->SetSize({ 50, 50 });

        //        // ���� �� Position�� ��ư�� '�߾�'�� �θ��� (x, yOffset)���� �̵���ŵ�ϴ�.
        //        rt->SetPosition({ x, yOffset });

        //        // LEGACY : ���� �ڵ�
        //        /*
        //        //rt->SetPivot({ 0,0 });
        //        //rt->SetAnchor({ 0,0 });
        //        //rt->SetSize({ 50,50 });
        //        //rt->SetParent(bgRect);
        //        //rt->SetPosition({ x, xOffset });
        //        */

        //        m_Menu->AddChild(btn);
        //        btn->SetActive(false);
        //        m_menuButtons.push_back(btn);

        //        x += gap;
        //    }
        //}

        // �⺻ UI ���̱�
        InitBuildMenu();
    }

    void GameScene::InitBuildMenu()
    {
        // �⺻ UI ���̱�
        m_defaultUI->SetActive(true);
        m_buildUI->SetActive(false);
        m_upgradeUI->SetActive(false);
        m_expeditionUI->SetActive(false);

        // �Ǽ� UI ( �ְ���, ������, �����, ����, (������) )
        m_buildHouse->SetActive(false);
        m_buildFishingspot->SetActive(false);
        m_buildRumbermill->SetActive(false);
        m_buildMine->SetActive(false);

        // �Ǽ� Info
        m_buildTypeText->SetActive(false);

        m_costInfoText->SetActive(false);
        m_costText->SetActive(false);
        m_costImage->SetActive(false);

        m_effectInfoText->SetActive(false);
        m_effectText->SetActive(false);
        m_effctImage->SetActive(false);
    }

    void GameScene::ShowBuildMenu()
    {
        // �Ǽ� UI ���̱�
        m_defaultUI->SetActive(false);
        m_buildUI->SetActive(true);
        m_upgradeUI->SetActive(false);
        m_expeditionUI->SetActive(false);

        // �Ǽ� UI ( �ְ���, ������, �����, ����, (������) )
        m_buildHouse->SetActive(true);
        m_buildFishingspot->SetActive(true);
        m_buildRumbermill->SetActive(true);
        m_buildMine->SetActive(true);

        // �Ǽ� Info
        CloseBuildInfo();
    }

    void GameScene::CloseBuildMenu()
    {
        // �Ǽ� UI ����
        m_defaultUI->SetActive(true);
        m_buildUI->SetActive(false);
        m_upgradeUI->SetActive(false);
        m_expeditionUI->SetActive(false);

        // �Ǽ� UI ( �ְ���, ������, �����, ����, (������) )
        m_buildHouse->SetActive(false);
        m_buildFishingspot->SetActive(false);
        m_buildRumbermill->SetActive(false);
        m_buildMine->SetActive(false);

        // �Ǽ� Info
        m_buildTypeText->SetActive(false);

        m_costInfoText->SetActive(false);
        m_costText->SetActive(false);
        m_costImage->SetActive(false);

        m_effectInfoText->SetActive(false);
        m_effectText->SetActive(false);
        m_effctImage->SetActive(false);
    }

    void GameScene::ShowBuildInfo(JDGlobal::Contents::BuildingType buildType, std::string costText, std::string effectText)
    {
        ChangeBuildInfo(buildType, costText, effectText);

        m_costInfoText->SetActive(true);
        m_costText->SetActive(true);
        m_costImage->SetActive(true);

        m_effectInfoText->SetActive(true);
        m_effectText->SetActive(true);
        m_effctImage->SetActive(true);
    }

    void GameScene::CloseBuildInfo()
    {
        // �Ǽ� Info
        m_buildTypeText->SetActive(true);
        m_buildTypeText->SetText(L"�ǹ� ����");

        m_costInfoText->SetActive(false);
        m_costText->SetActive(false);
        m_costImage->SetActive(false);

        m_effectInfoText->SetActive(false);
        m_effectText->SetActive(false);
        m_effctImage->SetActive(false);
    }

    void GameScene::ChangeBuildInfo(JDGlobal::Contents::BuildingType buildType, std::string costText, std::string effectText)
    {
        switch (buildType)
        {
        case JDGlobal::Contents::BuildingType::House:
            m_buildTypeText->SetText(L"�ְ���");
            break;
        case JDGlobal::Contents::BuildingType::FishingSpot:
            m_buildTypeText->SetText(L"������");
            break;
        case JDGlobal::Contents::BuildingType::LumberMill:
            m_buildTypeText->SetText(L"�����");
            break;
        case JDGlobal::Contents::BuildingType::Mine:
            m_buildTypeText->SetText(L"����");
            break;
        case JDGlobal::Contents::BuildingType::Lab:
            m_buildTypeText->SetText(L"������");
            break;
        }
    }

    void GameScene::ShowFilledMenu() {
        m_Menu->SetActive(true);
        // �� �޴� ��ư�� ����
        for (auto* btn : m_emptyButtons)   btn->SetActive(false);
        // ä�� �޴� ��ư�� �ѱ�
        for (auto* btn : m_filledButtons)  btn->SetActive(true);
    }
}
