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
        
    }

    void GameScene::OnLeave() {
        //cout << "[GameScene] OnLeave()\n";
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
                JDGlobal::Contents::State::Back, m_battleObject->GetComponent<Transform>()->GetPosition(), "CAT");
        }
        if (enemyResult.Total() > 0) {
            auto* enemyObj = CreateSoldierUnit(enemyResult, JDGlobal::Base::GameTag::Enemy, 
                JDGlobal::Contents::State::Move, m_battleObject->GetComponent<Transform>()->GetPosition(), "CAT");
        }

        m_btlElapsedTime = 0.0f;
        SafeDestroy(m_battleObject);
        std::cout << "[GameScene] ���� ��." << std::endl;
        

    }

    void GameScene::SpawnWaveEnemy(const Vector2F& pos) {
        auto* enemyObj = CreateSoldierUnit(m_enemyArmy.GetUnitCounts(), JDGlobal::Base::GameTag::Enemy, 
            JDGlobal::Contents::State::Move, pos, "CAT");

        m_enemyArmy.OverrideUnitCounts({ 0, 0 });
    }

    void GameScene::SpawnPlayerArmy(const Vector2F& pos) {
        auto* playerObj = CreateSoldierUnit(m_playerArmy.GetUnitCounts(), JDGlobal::Base::GameTag::Player, 
            JDGlobal::Contents::State::Move, pos, "CAT");
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
}
