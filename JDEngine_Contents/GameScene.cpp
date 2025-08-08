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

        // 자원 세팅.
        auto& rs = ResourceSystem::Instance();
        rs.SetTotalResource(Resource(10000, 10000, 10000));
        rs.SetMaxPopulation(500);
        rs.SetCurPopulation(100);

        m_playerArmy.OverrideUnitCounts({100, 100});

        // 징병 버튼.
        CreateUIButton( L"noviceBtn", { 0.0f, -300.0f }, "GAME_START_B", "Recruit Novice",
            [this]() { m_playerArmy.RecruitUnits(JDGameSystem::UnitType::Novice);
                std::cout << "Player Power: " << m_playerArmy.CalculateTotalPower() << std::endl; });

        CreateUIButton( L"expertBtn", { 0.0f, -400.0f }, "GAME_START_B", "Recruit Expert",
            [this]() { m_playerArmy.RecruitUnits(JDGameSystem::UnitType::Expert);
                std::cout << "Player Power: " << m_playerArmy.CalculateTotalPower() << std::endl; });


        // 병영.
        m_barracksObject = CreateStructure(L"barracksObj", JDGlobal::Base::GameTag::Barracks, { -500.0f, 100.0f }, "house");

        // 성벽.
        m_wallObject = CreateStructure(L"wallObj", JDGlobal::Base::GameTag::Wall, { -800.0f, 100.0f }, "house");

        { // 테스트. UI에 필요한 정보를 게임씬에서 어떻게 가져오는지.

            // 1. 각 병종별 자원, 전투력 정보.
            const JDGlobal::Contents::UnitTypeData& noviceData = m_playerArmy.GetUnitTypes()[0];
            const JDGlobal::Contents::UnitTypeData& expertData = m_playerArmy.GetUnitTypes()[1]; 

            Resource noviceCost = noviceData.GetRecruitCost(); // novice 비용.
            int novicePower = noviceData.GetPower();           // novice 전투력.

            Resource expertCost = expertData.GetRecruitCost(); // expert 비용.
            int expertPower = expertData.GetPower();           // expert 전투력.

            // 2. 병영 클릭 여부.
            if (m_isBarracksSelected) {
                // 이렇게 쓰면 될 듯.
            }

            // 3. 날짜 정보. 
            int currDay = WaveManager::Instance().GetCurrDay(); // 현재 날짜.

            // 4. 웨이브 정보.
            const WaveData* nextWave = WaveManager::Instance().GetWave(1); // 이런식으로 해당 날짜의 정보를 가져올 수 있음.

            // 5. 병사들 위치에 전투력 표시. 
            /*for (auto& objPtr : m_gameObjects)
            {
                if (!objPtr || objPtr->GetTag() != JDGlobal::Base::GameTag::Player &&
                    objPtr->GetTag() != JDGlobal::Base::GameTag::Enemy)
                    continue;
                // 아군과 적의 위치 가져오기.
                auto* transform = objPtr->GetComponent<Transform>();
                if (!transform) continue;
                Vector2F position = transform->GetPosition();

                // 전투력 가져오기.
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
            bgmChannel->stop(); // FMOD에서 채널을 멈춤
            bgmChannel = nullptr; // 포인터도 초기화 (안전)
        }
    }

    void GameScene::Update(float deltaTime) {
        SceneBase::Update(deltaTime);

        ProcessDayTimer(deltaTime); // 날짜 및 웨이브 관리.
        ProcessBattle(deltaTime); // 전투 지속시간 및 전투 결과 관리. 

        ClickUpdate();
        MoveEnemyObjects(deltaTime); // 적 이동.
        MovePlayerObjects(deltaTime); // 아군 이동.

        AttackWall(deltaTime); // 적이 성 공격하는 것 관리.

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

    void GameScene::ProcessDayTimer(float deltaTime) {
        m_elapsedTime += deltaTime;
        if (m_battleObject) m_btlElapsedTime += deltaTime;

        if (m_elapsedTime >= m_dayTime) {
            m_elapsedTime = 0.0f;
            WaveManager::Instance().AdvanceDay();

            const int currDay = WaveManager::Instance().GetCurrDay();
            //const int warningDay = currDay + 2;
            const int warningDay = currDay; // 당일에 화면에 나타나게.

            std::cout << "[GameScene] 날짜 증가: " << currDay << std::endl;

            // 웨이브가 있는 경우, 아직 화면에 나타나지 않았으면 생성.
            if (WaveManager::Instance().GetWave(warningDay) && !IsEnemySpawned(warningDay)) {
                m_enemyArmy.OverrideUnitCounts(WaveManager::Instance().GetWave(warningDay)->enemyUnits);
                float windowWidth = JDGlobal::Window::WindowSize::Instance().GetWidth();
                SpawnWaveEnemy({ (windowWidth + 100.0F) / 2.0f, 100.0f });
                AddEnemyDay(warningDay);
                std::cout << "[GameScene] 적 스폰됨." << std::endl;
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
        std::cout << "[GameScene] 전투 끝." << std::endl;
        

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
        const float speed = 100.0f; // 픽셀/초
        Vector2F wallDir = { -1.0f, 0.0f }; // 기본 방향. (성벽이 없다면)

        for (auto& objPtr : m_gameObjects)
        {
            if (!objPtr || objPtr->GetTag() != JDGlobal::Base::GameTag::Enemy || 
                objPtr->GetState() != JDGlobal::Contents::State::Move) continue;

            auto* transform = objPtr->GetComponent<Transform>();
            auto* sfx = objPtr->GetComponent<JDComponent::SFX>();
            if (!transform) continue;

            // 적이 전투 오브젝트와 충돌하면 전투 시간 초기화하고 제거.
            if (m_battleObject) {
                auto* battleTm = m_battleObject->GetComponent<Transform>();
                if (!battleTm)  continue;
                Vector2F diffPos = battleTm->GetPosition() - transform->GetPosition();

                if (diffPos.Length() <= 10.0f) {
                    auto* soldier = dynamic_cast<JDGameObject::Content::Soldier*>(objPtr.get());
                    if (soldier) {
                        m_enemyBattleArmy += soldier->GetUnitCounts();
                        m_enemyTotalPower += soldier->GetPower();
                        std::cout << "[GameScene] 적 병력 추가." << std::endl;
                    }
                    SafeDestroy(objPtr.get());
                    m_btlElapsedTime = 0.0f;
                    continue;
                }
            }

            // 성벽이랑 충돌하면 멈추고 성벽을 공격함.
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

            // 이동 적용.
            Vector2F delta = wallDir * (speed * deltaTime);
            Vector2F newPos = transform->GetPosition() + delta;
            transform->SetPosition(newPos);

            if (sfx) sfx->Play();  // 이동 시 효과음 재생
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

            // 아군이 전투 오브젝트와 충돌하면 전투 시간 초기화하고 제거.
            if (m_battleObject) {
                auto* battleTm = m_battleObject->GetComponent<Transform>();
                if (!battleTm) continue;
                Vector2F diffPos = battleTm->GetPosition() - transform->GetPosition();

                if (diffPos.Length() <= 10.0f) {
                    auto* soldier = dynamic_cast<JDGameObject::Content::Soldier*>(objPtr.get());
                    if (soldier) {
                        m_playerBattleArmy += soldier->GetUnitCounts();
                        m_playerTotalPower += soldier->GetPower();
                        std::cout << "[GameScene] 아군 병력 추가." << std::endl;
                    }
                    SafeDestroy(objPtr.get());
                    m_btlElapsedTime = 0.0f;
                    continue;
                }
            }

            // 최초 아군일 경우, 타겟 적과의 충돌 체크. 충돌하면 제거.
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
                        std::cout << "[GameScene] 아군 병력 추가." << std::endl;
                    }
                    auto* enemy = dynamic_cast<JDGameObject::Content::Soldier*>(m_targetEnemy);
                    if (enemy) {
                        m_enemyBattleArmy += enemy->GetUnitCounts();
                        m_enemyTotalPower += enemy->GetPower();
                        std::cout << "[GameScene] 적 병력 추가." << std::endl;
                    }

                    SafeDestroy(m_playerObject);
                    SafeDestroy(m_targetEnemy);

                    SpawnBattleObject(mid);
                    return;
                }
            }

            // 전투 오브젝트가 없고, 일반 아군이면 후퇴 상태로 전환
            if (!m_battleObject && objPtr.get() != m_playerObject) {
                if (!m_barracksObject) continue;
                auto* texRenderer = objPtr->GetComponent<TextureRenderer>();
                if (texRenderer) {
                    texRenderer->SetFlipX(true); // 좌우 반전!
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
                        std::cout << "[GameScene] 병영 합류." << std::endl;
                    }
                    SafeDestroy(objPtr.get());
                    continue;
                }

                objPtr->SetState(JDGlobal::Contents::State::Back);
            }

            Vector2F direction = { 1.0f, 0.0f }; // 기본값.
            float moveSpeed = speed;

            if (objPtr->GetState() == JDGlobal::Contents::State::Back) {
                // 병영 방향으로 후퇴
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
                // 적 유닛 방향으로 이동
                auto* enemyTm = m_targetEnemy->GetComponent<Transform>();
                if (enemyTm) {
                    Vector2F diff = enemyTm->GetPosition() - transform->GetPosition();
                    direction = diff.Normalized();
                }
            }
            else if (m_battleObject) {
                // 전투 오브젝트 방향으로 이동
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
                    std::cout << "[GameScene] 성벽 남은 체력: " << m_wallHealth << std::endl;
                    it->timer -= m_battleTime;
                }
            }

            // 비활성화된 적은 목록에서 제거
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
        // ImGui가 마우스 입력을 사용 중이면 게임 내 클릭을 무시합니다.
        if (ImGui::GetIO().WantCaptureMouse)
            return;

        InputManager& input = InputManager::Instance();
        MouseState state = input.GetMouseState();

        // state.leftClicked 또는 state.leftPressed 등 필요한 입력 상태를 사용합니다.
        if (state.leftClicked)
        {
            // (1) 마우스 좌표를 맨 위에서 한 번만 계산해서 재사용합니다.
            // UI 클릭 판정에 사용할 스크린 좌표 (D2D 기준: Y 아래가 양수)
            Vector2F screenMousePos(
                static_cast<float>(state.pos.x),
                static_cast<float>(state.pos.y)
            );
            // 게임 오브젝트 클릭 판정에 사용할 월드 좌표 (Unity 기준: Y 위가 양수)
            Vector2F worldMousePos = GetMouseWorldPos(); // 우리가 만든 통일된 함수 사용!

            bool clicked = false;

            ////////////////////////////////////////////////////////////////////////////////
            // 1. UI 클릭 검사 (스크린 좌표계 사용)
            ////////////////////////////////////////////////////////////////////////////////
            // 이 로직은 스크린 좌표를 사용하므로 기존과 동일하게 올바르게 동작합니다.
            for (int i = static_cast<int>(m_uiObjects.size()) - 1; i >= 0; --i)
            {
                auto& uiObj = m_uiObjects[i];
                if (!uiObj || !uiObj->IsActive()) continue;

                auto clickable = uiObj->GetComponent<Editor_Clickable>();
                // UI의 IsHit 함수에는 '스크린 좌표'를 그대로 넘겨줍니다.
                if (clickable && clickable->IsHit(screenMousePos))
                {
                    SetSelectedObject(uiObj.get());
                    clicked = true;
                    std::cout << " UI 오브젝트 클릭 함!!!!! ";
                    break; // UI를 클릭했으면 더 이상 진행 안 함
                }
            }

            ////////////////////////////////////////////////////////////////////////////////
            // 2. 게임오브젝트 클릭 검사 (월드 좌표계 사용)
            ////////////////////////////////////////////////////////////////////////////////
            if (!clicked)
            {
                // (2) 불필요하고 잘못된 좌표 변환 로직을 모두 제거합니다.
                /* Vector2F unityMousePos(mousePos.x, screenHeight - mousePos.y);
                    Vector2F worldMousePos = camera->ScreenToWorldPoint(unityMousePos);
                    -> 이 부분은 GetMouseWorldPos()로 대체되었으므로 삭제!
                */

                for (int i = static_cast<int>(m_gameObjects.size()) - 1; i >= 0; --i)
                {
                    auto& obj = m_gameObjects[i];
                    if (!obj || !obj->IsActive()) continue;

                    auto clickable = obj->GetComponent<Editor_Clickable>();
                    // (3) 게임 오브젝트의 IsHit 함수에는 위에서 계산한 '월드 좌표'를 넘겨줍니다.
                    if (clickable && clickable->IsHit(worldMousePos))
                    {
                        SetSelectedObject(obj.get());
                        clicked = true;
                        std::cout << " 게임 오브젝트 클릭 함!!!!! ";

                        // 태그 기반 처리
                        auto* gameObj = GetSelectedObject();
                        if (!gameObj) return;

                        auto tag = gameObj->GetTag();
                        bool isEnemyTag = (tag == JDGlobal::Base::GameTag::Enemy);

                        if (tag == JDGlobal::Base::GameTag::Barracks) {
                            m_isBarracksSelected = !m_isBarracksSelected;
                            std::cout << "[GameScene] 병영 클릭. 선택 상태: " << m_isBarracksSelected << std::endl;
                        }
                        else if (m_isBarracksSelected && !m_battleObject && (isEnemyTag && !m_targetEnemy) && m_playerArmy.GetTotalUnits() > 0) {
                            m_targetEnemy = gameObj;
                            SpawnPlayerArmy({ -400.0f, 100.0f });
                            //m_isBarracksSelected = false;
                            std::cout << "[GameScene] 병영 클릭 -> 적 클릭." << std::endl;
                        }
                        else if (m_isBarracksSelected && tag == JDGlobal::Base::GameTag::BattleAnim && m_playerArmy.GetTotalUnits() > 0) {
                            SpawnPlayerArmy({ -400.0f, 100.0f });
                            //m_isBarracksSelected = false;
                            std::cout << "[GameScene] 병영 클릭 -> 전투중 클릭." << std::endl;
                        }
                        else {
                            //m_isBarracksSelected = false; 
                            std::cout << "[GameScene] 이상한 거 클릭." << std::endl;
                        }
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

    // 공통 병사 생성 함수
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
                texRenderer->SetFlipX(true); // 좌우 반전!
            }
        }

        return obj;
    }

    // 공통 구조물 생성 함수
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

    // 공통 버튼 생성 함수
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
        // 게임 오브젝트 생성 (World Space)
        // =====================================================================

        // 배경
        auto* map_backGround = CreateGameObject<GameObject>(L"Map_Background");
        map_backGround->AddComponent<Transform>()->SetPosition(Vector2F{ 0.f, 0.f });
        auto map_backGroundTexture = map_backGround->AddComponent<TextureRenderer>("BACK_GROUND_OPACITY", RenderLayerInfo{ SortingLayer::BackGround, 1 });
        map_backGroundTexture->SetSize({ 3840, 2160 });

        // 배경 풀 밭
        auto* map_battle = CreateGameObject<GameObject>(L"Map_Field");
        map_battle->AddComponent<Transform>()->SetPosition(Vector2F{ 0.f, 0.f });
        auto map_battleTexture = map_battle->AddComponent<TextureRenderer>("ART_BattleMap01", RenderLayerInfo{ SortingLayer::BackGround, 1 });
        // auto map_battleTexture = map_battle->AddComponent<TextureRenderer>("BATTLE_MAP(old)", RenderLayerInfo{ SortingLayer::BackGround, 1 });
        map_battleTexture->SetSize({ JDGlobal::Window::WindowSize::Instance().GetSize() });

        // 풀 밭 _ 레이어 1
        auto* map_battleLayer01 = CreateGameObject<GameObject>(L"Map_FieldLayer01");
        map_battleLayer01->AddComponent<Transform>()->SetPosition(Vector2F{ 0.f, 0.f });
        auto map_battleLayer01Texture = map_battleLayer01->AddComponent<TextureRenderer>("ART_BattleMapLayer01", RenderLayerInfo{ SortingLayer::BackGround, 1 });
        map_battleLayer01Texture->SetSize({ JDGlobal::Window::WindowSize::Instance().GetSize() });

        // 성벽
        auto* map_castle = CreateGameObject<GameObject>(L"Map_Castle");
        map_castle->AddComponent<Transform>();
        auto map_castleTexture = map_castle->AddComponent<TextureRenderer>("ART_Rampart01", RenderLayerInfo{ SortingLayer::BackGround, 1 });
        map_castleTexture->SetSize({ 715, 960 });
        auto map_castleTR = map_castle->GetComponent<Transform>();
        map_castleTR->SetPosition(Vector2F{ -556.f, -13.f });

        // 배럭 01
        auto* map_barrack01 = CreateGameObject<GameObject>(L"Map_Barrack01");
        map_barrack01->AddComponent<Transform>();
        auto map_barrack01Texture = map_barrack01->AddComponent<TextureRenderer>("ART_Barracks01", RenderLayerInfo{ SortingLayer::BackGround, 1 });
        map_barrack01Texture->SetSize({ 87.5f, 80.f });
        auto map_barrack01TR = map_barrack01->GetComponent<Transform>();
        map_barrack01TR->SetPosition(Vector2F{ -135.f, 190.f });

        // 배럭 02
        auto* map_barrack02 = CreateGameObject<GameObject>(L"Map_Barrack02");
        map_barrack02->AddComponent<Transform>();
        auto map_barrack02Texture = map_barrack02->AddComponent<TextureRenderer>("ART_Barracks02", RenderLayerInfo{ SortingLayer::BackGround, 1 });
        map_barrack02Texture->SetSize({ 175, 280 });
        auto map_barrack02TR = map_barrack02->GetComponent<Transform>();
        map_barrack02TR->SetPosition(Vector2F{ -195.f, 80.f });

        //// 그리드
        //// =====================================================================
        //// 그리드 배치 정보
        //// =====================================================================

        //m_buildSystem->CreateGrid(this);

        //// 그리드를 배치할 전체 영역의 크기
        //const float areaWidth = 515.0f;
        //const float areaHeight = 760.0f;

        //// 그리드를 배치할 영역의 좌측 상단 월드 좌표
        //const Vector2F areaTopLeft = { -850.0f, 350.0f };

        //// 그리드의 행과 열 개수
        //m_totalCols = 4;
        //m_totalRows = 6;

        //// 셀과 셀 사이의 간격 (여백)
        //const float padding = 10.0f;

        //// =====================================================================
        //// 그리드 생성 로직
        //// =====================================================================

        //// 1. 실제 셀들이 배치될 수 있는 공간 계산 (전체 영역 - 총 간격)
        //float contentWidth = areaWidth - (padding * (m_totalCols - 1));
        //float contentHeight = areaHeight - (padding * (m_totalRows - 1));

        //// 2. 정사각형 셀의 한 변의 최대 크기 계산
        //float cellWidthPossible = contentWidth / m_totalCols;
        //float cellHeightPossible = contentHeight / m_totalRows;

        //// 3. 정사각형 유지를 위해 가능한 크기 중 더 작은 쪽을 최종 셀 크기로 선택
        //const float squareCellSize = std::min(cellWidthPossible, cellHeightPossible);

        //// 4. 최종 그리드의 전체 크기 계산
        //float finalGridWidth = (squareCellSize * m_totalCols) + (padding * (m_totalCols - 1));
        //float finalGridHeight = (squareCellSize * m_totalRows) + (padding * (m_totalRows - 1));

        //// 5. 전체 그리드를 영역 내에서 중앙에 배치하기 위한 시작 오프셋 계산
        //float offsetX = (areaWidth - finalGridWidth) / 2.0f;
        //float offsetY = (areaHeight - finalGridHeight) / 2.0f;


        //for (int col = 0; col < m_totalCols; ++col) {
        //    for (int row = 0; row < m_totalRows; ++row) {

        //        std::wstring name = L"Box_" + std::to_wstring(col) + L"_" + std::to_wstring(row);
        //        auto* box = CreateGameObject<Grid>(name.c_str());

        //        // 6. 각 셀의 최종 위치 계산
        //        //    시작점 + 중앙 오프셋 + (셀 크기 + 간격) * 인덱스 + 셀 중앙 보정
        //        float x = areaTopLeft.x + offsetX + (col * (squareCellSize + padding)) + (squareCellSize / 2.0f);
        //        float y = areaTopLeft.y - offsetY - (row * (squareCellSize + padding)) - (squareCellSize / 2.0f);

        //        box->GetComponent<Transform>()->SetPosition({ x, y });
        //        box->AddComponent<Editor_Clickable>();

        //        // TextureRenderer를 먼저 추가합니다. (이때 텍스처 이름은 비워둡니다)
        //        auto* textureRenderer = box->AddComponent<TextureRenderer>("");

        //        // ==========================================================
        //        // 박스 텍스처 이름 동적 생성 및 설정
        //        // ==========================================================
        //        int idx = col * m_totalRows + row; // 0 ~ 23
        //        int textureNumber = idx + 1;       // 1 ~ 24

        //        // stringstream을 사용해 "ART_Tile" + "0" + "숫자" 형태로 조합
        //        std::stringstream ss;
        //        ss << "ART_Tile" << std::setw(2) << std::setfill('0') << textureNumber;

        //        // 완성된 텍스처 키("ART_Tile01", "ART_Tile02" 등)를 설정
        //        textureRenderer->SetTextureName(ss.str());
        //        // ==========================================================

        //        // 크기 설정 및 콜라이더 추가
        //        textureRenderer->SetSize({ squareCellSize, squareCellSize });

        //        auto* collider = box->AddComponent<BoxCollider>(Vector2F{ squareCellSize / 2.0f, squareCellSize / 2.0f });
        //        collider->SetIndex(idx);
        //    }
        //}

        // =====================================================================
        // UI 오브젝트 생성 (Screen Space)
        // =====================================================================

        auto cam = D2DRenderer::Instance().GetCamera();

        // 1) [상단] 자원
        ////////////////////////////////////////////////////////////////////////////////

        // 인구
        m_resPop = CreateUIObject<Button>(L"UI_ResPop");
        m_resPop->SetText(L"");
        m_resPop->SetTextureName("ART_RESPop01");
        m_resPop->SetSize({ 155.0f, 65.f });
        m_resPop->SetPosition({ -858.f, 492.f });
        m_resPop->SetAnchor({ 0.0f, 1.0f });

        // 인구 텍스트
        m_resPopText = CreateUIObject<Text>(L"UI_ResPopText");
        m_resPopText->SetText(L"1234");
        m_resPopText->SetTextFormatName("Sebang_24");
        m_resPopText->SetColor(D2D1::ColorF(0x69512C));
        m_resPopText->SetSize({ 300, 100 });
        m_resPopText->SetPosition({ -835, 489 });

        // 인구 보너스
        m_resPopBonusText = CreateUIObject<Text>(L"UI_ResPopBonusText");
        m_resPopBonusText->SetText(L"+1234");
        m_resPopBonusText->SetTextFormatName("Sebang_16");
        m_resPopBonusText->SetColor(D2D1::ColorF(0.839f, 0.741f, 0.580f));
        m_resPopBonusText->SetSize({ 300, 100 });
        m_resPopBonusText->SetPosition({ -776, 515 });

        ////////////////////////////////////////////////////////////////////////////////

        // 식량
        m_resFood = CreateUIObject<Button>(L"UI_ResFood");
        m_resFood->SetText(L"");
        m_resFood->SetTextureName("ART_RESFood01");
        m_resFood->SetSize({ 165.0f, 65.f });
        m_resFood->SetPosition({ -660.f, 492.f });
        m_resFood->SetAnchor({ 0.0f, 1.0f });

        // 식량 텍스트
        m_resFoodText = CreateUIObject<Text>(L"UI_ResFoodText");
        m_resFoodText->SetText(L"1234");
        m_resFoodText->SetTextFormatName("Sebang_24");
        m_resFoodText->SetColor(D2D1::ColorF(0x69512C));
        m_resFoodText->SetSize({ 300, 100 });
        m_resFoodText->SetPosition({ -630, 489 });

        // 식량 보너스
        m_resFoodBonusText = CreateUIObject<Text>(L"UI_ResFoodBonusText");
        m_resFoodBonusText->SetText(L"+1234");
        m_resFoodBonusText->SetTextFormatName("Sebang_16");
        m_resFoodBonusText->SetColor(D2D1::ColorF(0.839f, 0.741f, 0.580f));
        m_resFoodBonusText->SetSize({ 300, 100 });
        m_resFoodBonusText->SetPosition({ -570, 515 });

        ////////////////////////////////////////////////////////////////////////////////

        // 목재
        m_resWood = CreateUIObject<Button>(L"UI_ResWood");
        m_resWood->SetText(L"");
        m_resWood->SetTextureName("ART_RESWood01");
        m_resWood->SetSize({ 165.0f, 68.f });
        m_resWood->SetPosition({ -450.f, 492.f });
        m_resWood->SetAnchor({ 0.0f, 1.0f });

        // 목재 텍스트
        m_resWoodText = CreateUIObject<Text>(L"UI_ResWoodText");
        m_resWoodText->SetText(L"1234");
        m_resWoodText->SetTextFormatName("Sebang_24");
        m_resWoodText->SetColor(D2D1::ColorF(0x69512C));
        m_resWoodText->SetSize({ 300, 100 });
        m_resWoodText->SetPosition({ -420, 489 });

        // 목재 보너스
        m_resWoodBonusText = CreateUIObject<Text>(L"UI_ResWoodBonusText");
        m_resWoodBonusText->SetText(L"+1234");
        m_resWoodBonusText->SetTextFormatName("Sebang_16");
        m_resWoodBonusText->SetColor(D2D1::ColorF(0.839f, 0.741f, 0.580f));
        m_resWoodBonusText->SetSize({ 300, 100 });
        m_resWoodBonusText->SetPosition({ -358, 515 });

        ////////////////////////////////////////////////////////////////////////////////

        // 광물
        m_resMineral = CreateUIObject<Button>(L"UI_ResMineral");
        m_resMineral->SetText(L"");
        m_resMineral->SetTextureName("ART_RESMineral01");
        m_resMineral->SetSize({ 165.0f, 64.f });
        m_resMineral->SetPosition({ -252.f, 489.f });
        m_resMineral->SetAnchor({ 0.0f, 1.0f });

        // 광물 텍스트
        m_resMineralText = CreateUIObject<Text>(L"UI_ResMineralText");
        m_resMineralText->SetText(L"1234");
        m_resMineralText->SetTextFormatName("Sebang_24");
        m_resMineralText->SetColor(D2D1::ColorF(0x69512C));
        m_resMineralText->SetSize({ 300, 100 });
        m_resMineralText->SetPosition({ -220, 489 });

        // 광물 보너스
        m_resMineralBonusText = CreateUIObject<Text>(L"UI_ResMineralBonusText");
        m_resMineralBonusText->SetText(L"+1234");
        m_resMineralBonusText->SetTextFormatName("Sebang_16");
        m_resMineralBonusText->SetColor(D2D1::ColorF(0.839f, 0.741f, 0.580f));
        m_resMineralBonusText->SetSize({ 300, 100 });
        m_resMineralBonusText->SetPosition({ -157, 515 });

        ////////////////////////////////////////////////////////////////////////////////

        // 1) [상단] 몬스터 웨이브
        m_monsterWaveBackground = CreateUIObject<Image>(L"UI_MonWaveBackground");
        m_monsterWaveBackground->SetTextureName("ART_UIWaveGauge01");
        m_monsterWaveBackground->SetSize({ 500.0f, 74.f });
        m_monsterWaveBackground->SetPosition({ 330.f, 485.f });
        m_monsterWaveBackground->SetAnchor({ 1.0f, 1.0f });

        ////////////////////////////////////////////////////////////////////////////////

        // 1) [상단] 날짜 및 옵션 패널 ( 일시정지, 재생, 빠른재생, 옵션 )
        m_datePanel = CreateUIObject<Image>(L"UI_ART_UIDate01");
        m_datePanel->SetTextureName("ART_UIDate01");
        m_datePanel->SetSize({ 350.0f, 166.f });
        m_datePanel->SetPosition({ 785.f, 447.f });
        m_datePanel->SetAnchor({ 1.0f, 1.0f });

        // 년 (Year) 텍스트
        m_yearText = CreateUIObject<Text>(L"UI_YearText");
        m_yearText->SetText(L"1995");
        m_yearText->SetTextFormatName("Sebang_24");
        m_yearText->SetColor(D2D1::ColorF(0x69512C));
        m_yearText->SetSize({ 300, 100 });
        m_yearText->SetPosition({ 690, 390 });

        // 월 (Month) 텍스트
        m_monthText = CreateUIObject<Text>(L"UI_MonthText");
        m_monthText->SetText(L"12");
        m_monthText->SetTextFormatName("Sebang_24");
        m_monthText->SetColor(D2D1::ColorF(0x69512C));
        m_monthText->SetSize({ 300, 100 });
        m_monthText->SetPosition({ 785, 390 });

        // 일 (Day) 텍스트
        m_dayText = CreateUIObject<Text>(L"UI_DayText");
        m_dayText->SetText(L"18");
        m_dayText->SetTextFormatName("Sebang_24");
        m_dayText->SetColor(D2D1::ColorF(0x69512C));
        m_dayText->SetSize({ 300, 100 });
        m_dayText->SetPosition({ 880, 390 });

        ////////////////////////////////////////////////////////////////////////////////

        // 정지 버튼
        m_stopButton = CreateUIObject<Button>(L"UI_StopButton");
        m_stopButton->SetTextureName("ART_Pause01");
        m_stopButton->SetText(L"");
        m_stopButton->SetSize({ 40, 47 });
        m_stopButton->SetPosition({ 679.5f, 475.f });

        // 정지 버튼 클릭하면 실행될 이벤트
        m_stopButton->AddOnClick("Quit Game", [this]()
            {

            });

        // 정지 버튼 마우스를 올리면 실행될 이벤트
        m_stopButton->AddOnEnter("Highlight On", [this]()
            {

            });

        // 정지 버튼 마우스가 벗어나면 실행될 이벤트
        m_stopButton->AddOnExit("Highlight Off", [this]()
            {

            });

        ////////////////////////////////////////////////////////////////////////////////

        // 재생 버튼
        m_playButton = CreateUIObject<Button>(L"UI_PlayButton");
        m_playButton->SetTextureName("ART_Play01");
        m_playButton->SetText(L"");
        m_playButton->SetSize({ 31.5f, 45.f });
        m_playButton->SetPosition({ 762.f, 475.f });

        // 재생 버튼 클릭하면 실행될 이벤트
        m_playButton->AddOnClick("Quit Game", [this]()
            {

            });

        // 재생 버튼 마우스를 올리면 실행될 이벤트
        m_playButton->AddOnEnter("Highlight On", [this]()
            {

            });

        // 재생 버튼 마우스가 벗어나면 실행될 이벤트
        m_playButton->AddOnExit("Highlight Off", [this]()
            {

            });

        ////////////////////////////////////////////////////////////////////////////////

        // 스피드 버튼
        m_speedButton = CreateUIObject<Button>(L"UI_SpeedButton");
        m_speedButton->SetTextureName("ART_Fast01");
        m_speedButton->SetText(L"");
        m_speedButton->SetSize({ 45.f, 45.f });
        m_speedButton->SetPosition({ 842.f, 475.f });

        // 스피드 버튼 클릭하면 실행될 이벤트
        m_speedButton->AddOnClick("Quit Game", [this]()
            {

            });

        // 스피드 버튼 마우스를 올리면 실행될 이벤트
        m_speedButton->AddOnEnter("Highlight On", [this]()
            {

            });

        // 스피드 버튼 마우스가 벗어나면 실행될 이벤트
        m_speedButton->AddOnExit("Highlight Off", [this]()
            {

            });

        ////////////////////////////////////////////////////////////////////////////////

        // 옵션 버튼
        m_optionButton = CreateUIObject<Button>(L"UI_OptionButton");
        m_optionButton->SetTextureName("ART_SettingIcon01");
        m_optionButton->SetText(L"");
        m_optionButton->SetSize({ 48.f, 50.5f });
        m_optionButton->SetPosition({ 919, 483 });
        m_optionButton->SetScale({ 0.85f, 0.85f });

        // 옵션 버튼 클릭하면 실행될 이벤트
        m_optionButton->AddOnClick("Quit Game", [this]()
            {

            });

        // 스피드 버튼 마우스를 올리면 실행될 이벤트
        m_optionButton->AddOnEnter("Highlight On", [this]()
            {

            });

        // 스피드 버튼 마우스가 벗어나면 실행될 이벤트
        m_optionButton->AddOnExit("Highlight Off", [this]()
            {

            });

        ////////////////////////////////////////////////////////////////////////////////

        // 0) 기본 UI
        // TODO : 리소스가 아직 없음.
        m_defaultUI = CreateUIObject<Image>(L"UI_Default");
        m_defaultUI->SetTextureName("ART_UIBuilding01");
        if (cam)
        {
            float screenWidth = static_cast<float>(cam->GetScreenWidth());
            float screenHeight = static_cast<float>(cam->GetScreenHeight());

            // 화면 크기로 설정
            m_defaultUI->SetSize(Vector2F{ screenWidth, screenHeight });
            m_defaultUI->SetPosition({ 0.0f, 0.0f });
            m_defaultUI->SetPivot({ 0.5f, 0.5f });
            m_defaultUI->SetAnchor({ 1.0f, 0.0f });
        }

        ////////////////////////////////////////////////////////////////////////////////

        // 1) 건설 UI
        m_buildUI = CreateUIObject<Image>(L"UI_Build");
        m_buildUI->SetTextureName("ART_UIBuilding01");
        if (cam)
        {
            float screenWidth = static_cast<float>(cam->GetScreenWidth());
            float screenHeight = static_cast<float>(cam->GetScreenHeight());

            // 화면 크기로 설정
            m_buildUI->SetSize(Vector2F{ screenWidth, screenHeight });
            m_buildUI->SetPosition({ 0.0f, 0.0f });
            m_buildUI->SetPivot({ 0.5f, 0.5f });
            m_buildUI->SetAnchor({ 1.0f, 0.0f });
        }

        ////////////////////////////////////////
        // 건설 UI _ Info
        ////////////////////////////////////////

        // 건설 타입 텍스트
        m_buildTypeText = CreateUIObject<Text>(L"UI_BuildTypeText");
        m_buildTypeText->SetText(L"광산");
        m_buildTypeText->SetTextFormatName("Sebang_Bold_24");
        m_buildTypeText->SetColor(D2D1::ColorF(0x69512C));
        m_buildTypeText->SetSize({ 300, 100 });
        m_buildTypeText->SetPosition({ -64, -390 });

        // 코스트 텍스트
        m_costInfoText = CreateUIObject<Text>(L"UI_BuildCostInfoText");
        m_costInfoText->SetText(L"코스트");
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

        // 코스트 이미지
        m_costImage = CreateUIObject<Image>(L"UI_BuildCostImage");
        m_costImage->SetTextureName("ART_CostWood01");
        m_costImage->SetSize({ 40, 31 });
        m_costImage->SetPosition({ -46, -428 });
        m_costImage->SetAnchor({ 1.0f, 0.0f });

        // 효과 텍스트
        m_effectInfoText = CreateUIObject<Text>(L"UI_BuildEffectInfoText");
        m_effectInfoText->SetText(L"효과");
        m_effectInfoText->SetTextFormatName("Sebang_16");
        m_effectInfoText->SetColor(D2D1::ColorF(0x69512C));
        m_effectInfoText->SetSize({ 300, 100 });
        m_effectInfoText->SetPosition({ -113, -470 });

        m_effectText = CreateUIObject<Text>(L"UI_BuildEffectText");
        m_effectText->SetText(L"1/초");
        m_effectText->SetTextFormatName("Sebang_16");
        m_effectText->SetColor(D2D1::ColorF(0x69512C));
        m_effectText->SetSize({ 300, 100 });
        m_effectText->SetPosition({ 0, -470 });

        // 효과 이미지
        m_effctImage = CreateUIObject<Image>(L"UI_BuildEffctImage");
        m_effctImage->SetTextureName("ART_CostFood01");
        m_effctImage->SetSize({ 40, 31 });
        m_effctImage->SetPosition({ -46, -470 });
        m_effctImage->SetAnchor({ 1.0f, 0.0f });

        ////////////////////////////////////////
        // 건설 UI _ 주거지
        ////////////////////////////////////////
        m_buildHouse = CreateUIObject<Button>(L"UI_Build_House");
        m_buildHouse->SetTextureName("ART_BuildCabin01");
        m_buildHouse->SetText(L"");
        m_buildHouse->SetSize({ 166.f, 166.f });
        m_buildHouse->SetPosition({ 200.f, -430.f });

        // 주거지 버튼 클릭하면 실행될 이벤트
        m_buildHouse->AddOnClick("Quit Game", [this]()
            {

            });

        // 주거지 버튼 마우스를 올리면 실행될 이벤트
        m_buildHouse->AddOnEnter("Highlight On", [this]()
            {
                ShowBuildInfo(JDGlobal::Contents::BuildingType::House, "50", "1/초");
            });

        // 주거지 버튼 마우스가 벗어나면 실행될 이벤트
        m_buildHouse->AddOnExit("Highlight Off", [this]()
            {
                CloseBuildInfo();
            });


        ////////////////////////////////////////
        // 건설 UI _ 낚시터
        ////////////////////////////////////////
        m_buildFishingspot = CreateUIObject<Button>(L"UI_Build_Fishing");
        m_buildFishingspot->SetTextureName("ART_BuildFishing01");
        m_buildFishingspot->SetText(L"");
        m_buildFishingspot->SetSize({ 166.f, 166.f });
        m_buildFishingspot->SetPosition({ 400.f, -430.f });

        // 낚시터 버튼 클릭하면 실행될 이벤트
        m_buildFishingspot->AddOnClick("Quit Game", [this]()
            {

            });

        // 낚시터 버튼 마우스를 올리면 실행될 이벤트
        m_buildFishingspot->AddOnEnter("Highlight On", [this]()
            {
                ShowBuildInfo(JDGlobal::Contents::BuildingType::FishingSpot, "40", "2/초");
            });

        // 낚시터 버튼 마우스가 벗어나면 실행될 이벤트
        m_buildFishingspot->AddOnExit("Highlight Off", [this]()
            {
                CloseBuildInfo();
            });


        ////////////////////////////////////////
        // 건설 UI _ 제재소
        ////////////////////////////////////////
        m_buildRumbermill = CreateUIObject<Button>(L"UI_Build_Lumbermill");
        m_buildRumbermill->SetTextureName("ART_BuildLumbermill01");
        m_buildRumbermill->SetText(L"");
        m_buildRumbermill->SetSize({ 166.f, 166.f });
        m_buildRumbermill->SetPosition({ 600.f, -430.f });

        // 제재소 버튼 클릭하면 실행될 이벤트
        m_buildRumbermill->AddOnClick("Quit Game", [this]()
            {

            });

        // 제재소 버튼 마우스를 올리면 실행될 이벤트
        m_buildRumbermill->AddOnEnter("Highlight On", [this]()
            {
                ShowBuildInfo(JDGlobal::Contents::BuildingType::LumberMill, "30", "3/초");
            });

        // 제재소 버튼 마우스가 벗어나면 실행될 이벤트
        m_buildRumbermill->AddOnExit("Highlight Off", [this]()
            {
                CloseBuildInfo();
            });


        ////////////////////////////////////////
        // 건설 UI _ 광산
        ////////////////////////////////////////
        m_buildMine = CreateUIObject<Button>(L"UI_Build_Mine");
        m_buildMine->SetTextureName("ART_BuildMine01");
        m_buildMine->SetText(L"");
        m_buildMine->SetSize({ 166.f, 166.f });
        m_buildMine->SetPosition({ 800.f, -430.f });

        // 광산 버튼 클릭하면 실행될 이벤트
        m_buildMine->AddOnClick("Quit Game", [this]()
            {

            });

        // 광산 버튼 마우스를 올리면 실행될 이벤트
        m_buildMine->AddOnEnter("Highlight On", [this]()
            {
                ShowBuildInfo(JDGlobal::Contents::BuildingType::Mine, "20", "4/초");
            });

        // 광산 버튼 마우스가 벗어나면 실행될 이벤트
        m_buildMine->AddOnExit("Highlight Off", [this]()
            {
                CloseBuildInfo();
            });

        ////////////////////////////////////////////////////////////////////////////////

        // 2) 건물 업그레이트 및 고양이 배치 UI
        m_upgradeUI = CreateUIObject<Image>(L"UI_Upgrade");
        m_upgradeUI->SetTextureName("ART_UICharSelect01");
        if (cam)
        {
            float screenWidth = static_cast<float>(cam->GetScreenWidth());
            float screenHeight = static_cast<float>(cam->GetScreenHeight());

            // 화면 크기로 설정
            m_upgradeUI->SetSize(Vector2F{ screenWidth, screenHeight });
            m_upgradeUI->SetPosition({ 0.0f, 0.0f });
            m_upgradeUI->SetPivot({ 0.5f, 0.5f });
            m_upgradeUI->SetAnchor({ 1.0f, 0.0f });
        }

        // 3) 징병 및 원정 UI
        m_expeditionUI = CreateUIObject<Image>(L"UI_Expedition");
        m_expeditionUI->SetTextureName("ART_UIRecruit01");
        if (cam)
        {
            float screenWidth = static_cast<float>(cam->GetScreenWidth());
            float screenHeight = static_cast<float>(cam->GetScreenHeight());

            // 화면 크기로 설정
            m_expeditionUI->SetSize(Vector2F{ screenWidth, screenHeight });
            m_expeditionUI->SetPosition({ 0.0f, 0.0f });
            m_expeditionUI->SetPivot({ 0.5f, 0.5f });
            m_expeditionUI->SetAnchor({ 1.0f, 0.0f });
        }

        //// 2) 필터
        //m_fillter = CreateUIObject<Image>(L"Fillter_Image");
        //m_fillter->SetTextureName("BACK_GROUND_OPACITY");
        //m_fillter->SetColor(D2D1::ColorF(D2D1::ColorF::White, 0.65f));

        //if (cam)
        //{
        //    float screenWidth = static_cast<float>(cam->GetScreenWidth());
        //    float screenHeight = static_cast<float>(cam->GetScreenHeight());

        //    // 화면 크기로 설정
        //    m_fillter->SetSize(Vector2F{ screenWidth, screenHeight });
        //    m_fillter->SetPosition({ 0.0f,0.0f });
        //    m_fillter->SetPivot({ 0.5f, 0.5f });
        //}

        //// 3) 대사
        //m_narration = CreateUIObject<Text>(L"Narration_Text");
        //// 튜토리얼 이벤트로 넘겼음.
        ///*m_narration->SetText(
        //    L"\"의 새로운 집사로 취임한 것을 환영합니다.\"\n"
        //    L"당신은 건물을 지어 마을을 키우고 도시를 지켜내야 합니다.\n"
        //);*/
        //m_narration->SetTextFormatName("Sebang_40");
        //m_narration->SetColor(D2D1::ColorF(0.839f, 0.741f, 0.580f));
        //m_narration->SetSize({ 1600, 300 });
        //m_narration->SetPosition({ 0, 320 });


        //// 4) 그리드 클릭하면 나오는 메뉴
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

        ////각버튼 키 목록
        //std::vector<std::pair<std::wstring, std::string>> EmptyButtons = {
        //{ L"FishingSpot", "fishing" },
        //{ L"LumberMill", "lumbermill" },
        //{ L"Mine", "mine" },
        //{ L"House", "cabin" },
        //// 필요하다면 더 추가…
        //};
        //std::vector<std::pair<std::wstring, std::string>> FilledButtons = {
        //{ L"CAT", "CAT" },
        //// 필요하다면 더 추가…
        //};

        //// 공통 초기 오프셋
        //float xOffset = 0.f;
        //float yOffset = -150.f;
        //float gap = 100.f;

        //// 2-1) 빈 메뉴용 버튼 생성
        //{
        //    float x = xOffset;
        //    float y = yOffset;
        //    for (auto& info : EmptyButtons) {
        //        auto* btn = CreateUIObject<Button>(info.first);
        //        // 튜토리얼용 보관
        //        m_TutorialUIObjects.push_back(m_uiObjects.back().get());
        //        m_emptyButtons.push_back(btn);

        //        btn->SetTextureName(info.second);
        //        btn->SetText(L"");
        //        auto* rt = btn->GetComponent<RectTransform>();

        //        // [수정] RectTransform의 SetParent로만 부모-자식 관계를 설정합니다.
        //        rt->SetParent(bgRect);

        //        // [수정] 피벗을 중앙으로 설정하여 위치를 잡기 쉽게 합니다.
        //        rt->SetPivot({ 0.0f, 0.0f });
        //        rt->SetAnchor({ 0.0f, 0.0f });
        //        rt->SetSize({ 50, 50 });

        //        // 이제 이 Position은 버튼의 '중앙'을 부모의 (x, yOffset)으로 이동시킵니다.
        //        rt->SetPosition({ x, y });


        //        // LEGACY : 과거 코드
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

        //// 2-2) 채운 메뉴용 버튼 생성
        //{
        //    float x = xOffset;
        //    for (auto& info : FilledButtons) {
        //        auto* btn = CreateUIObject<Button>(info.first);
        //        m_TutorialUIObjects.push_back(m_uiObjects.back().get());
        //        m_filledButtons.push_back(btn);

        //        btn->SetTextureName(info.second);
        //        btn->SetText(L"");
        //        auto* rt = btn->GetComponent<RectTransform>();

        //        // [수정] RectTransform의 SetParent로만 부모-자식 관계를 설정합니다.
        //        rt->SetParent(bgRect);

        //        // [수정] 피벗을 중앙으로 설정하여 위치를 잡기 쉽게 합니다.
        //        rt->SetPivot({ 0.5f, 0.5f });
        //        rt->SetAnchor({ 0.0f, 0.0f });
        //        rt->SetSize({ 50, 50 });

        //        // 이제 이 Position은 버튼의 '중앙'을 부모의 (x, yOffset)으로 이동시킵니다.
        //        rt->SetPosition({ x, yOffset });

        //        // LEGACY : 과거 코드
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

        // 기본 UI 보이기
        InitBuildMenu();
    }

    void GameScene::InitBuildMenu()
    {
        // 기본 UI 보이기
        m_defaultUI->SetActive(true);
        m_buildUI->SetActive(false);
        m_upgradeUI->SetActive(false);
        m_expeditionUI->SetActive(false);

        // 건설 UI ( 주거지, 낚시터, 제재소, 광산, (연구실) )
        m_buildHouse->SetActive(false);
        m_buildFishingspot->SetActive(false);
        m_buildRumbermill->SetActive(false);
        m_buildMine->SetActive(false);

        // 건설 Info
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
        // 건설 UI 보이기
        m_defaultUI->SetActive(false);
        m_buildUI->SetActive(true);
        m_upgradeUI->SetActive(false);
        m_expeditionUI->SetActive(false);

        // 건설 UI ( 주거지, 낚시터, 제재소, 광산, (연구실) )
        m_buildHouse->SetActive(true);
        m_buildFishingspot->SetActive(true);
        m_buildRumbermill->SetActive(true);
        m_buildMine->SetActive(true);

        // 건설 Info
        CloseBuildInfo();
    }

    void GameScene::CloseBuildMenu()
    {
        // 건설 UI 끄기
        m_defaultUI->SetActive(true);
        m_buildUI->SetActive(false);
        m_upgradeUI->SetActive(false);
        m_expeditionUI->SetActive(false);

        // 건설 UI ( 주거지, 낚시터, 제재소, 광산, (연구실) )
        m_buildHouse->SetActive(false);
        m_buildFishingspot->SetActive(false);
        m_buildRumbermill->SetActive(false);
        m_buildMine->SetActive(false);

        // 건설 Info
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
        // 건설 Info
        m_buildTypeText->SetActive(true);
        m_buildTypeText->SetText(L"건물 정보");

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
            m_buildTypeText->SetText(L"주거지");
            break;
        case JDGlobal::Contents::BuildingType::FishingSpot:
            m_buildTypeText->SetText(L"낚시터");
            break;
        case JDGlobal::Contents::BuildingType::LumberMill:
            m_buildTypeText->SetText(L"제재소");
            break;
        case JDGlobal::Contents::BuildingType::Mine:
            m_buildTypeText->SetText(L"광산");
            break;
        case JDGlobal::Contents::BuildingType::Lab:
            m_buildTypeText->SetText(L"연구소");
            break;
        }
    }

    void GameScene::ShowFilledMenu() {
        m_Menu->SetActive(true);
        // 빈 메뉴 버튼들 끄고
        for (auto* btn : m_emptyButtons)   btn->SetActive(false);
        // 채운 메뉴 버튼들 켜기
        for (auto* btn : m_filledButtons)  btn->SetActive(true);
    }
}
