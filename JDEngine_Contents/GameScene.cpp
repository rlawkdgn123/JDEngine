#include "pch.h"
#include "framework.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "RectTransform.h"
#include "D2DTransform.h"
#include "Texture.h"
#include "BuildSystem.h"
#include "GameScene.h"
#include "CombatSystem.h"
#include "ExpeditionSystem.h"
#include "TextRenderer.h"
#include "TitleScene.h"
#include "SceneManager.h"

using namespace std;
using namespace JDGameObject::Content;
using JDComponent::AnimationRender;
using JDComponent::D2DTM::RectTransform;
using JDComponent::TextureRenderer;
using Direction = JDGlobal::Contents::Direction;
using BuildingType = JDGlobal::Contents::BuildingType;
namespace JDScene {

    // TestScene
    void GameScene::OnEnter() {
        //system("cls");
        using namespace JDGameObject;
        using namespace JDComponent;
        using JDScene::GameScene;

        m_dataTableManager = &DataTableManager::Instance();
        m_buildSystem = make_unique<BuildSystem>();

        /*for (int level = 0; level < JDGlobal::Contents::MAX_GAME_LEVEL; ++level) {
            cout << "[Level " << level << "] InitPopulation = " << m_dataTableManager->GetHouseTableInfo().m_initPopulation[level] << '\n';
        }*/

        // 게임 맵 생성
        CreateGameMap();
        CreateBarrackUI();
        CreateOptionUI();
        //CreateFillter();

        //// 1) 배경
        //auto battleMap = CreateUIObject<Image>(L"BATTLE_MAP");
        //battleMap->SetTextureName("BATTLE_MAP(old)");

        //if (cam)
        //{
        //    float screenWidth = static_cast<float>(cam->GetScreenWidth());
        //    float screenHeight = static_cast<float>(cam->GetScreenHeight());

        //    // 화면 크기로 설정
        //    battleMap->SetSize(Vector2F{ screenWidth, screenHeight });
        //    battleMap->SetColor(D2D1::ColorF(D2D1::ColorF::White, 0.25f));
        //    battleMap->SetPosition({ 0.0f,0.0f });
        //    battleMap->SetPivot({ 0.5f, 0.5f });
        //}

        m_showedDays.clear();
        m_elapsedTime = 0.0f;
        WaveManager::Instance().LoadStageWaves();
        m_date = WaveManager::Instance().GetConvertedDate();

        // 자원 세팅.
        auto& rs = ResourceSystem::Instance();
        rs.SetTotalResource(Resource(1000, 1000, 1000));
        rs.SetMaxPopulation(500);
        rs.SetCurPopulation(100);

        m_playerArmy.OverrideUnitCounts({ 100, 100 });

        // 병영.
        m_barracksObject = CreateStructure(L"barracksObj", JDGlobal::Base::GameTag::Barracks, { -197.f, 144.f }, "ART_Barracks02_mouse_over");

        // 성벽.
        // m_wallObject = CreateStructure(L"wallObj", JDGlobal::Base::GameTag::Wall, { -300.0f, 75.0f }, "house");

        // 엔딩 여부.
        m_isEnding = false;

        /////////////////////////////////////////////////////////////////////////////
        AudioManager::Instance().PlayBGM("BGM_Battle", &bgmChannel);
        // AudioManager::Instance().PlayBGM("BGM_Fiield", &bgmChannel);

        DateUIUpdate();
        UpdateBarrackUI();
        UpdateAttackPowerText();
        UpdateAwayPointUI();
        CreateEndingUI();
        ChangeAwayCatImage();
    }

    void GameScene::OnLeave() {
        //cout << "[TestScene] OnLeave()\n";
        if (m_buildSystem) {
            m_buildSystem->DestroyGrid(this);
        }

        if (bgmChannel) {
            bgmChannel->stop(); // FMOD에서 채널을 멈춤
            bgmChannel = nullptr; // 포인터도 초기화 (안전)
        }

        // 선택된 오브젝트 포인터 초기화
        SetSelectedObject(nullptr);

        // 오브젝트 제거
        for (const auto& gameObject : m_gameObjects)
        {
            DestroyObject(gameObject.get());
        }
        for (const auto& uiObject : m_uiObjects)
        {
            DestroyObject(uiObject.get());
        }

        FinalizeGameScene();
    }

    void GameScene::Update(float deltaTime) {
        SceneBase::Update(deltaTime);

        // 애니 진행
        if (m_endAnimStarted) {

            float animSpeed = 0.8f;
            m_endAnimTime += deltaTime * animSpeed;
            
            auto Clamp01 = [](float x) { return x < 0.f ? 0.f : (x > 1.f ? 1.f : x); };
            auto Lerp = [](const Vector2F& a, const Vector2F& b, float k)->Vector2F {
                return { a.x + (b.x - a.x) * k, a.y + (b.y - a.y) * k };
                };

            // 이징을 모두 "float(float)" 형태로 래핑 ( EaseOutBack은 인자 2개라 래핑 필요)
            auto easeBack07 = [](float t) { return EaseOutBack(t, 0.7f); };
            auto easeBack10 = [](float t) { return EaseOutBack(t, 1.0f); };
            auto easeBack03 = [](float t) { return EaseOutBack(t, 0.3f); };
            auto easeCubic = [](float t) { return EaseOutCubic(t);      };
            auto easeElasticS = [](float t) { return EaseOutElasticSmall(t); };

            // 공통 평가 함수
            auto EvalPos = [&](const Vector2F& s, const Vector2F& d,
                float delay, float dur,
                auto&& ease)->Vector2F
                {
                    float t = Clamp01((m_endAnimTime - delay) / dur); // 요소별 딜레이/지속시간
                    float e = ease(t);                                // 요소별 이징
                    return Lerp(s, d, e);
                };

            // 각자 다른 속도·딜레이·이징
            const Vector2F posEnd = EvalPos(m_endStartPos, m_endTargetPos, m_delayEnd, m_durEnd, easeBack07);   // 살짝 튕김
            const Vector2F posRetry = EvalPos(m_retryStartPos, m_retryTargetPos, m_delayRetry, m_durRetry, easeCubic);    // 기본
            const Vector2F posMenu = EvalPos(m_menuStartPos, m_menuTargetPos, m_delayMenu, m_durMenu, easeBack07); // 더 탱탱

            m_endingUI->SetPosition(posEnd);
            m_retry->SetPosition(posRetry);
            m_goMenu->SetPosition(posMenu);

            // 모두 완료됐는지 체크(각자의 딜레이/지속시간 기준)
            const float tEnd = Clamp01((m_endAnimTime - m_delayEnd) / m_durEnd);
            const float tRetry = Clamp01((m_endAnimTime - m_delayRetry) / m_durRetry);
            const float tMenu = Clamp01((m_endAnimTime - m_delayMenu) / m_durMenu);

            if (tEnd >= 1.f && tRetry >= 1.f && tMenu >= 1.f) {
                m_endingUI->SetPosition(m_endTargetPos);
                m_retry->SetPosition(m_retryTargetPos);
                m_goMenu->SetPosition(m_menuTargetPos);
                m_endAnimStarted = false;
                m_endAnimDone = true;

                m_spear->SetActive(true);
                if (m_spearAnim) {
                    m_spearAnim->SetEnabled(true);
                    m_spearAnim->PlayOnceFromStart(); // ← 여기서 한 번만!
                }
            }
        }

        

        if(ResolveGameEnding()) return; // 엔딩 관리.

        UpdateResourceUI();

        ////////////////////////////////////////////////////////////////////////////////

        // 무슨 코드임???
        //float speed = 100.f;  // 픽셀/초
        //Vector2F delta{ speed * deltaTime, 0.f };

        //for (auto& objPtr : m_gameObjects) {

        //    auto* sfx = objPtr->GetComponent<JDComponent::SFX>();
        //    if (!sfx)
        //        continue;

        //    auto* tm = objPtr->GetComponent<Transform>();
        //    if (!tm)
        //        continue;

        //    if (objPtr->GetTag() != JDGlobal::Base::GameTag::Mover)
        //        continue;

        //    Vector2F oldPos = tm->GetPosition();
        //    Vector2F newPos{ oldPos.x + delta.x, oldPos.y + delta.y };
        //    tm->SetPosition(newPos);

        //    // 5) 디버그: SFX 가진 녀석인지, 이동 로그 출력
        //    if (newPos.x != oldPos.x || newPos.y != oldPos.y)
        //    {
        //        //sfx->Play();

        //        // 디버그: 소리 재생 로그
        //        /*std::wcout
        //            << L"[DEBUG] SFX Play at mover: key="
        //            << L", from (" << oldPos.x << L"," << oldPos.y
        //            << L") to (" << newPos.x << L"," << newPos.y
        //            << L")\n";*/
        //    }
        //}


        /*bool left = InputManager::Instance().GetMouseState().leftClicked;
        bool right = InputManager::Instance().GetMouseState().rightClicked;
        auto mousePos = GetMouseWorldPos();

        for (auto& objPtr : m_gameObjects) {
            auto* collider = objPtr->GetComponent<JDComponent::ColliderBase>();
            if (!collider) continue;

            auto* grid = dynamic_cast<Grid*>(objPtr.get());
            if (!grid)
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

                m_selectedCollider = collider;

                auto* boxCol = static_cast<JDComponent::BoxCollider*>(collider);
                if (boxCol->HasBuilding()) {
                    ShowFilledMenu();
                }
                else {
                    ShowEmptyMenu();
                }
            }
        }*/

        MouseState state = InputManager::Instance().GetMouseState();
        float mouseX = static_cast<float>(state.pos.x);
        float mouseY = static_cast<float>(state.pos.y);
        if (m_lightParticles) {
            m_lightParticles->Update(deltaTime, Vector2F{ mouseX, mouseY });
            m_lightParticles->Emit(Vector2F{ mouseX, mouseY }, 30, D2D1::ColorF(0.0f, 0.0f, 1.0f), 2.5f);
        }

        ProcessDayTimer(deltaTime); // 날짜 및 웨이브 관리.
        ProcessBattle(deltaTime); // 전투 지속시간 및 전투 결과 관리. 

        // 클릭 업데이트
        ClickUpdate();

        MoveEnemyObjects(deltaTime); // 적 이동.
        MovePlayerObjects(deltaTime); // 아군 이동.
        MoveExpedition(deltaTime); // 원정대 이동,

        AttackWall(deltaTime); // 적이 성 공격하는 것 관리.

        // 자원 시스템 업데이트
        ResourceSystem::Instance().Update(deltaTime);

        // 배속 버튼 관리
        GameSpeedButtonUpdate();


        // 따라다니는 전투력
        UpdateAttachments(deltaTime);

        ///// Test
        if (m_selectedCollider) {
            auto* boxCol = static_cast<JDComponent::BoxCollider*>(m_selectedCollider);
            Grid* grid = dynamic_cast<Grid*>(boxCol->GetOwner());

            if (!grid) return;

            // 주거지 체크.
            auto* house = dynamic_cast<House*>(grid->GetBuilding());
            auto* building = dynamic_cast<Building*>(grid->GetBuilding());
            if (house)
            {
                int nextLevel = house->GetLevel() + 1;
                if (nextLevel < JDGlobal::Contents::MAX_GAME_LEVEL) {
                    auto& cost = house->GetHouseStats().m_upgradeCost[nextLevel];
                    m_upgradeCostText->SetText(to_wstring(cost.m_wood));
                }
            }
            else if (building) {
                int nextLevel = building->GetLevel() + 1;
                if (nextLevel < JDGlobal::Contents::MAX_GAME_LEVEL) {
                    auto& cost = building->GetBuildingStats().m_upgradeCost[nextLevel];
                    m_upgradeCostText->SetText(to_wstring(cost.m_wood));
                }
            }
        }

        ShowClickedBarrackObj();
    }

    void GameScene::FixedUpdate(float fixedDeltaTime) {
        SceneBase::FixedUpdate(fixedDeltaTime);
        //cout << "[TestScene] FixedUpdate() - dt: " << fixedDeltaTime << "\n";
    }

    void GameScene::LateUpdate(float deltaTime) {
        SceneBase::LateUpdate(deltaTime);
        //cout << "[TestScene] LateUpdate() - dt: " << deltaTime << "\n";
    }

    void GameScene::Render(float deltaTime) {
        SceneBase::Render(deltaTime);

        auto& renderer = D2DRenderer::Instance();
        auto ctx = renderer.GetD2DContext();

        m_fader.Update(deltaTime);

        auto camera = D2DRenderer::Instance().GetCamera();
        float screenW = static_cast<float>(camera->GetScreenWidth());
        float screenH = static_cast<float>(camera->GetScreenHeight());

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
        // 카메라 뷰가 걸려 있는 상태이므로, 
        // 페이드를 스크린 공간에서 그리려면 Transform을 초기화
        
        m_fader.Render(ctx, { (float)screenW, (float)screenH });

        for (auto& obj : m_gameObjects) {
            auto name = obj->GetName(); // std::wstring / std::wstring_view 가정
            if (name == L"SPEAR")
            {
                D2DRenderer::Instance().RenderGameObject(*obj, deltaTime);
            }
        }

        for (auto& uiObj : m_uiObjects) {
            
            auto name = uiObj->GetName(); // std::wstring / std::wstring_view 가정
            if (name == L"ART_NomalEnd01"
                || name == L"ART_BACK_TO_MENU01"
                || name == L"ART_BACK_TO_MENU02"
                || name == L"ART_RETRY01"
                || name == L"ART_RETRY02"
                || name == L"RETRY_BUTTON"
                || name == L"MENU_BUTTON")
            {
                D2DRenderer::Instance().RenderUIObject(*uiObj);
            }
        }
        //// 현재 Transform(=카메라 뷰)을 잠시 저장
        //D2D1_MATRIX_3X2_F old;
        //ctx->GetTransform(&old);

        //// 스크린 좌표계(Identity) 로 바꿔서 페이드만 그리기
        //ctx->SetTransform(D2D1::Matrix3x2F::Identity());
        //

        //// 원래 Transform(카메라 뷰) 복원
        //ctx->SetTransform(old);

        // 얘는 왜 좌표계 복원 뒤에 한거지??
        /* for (auto& tutorialObj : m_TutorialUIObjects)
        {
            D2DRenderer::Instance().RenderUIObject(*uiObj);
        }

        for (auto& tutorialObj : m_TutorialObjects)
        for (auto& gameObj : m_gameObjects)
        {
            D2DRenderer::Instance().RenderGameObject(*gameObj, deltaTime);
        }*/

        if (m_lightParticles) {
            // 스크린 좌표로 바로 그릴 거면 Transform 초기화 후
            auto ctx = D2DRenderer::Instance().GetD2DContext();
            D2D1_MATRIX_3X2_F old;
            ctx->GetTransform(&old);
            ctx->SetTransform(D2D1::Matrix3x2F::Identity());

            m_lightParticles->Render(ctx);

            ctx->SetTransform(old);
        }

        m_buildSystem->UpdateTextureObj(m_selectedCollider);
    }
    void GameScene::ProcessDayTimer(float deltaTime)
    {
        m_elapsedTime += deltaTime;
        if (m_battleObject) m_btlElapsedTime += deltaTime;

        if (m_elapsedTime >= m_dayTime) {
            m_elapsedTime = 0.0f;
            WaveManager::Instance().AdvanceDay();
            m_date = WaveManager::Instance().GetConvertedDate();
            std::cout << "[GameScene] 현재 날짜: " << m_date.year << "년 " << m_date.month << "월 " << m_date.day << "일 " << std::endl;
            DateUIUpdate(); // 날짜 텍스트 업데이트.

            int power = WaveManager::Instance().GetWaveEnemyPower();
            if (power >= 0) {
                SpawnNextWaveIndicator(power);
            }

            AdvanceNextWaveIndicators(); // 웨이브 표시 옮기기.

            const int currDay = WaveManager::Instance().GetCurrDay();
            //const int warningDay = currDay + 2;
            const int warningDay = currDay; // 당일에 화면에 나타나게.

            std::cout << "[GameScene] 날짜 증가: " << currDay << std::endl;

            // 웨이브가 있는 경우, 아직 화면에 나타나지 않았으면 생성.
            if (WaveManager::Instance().GetWave(warningDay) && !IsEnemySpawned(warningDay)) {
                m_enemyArmy.OverrideUnitCounts(WaveManager::Instance().GetWave(warningDay)->enemyUnits);
                SpawnWaveEnemy({ 1010.0f, 230.0f });
                AddEnemyDay(warningDay);
                std::cout << "[GameScene] 적 스폰됨." << std::endl;
            }
        }
    }
    void GameScene::ProcessBattle(float deltaTime)
    {
        if (m_btlElapsedTime < m_battleTime) return;

        JDGameSystem::CombatSystem combat;
        JDGameSystem::UnitCounts playerResult;
        JDGameSystem::UnitCounts enemyResult;

        bool isWin = combat.ResolveBattle(
            m_playerBattleArmy,
            m_playerTotalPower,
            m_enemyBattleArmy,
            m_enemyTotalPower,
            playerResult,
            enemyResult
        );

        if (isWin) { 
            BattleReward();
        }

        m_playerBattleArmy = { 0 , 0 };
        m_playerTotalPower = 0;
        m_enemyBattleArmy = { 0 , 0 };
        m_enemyTotalPower = 0;

        if (playerResult.Total() > 0) {
            auto* playerObj = CreateSoldierUnit(playerResult, JDGlobal::Base::GameTag::Player,
                JDGlobal::Contents::State::Back, m_battleObject->GetComponent<Transform>()->GetPosition(), "ART_NaviAdv_Sprite01");
        }
        if (enemyResult.Total() > 0) {
            auto* enemyObj = CreateSoldierUnit(enemyResult, JDGlobal::Base::GameTag::Enemy,
                JDGlobal::Contents::State::Move, m_battleObject->GetComponent<Transform>()->GetPosition(), "ART_NaviAdv_Sprite01");

        }

        m_btlElapsedTime = 0.0f;
        SafeDestroy(m_battleObject);
        std::cout << "[GameScene] 전투 끝." << std::endl;
    }
    void GameScene::BattleReward()
    {
        m_buildSystem->AddChoiceCount(1); // 건물 선택권.
        std::cout << "[GameScene] WIN!!!!!!!!!!!!!!!" << std::endl;
    }
    void GameScene::SpawnWaveEnemy(const Vector2F& pos)
    {
        auto* enemyObj = CreateSoldierUnit(m_enemyArmy.GetUnitCounts(), JDGlobal::Base::GameTag::Enemy,
            JDGlobal::Contents::State::Move, pos, "ART_NaviAdv_Sprite01");

        m_enemyArmy.OverrideUnitCounts({ 0, 0 });
    }
    void GameScene::SpawnPlayerArmy(const Vector2F& pos)
    {
        auto* playerObj = CreateSoldierUnit(m_playerArmy.GetUnitCounts(), JDGlobal::Base::GameTag::Player,
            JDGlobal::Contents::State::Move, pos, "ART_NaviAdv_Sprite01");
        m_playerArmy.OverrideUnitCounts({ 0, 0 });
        UpdateAttackPowerText();
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

        for (auto* objPtr : m_enemies) {
            if (!objPtr || !objPtr->IsActive()) continue;
            if (objPtr->GetState() != JDGlobal::Contents::State::Move) continue;

            auto* transform = objPtr->GetComponent<Transform>();
            if (!transform) continue;
            auto* sfx = objPtr->GetComponent<JDComponent::SFX>();

            // 적이 전투 오브젝트와 충돌하면 전투 시간 초기화하고 제거.
            if (m_battleObject) {
                auto* battleTm = m_battleObject->GetComponent<Transform>();
                if (!battleTm)  continue;
                Vector2F diffPos = battleTm->GetPosition() - transform->GetPosition();

                if (diffPos.Length() <= 10.0f) {
                    auto* soldier = dynamic_cast<JDGameObject::Content::Soldier*>(objPtr);
                    if (soldier) {
                        m_enemyBattleArmy += soldier->GetUnitCounts();
                        m_enemyTotalPower += soldier->GetPower();
                        std::cout << "[GameScene] 적 병력 추가." << std::endl;
                    }
                    SafeDestroy(objPtr);
                    m_btlElapsedTime = 0.0f;
                    continue;
                }
            }

            Vector2F diffPos = m_wallPos - transform->GetPosition();

            if (diffPos.Length() <= 10.0f) {
                if (m_playerArmy.GetTotalUnits() > 0) {
                    m_targetEnemy = objPtr;
                    SpawnPlayerArmy(m_wallPos);
                    continue;
                }

                objPtr->SetState(JDGlobal::Contents::State::Idle);

                auto it = std::find_if(m_attackers.begin(), m_attackers.end(),
                    [&](auto& a) { return a.enemy == objPtr; });
                if (it == m_attackers.end()) { m_attackers.push_back({ objPtr, 0.0f }); }
                continue;
            }
            wallDir = diffPos.Normalized();

            // 이동 적용.
            Vector2F delta = wallDir * (speed * deltaTime);
            Vector2F newPos = transform->GetPosition() + delta;
            transform->SetPosition(newPos);

            if (sfx) sfx->Play();  // 이동 시 효과음 재생
        }
    }

    void GameScene::MovePlayerObjects(float deltaTime)
    {
        const float speed = 100.0f;
        const float backSpeed = 100.0f;
        Vector2F delta;

        for (auto* objPtr : m_players) {
            if (!objPtr || !objPtr->IsActive()) continue;
            auto* transform = objPtr->GetComponent<Transform>();
            if (!transform) continue;
            auto* sfx = objPtr->GetComponent<JDComponent::SFX>();

            // 아군이 전투 오브젝트와 충돌하면 전투 시간 초기화하고 제거.
            if (m_battleObject) {
                auto* battleTm = m_battleObject->GetComponent<Transform>();
                if (!battleTm) continue;
                Vector2F diffPos = battleTm->GetPosition() - transform->GetPosition();

                if (diffPos.Length() <= 10.0f) {
                    auto* soldier = dynamic_cast<JDGameObject::Content::Soldier*>(objPtr);
                    if (soldier) {
                        m_playerBattleArmy += soldier->GetUnitCounts();
                        m_playerTotalPower += soldier->GetPower();
                        std::cout << "[GameScene] 아군 병력 추가." << std::endl;
                    }
                    SafeDestroy(objPtr);
                    m_btlElapsedTime = 0.0f;
                    continue;
                }
            }

            // 최초 아군일 경우, 타겟 적과의 충돌 체크. 충돌하면 제거.
            if (objPtr == m_playerObject && m_targetEnemy && m_playerObject) {
                auto* enemyTm = m_targetEnemy->GetComponent<Transform>();
                if (!enemyTm) continue;
                Vector2F diffPos = enemyTm->GetPosition() - transform->GetPosition();

                if (diffPos.Length() <= 10.0f) {
                    Vector2F pPos = transform->GetPosition();
                    Vector2F ePos = enemyTm->GetPosition();
                    Vector2F mid = { (pPos.x + ePos.x) * 0.5f, (pPos.y + ePos.y) * 0.5f };
                    auto* player = dynamic_cast<JDGameObject::Content::Soldier*>(objPtr);
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
            if (!m_battleObject && objPtr != m_playerObject) {
                if (!m_barracksObject) continue;
                auto* texRenderer = objPtr->GetComponent<TextureRenderer>();
                if (texRenderer) {
                    texRenderer->SetFlipX(true); // 좌우 반전!
                }
                /*auto* barracksTm = m_barracksObject->GetComponent<Transform>();
                if (!barracksTm) continue;
                Vector2F diffPos = barracksTm->GetPosition() - transform->GetPosition();*/
                Vector2F diffPos = m_wallPos - transform->GetPosition();

                if (diffPos.Length() <= 10.0f) {
                    auto* soldier = dynamic_cast<JDGameObject::Content::Soldier*>(objPtr);
                    if (soldier) {
                        JDGameSystem::UnitCounts updated = soldier->GetUnitCounts();
                        updated += m_playerArmy.GetUnitCounts();
                        m_playerArmy.OverrideUnitCounts(updated);
                        UpdateAttackPowerText();
                        std::cout << "[GameScene] 병영 합류." << std::endl;
                    }
                    SafeDestroy(objPtr);
                    continue;
                }

                objPtr->SetState(JDGlobal::Contents::State::Back);
            }

            // 이동.
            Vector2F direction = { 1.0f, 0.0f }; // 기본값.
            float moveSpeed = speed;

            if (objPtr->GetState() == JDGlobal::Contents::State::Back) {
                // 병영 방향으로 후퇴
                /*if (m_barracksObject) {
                    auto* barracksTm = m_barracksObject->GetComponent<Transform>();
                    if (barracksTm) {
                        Vector2F diff = barracksTm->GetPosition() - transform->GetPosition();
                        direction = diff.Normalized();
                        moveSpeed = backSpeed;
                    }
                }*/
                Vector2F diff = m_wallPos - transform->GetPosition();
                direction = diff.Normalized();
                moveSpeed = backSpeed;
            }
            else if (objPtr == m_playerObject && m_targetEnemy) {
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
    void GameScene::MoveExpedition(float deltaTime)
    {
        const float speed = 100.0f;

        if (!m_expeditionObject) return;

        auto* transform = m_expeditionObject->GetComponent<Transform>();
        auto* sfx = m_expeditionObject->GetComponent<JDComponent::SFX>();

        if (!transform) return;

        Vector2F target = m_waypoints[m_currentWaypointIndex];
        Vector2F current = transform->GetPosition();

        Vector2F diff = target - current;
        float dist = diff.Length();

        if (dist < 10.0f) { 
            m_currentWaypointIndex = (m_currentWaypointIndex + 1) % m_waypoints.size();

            if (m_currentWaypointIndex == 0) { 
                JDGameSystem::ExpeditionSystem::Instance().ResolveExpedition();
                SafeDestroy(m_expeditionObject);
                UpdateAwayPointUI();

                if (JDGameSystem::ExpeditionSystem::Instance().ReachedTheGoal()) {
                    m_awayButton->SetTextureName("Art_Expedition_Button(Cannot)");
                    m_awayButton->SetSize({ 125.f, 35.0f });
                }

                std::cout << "[GameScene] 원정 끝." << std::endl;

                auto& expeditionSystem = JDGameSystem::ExpeditionSystem::Instance();
                int rate = expeditionSystem.GetExpeditionInfo(expeditionSystem.GetExpeditionGrade()).m_successRate;

                JDGameSystem::ExpeditionSystem::Instance().RollBonusType();
                int idx = JDGameSystem::ExpeditionSystem::Instance().GetRandomResourceIndex();
                std::wstring bonusName;
                switch (idx) {
                case 0: bonusName = L"식량"; break;
                case 1: bonusName = L"목재"; break;
                case 2: bonusName = L"광물"; break;
                default: bonusName = L"알 수 없음"; break;
                }

                m_awayAwardText02->SetText(std::to_wstring(rate) + L"% 확률로 " + bonusName + L" 추가 보상");

            }
        }
        else {
            Vector2F direction = diff.Normalized();
            current += direction * speed * deltaTime;
            transform->SetPosition(current);
        }
    }
    void GameScene::AttackWall(float deltaTime)
    {
        for (auto it = m_attackers.begin(); it != m_attackers.end();) {
            it->timer += deltaTime;
            if (it->enemy && it->timer >= m_battleTime) {
                auto* soldier = dynamic_cast<JDGameObject::Content::Soldier*>(it->enemy);
                if (soldier) {
                    int dmg = soldier->GetPower();
                    m_wallHealth = std::max(0, m_wallHealth - dmg);
                    UpdateBarrackUI();
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

    void GameScene::SafeDestroy(GameObjectBase* obj)
    {
        if (!obj) return;

        // 새로 추가: 컨테이너/팔로워 정리
        DetachByHost(obj);
        UnregisterUnit(obj);

        if (m_targetEnemy == obj) m_targetEnemy = nullptr;
        if (m_playerObject == obj) m_playerObject = nullptr;
        if (m_barracksObject == obj) m_barracksObject = nullptr;
        if (m_battleObject == obj) m_battleObject = nullptr;
        if (m_expeditionObject == obj) m_expeditionObject = nullptr;
        if (GetSelectedObject() == obj) SetSelectedObject(nullptr);

        m_attackers.erase(std::remove_if(m_attackers.begin(), m_attackers.end(),
            [&](auto& a) { return a.enemy == obj; }), m_attackers.end());

        SceneBase::DestroyObject(obj);
    }

    void GameScene::ClickUpdate()
    {
        auto camera = D2DRenderer::Instance().GetCamera();
        if (!camera) return;

        InputManager& input = InputManager::Instance();
        MouseState state = input.GetMouseState();
        bool left = InputManager::Instance().GetMouseState().leftClicked;
        bool right = InputManager::Instance().GetMouseState().rightClicked;
        auto mousePos = GetMouseWorldPos();

        if (ImGui::GetIO().WantCaptureMouse)
            return;

        if (state.leftClicked)
        {

            // (1) 마우스 좌표를 한 번만 계산해서 재사용합니다.
            // UI 클릭 판정에 사용할 스크린 좌표 (D2D 기준: Y 아래가 양수)
            Vector2F screenMousePos(static_cast<float>(state.pos.x), static_cast<float>(state.pos.y));
            // 게임 오브젝트 클릭 판정에 사용할 월드 좌표 (Unity 기준: Y 위가 양수)
            Vector2F worldMousePos = GetMouseWorldPos(); // 우리가 만든 통일된 함수 사용!

            bool clicked = false;

            ////////////////////////////////////////////////////////////////////////////////
            // 1. UI 클릭 검사 ( 스크린 좌표계 사용 )
            ////////////////////////////////////////////////////////////////////////////////

            for (int i = static_cast<int>(m_uiObjects.size()) - 1; i >= 0; --i)
            {
                auto& uiObj = m_uiObjects[i];
                if (!uiObj) continue;
                if (!uiObj->IsActive()) continue;

                auto clickable = uiObj->GetComponent<Editor_Clickable>();
                if (clickable && clickable->IsHit(screenMousePos))
                {
                    SetSelectedObject(uiObj.get());
                    clicked = true;
                    //std::cout << " UI 오브젝트 클릭";

                    //// 건물 건설 취소
                    //if (
                    //    isGridBuild == true && GetSelectedObject() != m_buildUI
                    //    && GetSelectedObject() != m_buildHouse && GetSelectedObject() != m_buildFishingspot
                    //    && GetSelectedObject() != m_buildRumbermill && GetSelectedObject() != m_buildMine
                    //    )
                    //{
                    //    isGridBuild = false;
                    //    std::cout << "[Error] 건물건설 취소";
                    //    CloseGridCreateMenu();
                    //    break;
                    //}

                    /*if (uiObj.get()->GetParent() == m_Menu) {
                        m_selectedTool = static_cast<Button*>(uiObj.get());
                        std::wcout << L"선택된 툴: " << m_selectedTool->GetName() << std::endl;
                    }*/
                    break;
                }
            }

            ////////////////////////////////////////////////////////////////////////////////
            // 2. 게임오브젝트 클릭 검사 ( 월드 좌표계 사용 )
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

                        // 그리드 클릭
                        for (auto& objPtr : m_gameObjects) {
                            auto* collider = objPtr->GetComponent<JDComponent::ColliderBase>();
                            if (!collider) continue;

                            auto* grid = dynamic_cast<Grid*>(objPtr.get());

                            if (!grid) continue; // 그리드가 아니면 스킵

                            int id = collider->GetIndex();

                            if (!collider->IsMouseOver(mousePos)) continue;

                            std::cout << "[DEBUG] right ID: " << id << std::endl;

                            m_selectedCollider = collider;

                            //auto* boxCol = static_cast<JDComponent::BoxCollider*>(collider);

                            if (grid->IsOccupied()) {
                                if (grid->HasBuilding()) {
                                    cout << "HasBuilding() : True" << endl;
                                    isGridSetting = true;
                                    if (m_isBarracksSelected) SetBarracksSelected(false); // 이미 다른걸 클릭했으니 병영이 켜져있다면 병영 끄기. 
                                    ShowGridSettingMenu();
                                }
                                else {
                                    cout << "HasBuilding() : False" << endl;
                                    isGridBuild = true;
                                    if (m_isBarracksSelected) SetBarracksSelected(false); // 이미 다른걸 클릭했으니 병영이 켜져있다면 병영 끄기. 
                                    ShowGridCreateMenu();
                                }
                            }
                            else {
                                if (grid->IsExpanded()) {
                                    cout << "확장 가능한 미점유 지역입니다." << endl;
                                    if (m_buildSystem->GetChoiceCount() > 0) {
                                        m_buildSystem->AddChoiceCount(-1);
                                        cout << "지역이 확장되었습니다." << endl;
                                        grid->SetExpanded(false);
                                        grid->SetOccupied(true);   
                                        for (int dir = 0; dir < static_cast<int>(Direction::DIRECTION_MAX); ++dir) {
                                            
                                            auto otherGrid = grid->GetOtherGrid(dir);
                                            if (!otherGrid || otherGrid->IsOccupied()) continue;
                                            else { otherGrid->SetExpanded(true); }
                                        }
                                    }
                                }
                                else {
                                    cout << "확장 불가능한 미점유 지역입니다." << endl;
                                }
                            }

                            break;
                        }

                        // 태그 기반 처리
                        auto* gameObj = GetSelectedObject();
                        if (!gameObj) return;

                        auto tag = gameObj->GetTag();
                        bool isEnemyTag = (tag == JDGlobal::Base::GameTag::Enemy);

                        if (tag == JDGlobal::Base::GameTag::Barracks) {
                            SetBarracksSelected(!m_isBarracksSelected);
                            //m_isBarracksSelected = !m_isBarracksSelected;
                            std::cout << "[GameScene] 병영 클릭. 선택 상태: " << m_isBarracksSelected << std::endl;

                            // UI 로직
                            /*if (m_isBarracksSelected)
                            {
                                isAway = true;
                                ShowAwayMenu();
                            }
                            else
                            {
                                isAway = false;
                                CloseAwayMenu();
                            }*/
                        }
                        else if (m_isBarracksSelected && !m_battleObject && (isEnemyTag && !m_targetEnemy) && m_playerArmy.GetTotalUnits() > 0) {
                            m_targetEnemy = gameObj;
                            //Vector2F pos = m_barracksObject->GetComponent<Transform>()->GetPosition();
                            SpawnPlayerArmy(m_wallPos);
                            //m_isBarracksSelected = false;
                            std::cout << "[GameScene] 병영 클릭 -> 적 클릭." << std::endl;
                        }
                        else if (m_isBarracksSelected && tag == JDGlobal::Base::GameTag::BattleAnim && m_playerArmy.GetTotalUnits() > 0) {
                            //Vector2F pos = m_barracksObject->GetComponent<Transform>()->GetPosition();
                            SpawnPlayerArmy(m_wallPos);
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

            if (!clicked)
            {
                SetSelectedObject(nullptr);

                // 창 닫기.
                if (isGridBuild)
                {
                    CloseGridCreateMenu();
                    CloseGridSettingMenu();
                    CloseAwayMenu();
                }

                else if (isGridSetting)
                {
                    CloseGridCreateMenu();
                    CloseGridSettingMenu();
                    CloseAwayMenu();
                }

                else if (isAway)
                {
                    CloseGridCreateMenu();
                    CloseGridSettingMenu();
                    CloseAwayMenu();
                }

                if (m_isBarracksSelected) SetBarracksSelected(false);
            }
        }
        /*if (state.rightClicked)
        {
            for (auto& objPtr : m_gameObjects) {

                auto* collider = objPtr->GetComponent<JDComponent::ColliderBase>();
                if (!collider) continue;

                auto* grid = dynamic_cast<Grid*>(objPtr.get());
                if (!grid)
                    continue;

                int id = collider->GetIndex();

                if (!collider->IsMouseOver(mousePos))
                    continue;

                if (collider->IsOpen())
                {
                    std::cout << "[DEBUG] right ID: " << id << std::endl;

                    m_selectedCollider = collider;

                    auto* boxCol = static_cast<JDComponent::BoxCollider*>(collider);
                    if (boxCol->HasBuilding()) {
                        ShowGridSettingMenu();
                    }
                    else {
                        isbuild = true;
                        ShowGridCreateMenu();
                    }
                }
            }
        }*/
    }

    JDGameObject::GameObjectBase* GameScene::CreateSoldierUnit(const JDGameSystem::UnitCounts& units, JDGlobal::Base::GameTag tag, JDGlobal::Contents::State state, const Vector2F& pos, const std::string& textureName)
    {
        auto* obj = CreateGameObject<Soldier>((tag == JDGlobal::Base::GameTag::Player) ? L"playerObj" : L"enemyObj");
        obj->SetUnitCounts(units);

        JDGameSystem::ArmySystem army; army.OverrideUnitCounts(units);
        obj->SetPower(army.CalculateTotalPower());

        obj->SetTag(tag);
        obj->SetState(state);
        obj->AddComponent<JDComponent::TextureRenderer>(textureName, RenderLayerInfo{ SortingLayer::Cat, 1 });

        auto bitmap = static_cast<ID2D1Bitmap*>(AssetManager::Instance().GetTexture(textureName));
        Vector2F size = { bitmap->GetSize().width / 2.0f, bitmap->GetSize().height / 2.0f };

        obj->AddComponent<JDComponent::BoxCollider>(size);
        obj->GetComponent<JDComponent::BoxCollider>()->SetOpen(true);
        obj->GetComponent<Transform>()->SetPosition(pos);
        obj->AddComponent<JDComponent::SFX>("Step");

        if (tag == JDGlobal::Base::GameTag::Enemy) {
            obj->AddComponent<Editor_Clickable>();
            if (auto* tr = obj->GetComponent<TextureRenderer>()) tr->SetFlipX(true);
        }

        // 진영 컨테이너 등록
        RegisterUnit(tag, obj);

        // 전투력 표시.
        auto* headTextGO = CreateHeadMarker(obj);
        if (auto* tr = headTextGO->GetComponent<JDComponent::TextRenderer>()) {
            tr->SetText(std::to_wstring(obj->GetPower()));
        }

        return obj;
    }

    JDGameObject::GameObjectBase* GameScene::CreateStructure(const std::wstring& name, JDGlobal::Base::GameTag tag, const Vector2F& pos, const std::string& textureName)
    {
        auto* obj = CreateGameObject<GameObject>(name);
        obj->SetTag(tag);
        obj->AddComponent<Editor_Clickable>();
        obj->AddComponent<JDComponent::TextureRenderer>(textureName, RenderLayerInfo{ SortingLayer::BackGround, 1 });
        obj->GetComponent<JDComponent::TextureRenderer>()->SetSize({177, 157}); // 어차피 병영만 이 함수 쓰니까. 걍 써..

        //auto bitmap = static_cast<ID2D1Bitmap*>(AssetManager::Instance().GetTexture(textureName));
        //Vector2F size = { bitmap->GetSize().width / 2.0f, bitmap->GetSize().height / 2.0f };
        //Vector2F size = { 175 / 2.0f, 280 / 2.0f };
        Vector2F size = { 177 / 2.0f, 157 / 2.0f };

        obj->AddComponent<JDComponent::BoxCollider>(size);
        obj->GetComponent<JDComponent::BoxCollider>()->SetOpen(true);
        obj->GetComponent<Transform>()->SetPosition(pos);

        return obj;
    }

    /*JDGameObject::GameObjectBase* GameScene::CreateUIButton(const std::wstring& name, const Vector2F& pos, const std::string& textureName, const std::string& clickEventName, std::function<void()> callback)
    {
        auto* btn = CreateUIObject<Button>(name);
        btn->SetTextureName(textureName);
        btn->SetSizeToOriginal();
        btn->SetPosition(pos);
        btn->AddOnClick(clickEventName, callback);

        return btn;
    }*/

    void GameScene::CreateExpedition()
    {
        auto* obj = CreateGameObject<Soldier>(L"playerObj");

        obj->SetTag(JDGlobal::Base::GameTag::Player);
        obj->SetState(JDGlobal::Contents::State::Idle);
        obj->AddComponent<JDComponent::TextureRenderer>("ART_NaviAdv_Sprite01", RenderLayerInfo{ SortingLayer::Cat, 1 });

        obj->GetComponent<Transform>()->SetPosition(Vector2F{-158.0f, 30.0f});
        obj->AddComponent<JDComponent::SFX>("Step");

        m_expeditionObject = obj;

        std::cout << "[GameScene] 원정대 생성." << std::endl;
    }

    void GameScene::UpdateResourceUI()
    {
        auto curPop = ResourceSystem::Instance().GetCurPopulation();
        if (curPop >= 0) m_curPopText->SetText(std::to_wstring(curPop));
        else m_curPopText->SetText(std::to_wstring(curPop));

        auto maxPop = ResourceSystem::Instance().GetMaxPopulation();
        m_maxPopText->SetText(std::to_wstring(maxPop));

        auto totFood = ResourceSystem::Instance().GetTotalResource().m_food;
        m_curFoodText->SetText(std::to_wstring(totFood));

        auto resFood = ResourceSystem::Instance().GetTotalResourcePerSec().m_food;
        if (resFood >= 0) m_resFoodText->SetText(L"+" + std::to_wstring(resFood));  // curPop 대신 resFood 기준으로 체크하는 게 적절
        else m_resFoodText->SetText(std::to_wstring(resFood));

        auto totWood = ResourceSystem::Instance().GetTotalResource().m_wood;
        m_curWoodText->SetText(std::to_wstring(totWood));

        auto resWood = ResourceSystem::Instance().GetTotalResourcePerSec().m_wood;
        if (resWood >= 0) m_resWoodText->SetText(L"+" + std::to_wstring(resWood));
        else m_resWoodText->SetText(std::to_wstring(resWood));

        auto totMineral = ResourceSystem::Instance().GetTotalResource().m_mineral;
        m_curMineralText->SetText(std::to_wstring(totMineral));  // totMineral 출력으로 수정

        auto resMineral = ResourceSystem::Instance().GetTotalResourcePerSec().m_mineral;
        if (resMineral >= 0) m_resMineralText->SetText(L"+" + std::to_wstring(resMineral));
        else m_resMineralText->SetText(std::to_wstring(resMineral));

        if (m_selectedCollider) {
            int level = 0;
            BuildingType buldingType = BuildingType::FishingSpot;

            auto* boxCol = static_cast<JDComponent::BoxCollider*>(m_selectedCollider);
            Grid* grid = dynamic_cast<Grid*>(boxCol->GetOwner());
            if (grid) {
                if (!grid->IsOccupied()) {
                    cout << "[Error] 점유되지 않은 구역입니다. 건물 설치 취소!!" << endl;
                    CloseGridSettingMenu();
                    return;
                }
                if (grid->HasBuilding()) { // 그리드가 빌딩이 있고, 점유중이면
                    if (FishingSpot* building = dynamic_cast<FishingSpot*>(grid->GetBuilding())) {
                        level = building->GetLevel();
                        
                        cout << to_string(m_dataTableManager->GetFishingSpotTableInfo().m_resourceGenPerSec[level].m_food) << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
                        buldingType = BuildingType::FishingSpot;
                    }
                    else if (LumberMill* building = dynamic_cast<LumberMill*>(grid->GetBuilding())) {
                        level = building->GetLevel();
                        buldingType = BuildingType::LumberMill;
                    }
                    else if (Mine* building = dynamic_cast<Mine*>(grid->GetBuilding())) {
                        level = building->GetLevel();
  
                        buldingType = BuildingType::Mine;
                    }
                    else if (House* building = dynamic_cast<House*>(grid->GetBuilding())) {
                        level = building->GetLevel();
                        cout << to_string(m_dataTableManager->GetHouseTableInfo().m_initPopulation[level]) << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
                        buldingType = BuildingType::House;
                    }
                }

                switch (buldingType) {
                case BuildingType::House: 
                    m_upgradeEffctImage->SetTextureName("ART_CostPop01");
                    m_upgradeEffectText->SetText(to_wstring(m_dataTableManager->GetHouseTableInfo().m_initPopulation[level]) + L"/초");
                    break;
                case BuildingType::FishingSpot: 
                    m_upgradeEffctImage->SetTextureName("ART_CostFood01");
                    m_upgradeEffectText->SetText(to_wstring(m_dataTableManager->GetFishingSpotTableInfo().m_resourceGenPerSec[level].m_food) + L"/초");
                    break;
                case BuildingType::LumberMill: 
                    m_upgradeEffctImage->SetTextureName("ART_CostWood01");
                    m_upgradeEffectText->SetText(to_wstring(m_dataTableManager->GetLumbermillTableInfo().m_resourceGenPerSec[level].m_wood) + L"/초");
                    break;
                case BuildingType::Mine: 
                    m_upgradeEffctImage->SetTextureName("ART_CostMineral01");
                    m_upgradeEffectText->SetText(to_wstring(m_dataTableManager->GetMineTableInfo().m_resourceGenPerSec[level].m_mineral) + L"/초");
                    break;
                }

            }
        }
    }

    void GameScene::InitGridCreateMenu()
    {
        // 기본 UI 보이기
        m_defaultUI->SetActive(true);
        m_buildUI->SetActive(false);
        m_upgradeUI->SetActive(false);
        m_awayUI->SetActive(false);

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

    void GameScene::ShowGridCreateMenu()
    {
        cout << "m_gridCreateButtons 켜짐" << endl;
        // 건설 UI 보이기
        CloseGridSettingMenu();
        CloseAwayMenu();

        m_defaultUI->SetActive(false);
        m_buildUI->SetActive(true);
        m_upgradeUI->SetActive(false);
        m_awayUI->SetActive(false);

        // 건설 UI ( 주거지, 낚시터, 제재소, 광산, (연구실) )
        m_buildHouse->SetActive(true);
        m_buildFishingspot->SetActive(true);
        m_buildRumbermill->SetActive(true);
        m_buildMine->SetActive(true);

        // 건설 Info
        CloseGridCreateInfo();
    }

    void GameScene::CloseGridCreateMenu()
    {
        cout << "m_gridCreateButtons 꺼짐" << endl;
        // 건설 UI 끄기
        m_defaultUI->SetActive(true);
        m_buildUI->SetActive(false);
        m_upgradeUI->SetActive(false);
        m_awayUI->SetActive(false);

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

    void GameScene::InitGridSettingMenu()
    {
        // 업그레이드 UI 안보이게
        m_upgradeUI->SetActive(false);

        // 고양이 배치 UI
        m_catTypeText->SetActive(false);

        m_catCostInfoText->SetActive(false);
        m_catCostText->SetActive(false);
        m_catCostImage->SetActive(false);

        m_catEffectInfoText->SetActive(false);
        m_catEffectText->SetActive(false);
        m_catEffctImage->SetActive(false);

        m_naviSetButton->SetActive(false);
        m_felisSetButton->SetActive(false);
        m_koneSetButton->SetActive(false);

        // 건설 업그레이드 UI
        m_builtTypeText->SetActive(false);

        m_upgradeCostInfoText->SetActive(false);
        m_upgradeCostText->SetActive(false);
        m_upgradeCostImage->SetActive(false);

        m_upgradeEffectInfoText->SetActive(false);
        m_upgradeEffectText->SetActive(false);
        m_upgradeEffctImage->SetActive(false);

        m_downgradeButton->SetActive(false);
        m_upgradeButton->SetActive(false);
    }

    //void GameScene::ShowUpgradeMenu()
    //{
    //    // 업그레이드 UI 보이기
    //    CloseBuildMenu();
    //    CloseAWayMenu();

    //    m_defaultUI->SetActive(false);
    //    m_buildUI->SetActive(false);
    //    m_upgradeUI->SetActive(true);
    //    m_awayUI->SetActive(false);

    //    // 고양이 배치 UI
    //    m_catTypeText->SetActive(true);

    //    //m_catCostInfoText->SetActive(true);
    //    //m_catCostText->SetActive(true);
    //    //m_catCostImage->SetActive(true);

    //    //m_catEffectInfoText->SetActive(true);
    //    //m_catEffectText->SetActive(true);
    //    //m_catEffctImage->SetActive(true);

    //    m_naviSetButton->SetActive(true);
    //    m_felisSetButton->SetActive(true);
    //    m_koneSetButton->SetActive(true);

    //    // 건설 업그레이드 UI
    //    m_builtTypeText->SetActive(true);
    //    
    //    m_upgradeCostInfoText->SetActive(true);
    //    m_upgradeCostText->SetActive(true);
    //    m_upgradeCostImage->SetActive(true);

    //    m_upgradeEffectInfoText->SetActive(true);
    //    m_upgradeEffectText->SetActive(true);
    //    m_upgradeEffctImage->SetActive(true);

    //    m_downgradeButton->SetActive(true);
    //    m_upgradeButton->SetActive(true);
    //}

    void GameScene::CloseGridSettingMenu()
    {
        // 업그레이드 UI 끄기
        m_defaultUI->SetActive(true);
        m_buildUI->SetActive(false);
        m_upgradeUI->SetActive(false);
        m_awayUI->SetActive(false);

        // 고양이 배치 UI
        m_catTypeText->SetActive(false);

        m_catCostInfoText->SetActive(false);
        m_catCostText->SetActive(false);
        m_catCostImage->SetActive(false);

        m_catEffectInfoText->SetActive(false);
        m_catEffectText->SetActive(false);
        m_catEffctImage->SetActive(false);

        m_naviSetButton->SetActive(false);
        m_felisSetButton->SetActive(false);
        m_koneSetButton->SetActive(false);

        // 건설 업그레이드 UI
        m_builtTypeText->SetActive(false);

        m_upgradeCostInfoText->SetActive(false);
        m_upgradeCostText->SetActive(false);
        m_upgradeCostImage->SetActive(false);

        m_upgradeEffectInfoText->SetActive(false);
        m_upgradeEffectText->SetActive(false);
        m_upgradeEffctImage->SetActive(false);

        m_downgradeButton->SetActive(false);
        m_upgradeButton->SetActive(false);
    }

    //void GameScene::ShowBuildInfo(JDGlobal::Contents::BuildingType buildType, std::string costText, std::string effectText)
    void GameScene::ShowGridCreateInfo(JDGlobal::Contents::BuildingType buildType, std::string costText, std::string effectText)
    {
        ChangeGridCreateInfo(buildType, costText, effectText);

        m_costInfoText->SetActive(true);
        m_costText->SetActive(true);
        m_costImage->SetActive(true);

        m_effectInfoText->SetActive(true);
        m_effectText->SetActive(true);
        m_effctImage->SetActive(true);
    }

    void GameScene::CloseGridCreateInfo()
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

    void GameScene::ShowCatInfo(JDGlobal::Contents::CatType catType, std::string costText, std::string effectText)
    {
        ChangeUpgradeInfo(catType, costText, effectText);

        m_catCostInfoText->SetActive(true);
        m_catCostText->SetActive(true);
        m_catCostImage->SetActive(true);

        m_catEffectInfoText->SetActive(true);
        m_catEffectText->SetActive(true);
        m_catEffctImage->SetActive(true);
    }

    void GameScene::CloseCatInfo()
    {
        m_catTypeText->SetActive(true);
        m_catTypeText->SetText(L"배치할 냥이");

        m_catCostInfoText->SetActive(false);
        m_catCostText->SetActive(false);
        m_catCostImage->SetActive(false);

        m_catEffectInfoText->SetActive(false);
        m_catEffectText->SetActive(false);
        m_catEffctImage->SetActive(false);
    }

    void GameScene::ChangeUpgradeInfo(JDGlobal::Contents::CatType catType, std::string costText, std::string effectText)
    {
        switch (catType)
        {
        case JDGlobal::Contents::CatType::Kone:
            m_catTypeText->SetText(L"코네냥");
            break;
        case JDGlobal::Contents::CatType::Navi:
            m_catTypeText->SetText(L"나비냥");
            break;
        case JDGlobal::Contents::CatType::Felis:
            m_catTypeText->SetText(L"펠리스냥");
            break;
        default:
            break;
        }
    }

    void GameScene::InitAwayMenu()
    {
        // 병영 UI 안보이게
        m_awayUI->SetActive(false);

        /////
        // 견습 냥이
        m_trainerCatButton->SetActive(false);
        m_trainerCatName->SetActive(false);

        m_trainerCatCostInfo->SetActive(false);
        m_trainerCatCostImage01->SetActive(false);
        m_trainerCatCostText01->SetActive(false);
        m_trainerCatCostImage02->SetActive(false);
        m_trainerCatCostText02->SetActive(false);

        m_trainerCatRecruitInfo->SetActive(false);
        m_trainerCatRecruitText->SetActive(false);

        m_trainerCatPowerInfo->SetActive(false);
        m_trainerCatPowerText->SetActive(false);

        /////
        // 숙련 냥이
        m_expertCatButton->SetActive(false);
        m_expertCatName->SetActive(false);

        m_expertCatCostInfo->SetActive(false);
        m_expertCatCostImage01->SetActive(false);
        m_expertCatCostText01->SetActive(false);
        m_expertCatCostImage02->SetActive(false);
        m_expertCatCostText02->SetActive(false);

        m_expertCatRecruitInfo->SetActive(false);
        m_expertCatRecruitText->SetActive(false);

        m_expertCatPowerInfo->SetActive(false);
        m_expertCatPowerText->SetActive(false);

        /////
        // 원정 보내기
        m_awayInfo->SetActive(false);

        m_awayBeginner->SetActive(false);           // 초급
        m_awayIntermediate->SetActive(false);       // 중급
        m_awayAdvanced->SetActive(false);           // 상급

        /////
        // 원정 팝업
        m_awayPopupUI->SetActive(false);
        m_awayPopupInfo->SetActive(false);

        m_awayCostInfo->SetActive(false);
        m_awayCostImage01->SetActive(false);
        m_awayCostText01->SetActive(false);
        m_awayCostImage02->SetActive(false);
        m_awayCostText02->SetActive(false);
        m_awayCostImage03->SetActive(false);
        m_awayCostText03->SetActive(false);

        m_awayAwardInfo->SetActive(false);
        m_awayAwardText01->SetActive(false);
        m_awayAwardText02->SetActive(false);

        m_awayButton->SetActive(false);

        m_awayStar01->SetActive(false);
        m_awayStar02->SetActive(false);
        m_awayStar03->SetActive(false);
    }

    void GameScene::ShowAwayMenu()
    {
        // 병영 UI 보이기
        CloseGridCreateMenu();
        CloseGridSettingMenu();

        m_defaultUI->SetActive(false);
        m_buildUI->SetActive(false);
        m_upgradeUI->SetActive(false);
        m_awayUI->SetActive(true);

        /////
        // 견습 냥이
        m_trainerCatButton->SetActive(true);
        m_trainerCatName->SetActive(true);

        m_trainerCatCostInfo->SetActive(true);
        m_trainerCatCostImage01->SetActive(true);
        m_trainerCatCostText01->SetActive(true);
        m_trainerCatCostImage02->SetActive(true);
        m_trainerCatCostText02->SetActive(true);

        m_trainerCatRecruitInfo->SetActive(true);
        m_trainerCatRecruitText->SetActive(true);

        m_trainerCatPowerInfo->SetActive(true);
        m_trainerCatPowerText->SetActive(true);

        /////
        // 숙련 냥이
        m_expertCatButton->SetActive(true);
        m_expertCatName->SetActive(true);

        m_expertCatCostInfo->SetActive(true);
        m_expertCatCostImage01->SetActive(true);
        m_expertCatCostText01->SetActive(true);
        m_expertCatCostImage02->SetActive(true);
        m_expertCatCostText02->SetActive(true);

        m_expertCatRecruitInfo->SetActive(true);
        m_expertCatRecruitText->SetActive(true);

        m_expertCatPowerInfo->SetActive(true);
        m_expertCatPowerText->SetActive(true);

        /////
        // 원정 보내기
        m_awayInfo->SetActive(true);

        m_awayBeginner->SetActive(true);           // 초급
        m_awayIntermediate->SetActive(true);       // 중급
        m_awayAdvanced->SetActive(true);           // 상급

        /////
        // 원정 팝업
        // Comment : 원정 팝업은 버튼을 눌러야 나와야 함.
        /*
        m_awayPopupUI->SetActive(false);
        m_awayPopupInfo->SetActive(false);

        m_awayCostInfo->SetActive(false);
        m_awayCostImage01->SetActive(false);
        m_awayCostText01->SetActive(false);
        m_awayCostImage02->SetActive(false);
        m_awayCostText02->SetActive(false);
        m_awayCostImage03->SetActive(false);
        m_awayCostText03->SetActive(false);

        m_awayAwardInfo->SetActive(false);
        m_awayAwardText01->SetActive(false);
        m_awayAwardText02->SetActive(false);

        m_awayButton->SetActive(false);
        */
    }

    void GameScene::CloseAwayMenu()
    {
        // 팝업 끄기
        CloseAwayPopup();

        // 병영 UI 끄기
        m_defaultUI->SetActive(true);
        m_buildUI->SetActive(false);
        m_upgradeUI->SetActive(false);
        m_awayUI->SetActive(false);

        /////
        // 견습 냥이
        m_trainerCatButton->SetActive(false);
        m_trainerCatName->SetActive(false);

        m_trainerCatCostInfo->SetActive(false);
        m_trainerCatCostImage01->SetActive(false);
        m_trainerCatCostText01->SetActive(false);
        m_trainerCatCostImage02->SetActive(false);
        m_trainerCatCostText02->SetActive(false);

        m_trainerCatRecruitInfo->SetActive(false);
        m_trainerCatRecruitText->SetActive(false);

        m_trainerCatPowerInfo->SetActive(false);
        m_trainerCatPowerText->SetActive(false);

        /////
        // 숙련 냥이
        m_expertCatButton->SetActive(false);
        m_expertCatName->SetActive(false);

        m_expertCatCostInfo->SetActive(false);
        m_expertCatCostImage01->SetActive(false);
        m_expertCatCostText01->SetActive(false);
        m_expertCatCostImage02->SetActive(false);
        m_expertCatCostText02->SetActive(false);

        m_expertCatRecruitInfo->SetActive(false);
        m_expertCatRecruitText->SetActive(false);

        m_expertCatPowerInfo->SetActive(false);
        m_expertCatPowerText->SetActive(false);

        /////
        // 원정 보내기
        m_awayInfo->SetActive(false);

        m_awayBeginner->SetActive(false);           // 초급
        m_awayIntermediate->SetActive(false);       // 중급
        m_awayAdvanced->SetActive(false);           // 상급

        /////
        // 원정 팝업
        m_awayPopupUI->SetActive(false);
        m_awayPopupInfo->SetActive(false);

        m_awayCostInfo->SetActive(false);
        m_awayCostImage01->SetActive(false);
        m_awayCostText01->SetActive(false);
        m_awayCostImage02->SetActive(false);
        m_awayCostText02->SetActive(false);
        m_awayCostImage03->SetActive(false);
        m_awayCostText03->SetActive(false);

        m_awayAwardInfo->SetActive(false);
        m_awayAwardText01->SetActive(false);
        m_awayAwardText02->SetActive(false);

        m_awayButton->SetActive(false);

        m_awayStar01->SetActive(false);
        m_awayStar02->SetActive(false);
        m_awayStar03->SetActive(false);
    }

    void GameScene::ShowAwayPopup()
    {
        /////
        // 원정 팝업
        m_awayPopupUI->SetActive(true);
        m_awayPopupInfo->SetActive(true);

        m_awayCostInfo->SetActive(true);
        m_awayCostImage01->SetActive(true);
        m_awayCostText01->SetActive(true);
        m_awayCostImage02->SetActive(true);
        m_awayCostText02->SetActive(true);
        m_awayCostImage03->SetActive(true);
        m_awayCostText03->SetActive(true);

        m_awayAwardInfo->SetActive(true);
        m_awayAwardText01->SetActive(true);
        m_awayAwardText02->SetActive(true);

        m_awayButton->SetActive(true);

        m_awayStar01->SetActive(true);
        m_awayStar02->SetActive(true);
        m_awayStar03->SetActive(true);
    }

    void GameScene::CloseAwayPopup()
    {
        /////
        // 원정 팝업
        m_awayPopupUI->SetActive(false);
        m_awayPopupInfo->SetActive(false);

        m_awayCostInfo->SetActive(false);
        m_awayCostImage01->SetActive(false);
        m_awayCostText01->SetActive(false);
        m_awayCostImage02->SetActive(false);
        m_awayCostText02->SetActive(false);
        m_awayCostImage03->SetActive(false);
        m_awayCostText03->SetActive(false);

        m_awayAwardInfo->SetActive(false);
        m_awayAwardText01->SetActive(false);
        m_awayAwardText02->SetActive(false);

        m_awayButton->SetActive(false);

        m_awayStar01->SetActive(false);
        m_awayStar02->SetActive(false);
        m_awayStar03->SetActive(false);
    }

    // void GameScene::ChangeBuildInfo(JDGlobal::Contents::BuildingType buildType, std::string costText, std::string effectText)
    void GameScene::ChangeGridCreateInfo(JDGlobal::Contents::BuildingType buildType, std::string costText, std::string effectText)
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

    void GameScene::ShowGridSettingMenu() {
        // 셋팅 UI 보이기
        CloseGridCreateMenu();
        CloseAwayMenu();

        m_defaultUI->SetActive(false);
        m_buildUI->SetActive(false);
        m_upgradeUI->SetActive(true);
        m_awayUI->SetActive(false);

        // 고양이 배치 UI
        m_catTypeText->SetActive(true);

        //m_catCostInfoText->SetActive(true);
        //m_catCostText->SetActive(true);
        //m_catCostImage->SetActive(true);

        //m_catEffectInfoText->SetActive(true);
        //m_catEffectText->SetActive(true);
        //m_catEffctImage->SetActive(true);

        m_naviSetButton->SetActive(true);
        m_felisSetButton->SetActive(true);
        m_koneSetButton->SetActive(true);

        // 건설 업그레이드 UI
        m_builtTypeText->SetActive(true);

        m_upgradeCostInfoText->SetActive(true);
        m_upgradeCostText->SetActive(true);
        m_upgradeCostImage->SetActive(true);

        m_upgradeEffectInfoText->SetActive(true);
        m_upgradeEffectText->SetActive(true);
        m_upgradeEffctImage->SetActive(true);

        m_downgradeButton->SetActive(true);
        m_upgradeButton->SetActive(true);
    }

    void GameScene::CreateGameMap()
    {
        using namespace JDGameObject;
        using namespace JDComponent;
        using JDScene::GameScene;

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
        auto map_battleLayer01Texture = map_battleLayer01->AddComponent<TextureRenderer>("ART_BattleMapLayer01", RenderLayerInfo{ SortingLayer::Front, 1 });
        map_battleLayer01Texture->SetSize({ JDGlobal::Window::WindowSize::Instance().GetSize() });

        // 성벽
        auto* map_castle = CreateGameObject<GameObject>(L"Map_Castle");
        map_castle->AddComponent<Transform>();
        auto map_castleTexture = map_castle->AddComponent<TextureRenderer>("ART_Rampart01", RenderLayerInfo{ SortingLayer::BackGround, 1 });
        map_castleTexture->SetSize({ 715, 960 });
        auto map_castleTR = map_castle->GetComponent<Transform>();
        map_castleTR->SetPosition(Vector2F{ -560.f, -13.f });

        // 배럭 01
        auto* map_barrack01 = CreateGameObject<GameObject>(L"Map_Barrack01");
        map_barrack01->AddComponent<Transform>();
        auto map_barrack01Texture = map_barrack01->AddComponent<TextureRenderer>("ART_Barracks01", RenderLayerInfo{ SortingLayer::BackGround, 1 });
        map_barrack01Texture->SetSize({ 87.5f, 80.f });
        auto map_barrack01TR = map_barrack01->GetComponent<Transform>();
        map_barrack01TR->SetPosition(Vector2F{ -135.f, 190.f });

        // 배럭 02
        //auto* map_barrack02 = CreateGameObject<GameObject>(L"Map_Barrack02");
        //map_barrack02->AddComponent<Transform>();
        //auto map_barrack02Texture = map_barrack02->AddComponent<TextureRenderer>("ART_Barracks02", RenderLayerInfo{ SortingLayer::BackGround, 1 });
        //map_barrack02Texture->SetSize({ 175, 280 });
        //auto map_barrack02TR = map_barrack02->GetComponent<Transform>();
        //map_barrack02TR->SetPosition(Vector2F{ -196.5f, 80.0f });

        // 그리드
        // =====================================================================
        // 그리드 배치 정보
        // =====================================================================

        m_buildSystem->CreateGrid(this);
        /*
        // 그리드를 배치할 전체 영역의 크기
        const float areaWidth = 515.0f;
        const float areaHeight = 760.0f;

        // 그리드를 배치할 영역의 좌측 상단 월드 좌표
        const Vector2F areaTopLeft = { -850.0f, 350.0f };

        // 그리드의 행과 열 개수
        m_totalCols = 4;
        m_totalRows = 6;

        // 셀과 셀 사이의 간격 (여백)
        const float padding = 10.0f;

        // =====================================================================
        // 그리드 생성 로직
        // =====================================================================

        // 1. 실제 셀들이 배치될 수 있는 공간 계산 (전체 영역 - 총 간격)
        float contentWidth = areaWidth - (padding * (m_totalCols - 1));
        float contentHeight = areaHeight - (padding * (m_totalRows - 1));

        // 2. 정사각형 셀의 한 변의 최대 크기 계산
        float cellWidthPossible = contentWidth / m_totalCols;
        float cellHeightPossible = contentHeight / m_totalRows;

        // 3. 정사각형 유지를 위해 가능한 크기 중 더 작은 쪽을 최종 셀 크기로 선택
        const float squareCellSize = std::min(cellWidthPossible, cellHeightPossible);

        // 4. 최종 그리드의 전체 크기 계산
        float finalGridWidth = (squareCellSize * m_totalCols) + (padding * (m_totalCols - 1));
        float finalGridHeight = (squareCellSize * m_totalRows) + (padding * (m_totalRows - 1));

        // 5. 전체 그리드를 영역 내에서 중앙에 배치하기 위한 시작 오프셋 계산
        float offsetX = (areaWidth - finalGridWidth) / 2.0f;
        float offsetY = (areaHeight - finalGridHeight) / 2.0f;


        for (int col = 0; col < m_totalCols; ++col) {
            for (int row = 0; row < m_totalRows; ++row) {

                std::wstring name = L"Box_" + std::to_wstring(col) + L"_" + std::to_wstring(row);
                auto* box = CreateGameObject<Grid>(name.c_str());

                // 6. 각 셀의 최종 위치 계산
                //    시작점 + 중앙 오프셋 + (셀 크기 + 간격) * 인덱스 + 셀 중앙 보정
                float x = areaTopLeft.x + offsetX + (col * (squareCellSize + padding)) + (squareCellSize / 2.0f);
                float y = areaTopLeft.y - offsetY - (row * (squareCellSize + padding)) - (squareCellSize / 2.0f);

                box->GetComponent<Transform>()->SetPosition({ x, y });
                box->AddComponent<Editor_Clickable>();

                // TextureRenderer를 먼저 추가합니다. (이때 텍스처 이름은 비워둡니다)
                auto* textureRenderer = box->AddComponent<TextureRenderer>("");

                // ==========================================================
                // 박스 텍스처 이름 동적 생성 및 설정
                // ==========================================================
                int idx = col * m_totalRows + row; // 0 ~ 23
                int textureNumber = idx + 1;       // 1 ~ 24

                // stringstream을 사용해 "ART_Tile" + "0" + "숫자" 형태로 조합
                std::stringstream ss;
                ss << "ART_Tile" << std::setw(2) << std::setfill('0') << textureNumber;

                // 완성된 텍스처 키("ART_Tile01", "ART_Tile02" 등)를 설정
                textureRenderer->SetTextureName(ss.str());
                // ==========================================================

                // 크기 설정 및 콜라이더 추가
                textureRenderer->SetSize({ squareCellSize, squareCellSize });

                auto* collider = box->AddComponent<BoxCollider>(Vector2F{ squareCellSize / 2.0f, squareCellSize / 2.0f });
                collider->SetIndex(idx);
            }
        }
        */
        // =====================================================================
        // UI 오브젝트 생성 (Screen Space)
        // =====================================================================

        auto cam = D2DRenderer::Instance().GetCamera();

        // 1) [상단] 자원
        ////////////////////////////////////////////////////////////////////////////////

        // 인구
        m_buttonPop = CreateUIObject<Button>(L"UI_Pop");
        m_buttonPop->SetText(L"");
        m_buttonPop->SetTextureName("ART_RESPop01");
        m_buttonPop->SetSize({ 150.0f, 66.f });
        m_buttonPop->SetPosition({ -858.f, 493.f });
        m_buttonPop->SetAnchor({ 0.0f, 1.0f });

        // 인구 텍스트
        m_curPopText = CreateUIObject<Text>(L"UI_CurPopText");
        m_curPopText->SetText(std::to_wstring(ResourceSystem::Instance().GetCurPopulation()));
        m_curPopText->SetTextFormatName("Sebang_22");
        m_curPopText->SetColor(D2D1::ColorF(0x69512C));
        m_curPopText->SetSize({ 300, 100 });
        m_curPopText->SetPosition({ -832, 489 });

        // 인구 최대치 텍스트
        m_maxPopText = CreateUIObject<Text>(L"UI_MaxPopText");
        m_maxPopText->SetText(std::to_wstring(ResourceSystem::Instance().GetMaxPopulation()));
        m_maxPopText->SetTextFormatName("Sebang_16");
        m_maxPopText->SetColor(D2D1::ColorF(0.839f, 0.741f, 0.580f));
        m_maxPopText->SetSize({ 300, 100 });
        m_maxPopText->SetPosition({ -775, 510 });

        ////////////////////////////////////////////////////////////////////////////////

        // 식량
        m_buttonFood = CreateUIObject<Button>(L"UI_Food");
        m_buttonFood->SetText(L"");
        m_buttonFood->SetTextureName("ART_RESFood01");
        m_buttonFood->SetSize({ 165.0f, 65.f });
        m_buttonFood->SetPosition({ -660.f, 492.f });
        m_buttonFood->SetAnchor({ 0.0f, 1.0f });

        // 식량 텍스트
        m_curFoodText = CreateUIObject<Text>(L"UI_CurFoodText");
        m_curFoodText->SetText(to_wstring(ResourceSystem::Instance().GetTotalResource().m_food));
        m_curFoodText->SetTextFormatName("Sebang_22");
        m_curFoodText->SetColor(D2D1::ColorF(0x69512C));
        m_curFoodText->SetSize({ 300, 100 });
        m_curFoodText->SetPosition({ -628, 489 });

        // 식량 초당 재생 총량 텍스트
        m_resFoodText = CreateUIObject<Text>(L"UI_ResFoodText");
        m_resFoodText->SetText(to_wstring(ResourceSystem::Instance().GetTotalResourcePerSec().m_food));
        m_resFoodText->SetTextFormatName("Sebang_16");
        m_resFoodText->SetColor(D2D1::ColorF(0.839f, 0.741f, 0.580f));
        m_resFoodText->SetSize({ 300, 100 });
        m_resFoodText->SetPosition({ -568, 510 });

        ////////////////////////////////////////////////////////////////////////////////

        // 목재
        m_buttonWood = CreateUIObject<Button>(L"UI_Wood");
        m_buttonWood->SetText(L"");
        m_buttonWood->SetTextureName("ART_RESWood01");
        m_buttonWood->SetSize({ 165.0f, 68.f });
        m_buttonWood->SetPosition({ -450.f, 492.f });
        m_buttonWood->SetAnchor({ 0.0f, 1.0f });

        // 목재 텍스트
        m_curWoodText = CreateUIObject<Text>(L"UI_CurWoodText");
        m_curWoodText->SetText(to_wstring(ResourceSystem::Instance().GetTotalResource().m_wood));
        m_curWoodText->SetTextFormatName("Sebang_22");
        m_curWoodText->SetColor(D2D1::ColorF(0x69512C));
        m_curWoodText->SetSize({ 300, 100 });
        m_curWoodText->SetPosition({ -420, 489 });

        // 목재 초당 재생 총량 텍스트
        m_resWoodText = CreateUIObject<Text>(L"UI_ResWoodText");
        m_resWoodText->SetText(to_wstring(ResourceSystem::Instance().GetTotalResourcePerSec().m_wood));
        m_resWoodText->SetTextFormatName("Sebang_16");
        m_resWoodText->SetColor(D2D1::ColorF(0.839f, 0.741f, 0.580f));
        m_resWoodText->SetSize({ 300, 100 });
        m_resWoodText->SetPosition({ -362, 510 });

        ////////////////////////////////////////////////////////////////////////////////

        // 광물
        m_buttonMineral = CreateUIObject<Button>(L"UI_Mineral");
        m_buttonMineral->SetText(L"");
        m_buttonMineral->SetTextureName("ART_RESMineral01");
        m_buttonMineral->SetSize({ 165.0f, 64.f });
        m_buttonMineral->SetPosition({ -252.f, 489.f });
        m_buttonMineral->SetAnchor({ 0.0f, 1.0f });

        // 광물 텍스트
        m_curMineralText = CreateUIObject<Text>(L"UI_CurMineralText");
        m_curMineralText->SetText(to_wstring(ResourceSystem::Instance().GetTotalResource().m_mineral));
        m_curMineralText->SetTextFormatName("Sebang_22");
        m_curMineralText->SetColor(D2D1::ColorF(0x69512C));
        m_curMineralText->SetSize({ 300, 100 });
        m_curMineralText->SetPosition({ -222.5f, 489.0f });

        // 광물 초당 재생 총량 텍스트
        m_resMineralText = CreateUIObject<Text>(L"UI_ResMineralText");
        m_resMineralText->SetText(to_wstring(ResourceSystem::Instance().GetTotalResourcePerSec().m_mineral));
        m_resMineralText->SetTextFormatName("Sebang_16");
        m_resMineralText->SetColor(D2D1::ColorF(0.839f, 0.741f, 0.580f));
        m_resMineralText->SetSize({ 300, 100 });
        m_resMineralText->SetPosition({ -161, 510 });

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
        m_yearText->SetPosition({ 695, 390 });

        // 월 (Month) 텍스트
        m_monthText = CreateUIObject<Text>(L"UI_MonthText");
        m_monthText->SetText(L"12");
        m_monthText->SetTextFormatName("Sebang_24");
        m_monthText->SetColor(D2D1::ColorF(0x69512C));
        m_monthText->SetSize({ 300, 100 });
        m_monthText->SetPosition({ 800, 390 });

        // 일 (Day) 텍스트
        m_dayText = CreateUIObject<Text>(L"UI_DayText");
        m_dayText->SetText(L"18");
        m_dayText->SetTextFormatName("Sebang_24");
        m_dayText->SetColor(D2D1::ColorF(0x69512C));
        m_dayText->SetSize({ 300, 100 });
        m_dayText->SetPosition({ 890, 390 });

        ////////////////////////////////////////////////////////////////////////////////

        // 정지 버튼
        m_stopButton = CreateUIObject<Button>(L"UI_StopButton");
        m_stopButton->SetTextureName("ART_Pause01");
        m_stopButton->SetText(L"");
        m_stopButton->SetSize({ 60, 68 });
        m_stopButton->SetPosition({ 675.f, 475.f });

        // 정지 버튼 클릭하면 실행될 이벤트
        m_stopButton->AddOnClick("Quit Game", [this]()
            {
                if (isOpenOption) { return; }

                SetTimeScale(0.0f);
            });

        // 정지 버튼 마우스를 올리면 실행될 이벤트
        m_stopButton->AddOnEnter("Highlight On", [this]()
            {
                if (isOpenOption) { return; }
            });

        // 정지 버튼 마우스가 벗어나면 실행될 이벤트
        m_stopButton->AddOnExit("Highlight Off", [this]()
            {
                if (isOpenOption) { return; }
            });

        ////////////////////////////////////////////////////////////////////////////////

        // 재생 버튼
        m_playButton = CreateUIObject<Button>(L"UI_PlayButton");
        m_playButton->SetTextureName("ART_Play01");
        m_playButton->SetText(L"");
        m_playButton->SetSize({ 51, 65 });
        m_playButton->SetPosition({ 760.f, 475.f });

        // 재생 버튼 클릭하면 실행될 이벤트
        m_playButton->AddOnClick("Quit Game", [this]()
            {
                if (isOpenOption) { return; }

                SetTimeScale(1.0f);
            });

        // 재생 버튼 마우스를 올리면 실행될 이벤트
        m_playButton->AddOnEnter("Highlight On", [this]()
            {
                if (isOpenOption) { return; }
            });

        // 재생 버튼 마우스가 벗어나면 실행될 이벤트
        m_playButton->AddOnExit("Highlight Off", [this]()
            {
                if (isOpenOption) { return; }
            });

        ////////////////////////////////////////////////////////////////////////////////

        // 스피드 버튼
        m_speedButton = CreateUIObject<Button>(L"UI_SpeedButton");
        m_speedButton->SetTextureName("ART_Fast01");
        m_speedButton->SetText(L"");
        m_speedButton->SetSize({ 67, 72 });
        m_speedButton->SetPosition({ 842.f, 473.f });

        // 스피드 버튼 클릭하면 실행될 이벤트
        m_speedButton->AddOnClick("Quit Game", [this]()
            {
                if (isOpenOption) { return; }

                if (GetTimeScale() == 2.0f) {
                    SetTimeScale(4.0f);
                }
                else if (GetTimeScale() == 4.0f) {
                    SetTimeScale(8.0f);
                }
                else {
                    SetTimeScale(2.0f);
                }
            });

        // 스피드 버튼 마우스를 올리면 실행될 이벤트
        m_speedButton->AddOnEnter("Highlight On", [this]()
            {
                if (isOpenOption) { return; }
            });

        // 스피드 버튼 마우스가 벗어나면 실행될 이벤트
        m_speedButton->AddOnExit("Highlight Off", [this]()
            {
                if (isOpenOption) { return; }
            });

        ////////////////////////////////////////////////////////////////////////////////

        // 옵션 버튼
        m_optionButton = CreateUIObject<Button>(L"UI_OptionButton");
        m_optionButton->SetTextureName("ART_SettingIcon01");
        m_optionButton->SetText(L"");
        m_optionButton->SetSize({ 48.f, 50.5f });
        m_optionButton->SetPosition({ 923, 482 });

        // 옵션 버튼 클릭하면 실행될 이벤트
        m_optionButton->AddOnClick("Show Option", [this]()
            {
                if (isOpenOption) { return; }

                ShowOptionUI();
            });

        // 옵션 버튼 마우스를 올리면 실행될 이벤트
        m_optionButton->AddOnEnter("Highlight On", [this]()
            {
                if (isOpenOption) { return; }
            });

        // 옵션 버튼 마우스가 벗어나면 실행될 이벤트
        m_optionButton->AddOnExit("Highlight Off", [this]()
            {
                if (isOpenOption) { return; }
            });

        ////////////////////////////////////////////////////////////////////////////////

        // 원정 이미지
        m_away = CreateUIObject<Image>(L"UI_Away");
        m_away->SetTextureName("ART_Away01");
        m_away->SetSize({ 322, 65 });
        m_away->SetPosition({ 800, -300 });
        m_away->SetAnchor({ 1.0f, 0.0f });

        // 원정 현재 값
        m_awayCurValue = CreateUIObject<Text>(L"UI_AwayCurValue");
        m_awayCurValue->SetText(L"200");
        m_awayCurValue->SetTextFormatName("Sebang_Bold_22");
        m_awayCurValue->SetColor(D2D1::ColorF(0xD6BD94));
        m_awayCurValue->SetSize({ 100, 50 });
        m_awayCurValue->SetPosition({ 704, -300 });
        m_awayCurValue->SetAnchor({ 1.0f, 0.0f });

        // 원정 구분 선
        m_awayDivText = CreateUIObject<Text>(L"UI_AwayDivText");
        m_awayDivText->SetText(L"/");
        m_awayDivText->SetTextFormatName("Sebang_Bold_22");
        m_awayDivText->SetColor(D2D1::ColorF(0xD6BD94));
        m_awayDivText->SetSize({ 100, 50 });
        m_awayDivText->SetPosition({ 732, -300 });
        m_awayDivText->SetAnchor({ 1.0f, 0.0f });

        // 원정 최대 값
        m_awayMaxValue = CreateUIObject<Text>(L"UI_AwayMaxValue");
        m_awayMaxValue->SetText(L"200");
        m_awayMaxValue->SetTextFormatName("Sebang_Bold_22");
        m_awayMaxValue->SetColor(D2D1::ColorF(0xD6BD94));
        m_awayMaxValue->SetSize({ 100, 50 });
        m_awayMaxValue->SetPosition({ 760, -300 });
        m_awayMaxValue->SetAnchor({ 1.0f, 0.0f });

        ////////////////////////////////////////////////////////////////////////////////
        // 0) 기본 UI
        // TODO : 리소스가 아직 없음.
        m_defaultUI = CreateUIObject<Image>(L"UI_Default");
        m_defaultUI->SetTextureName("ART_UIBuilding01_Board");

        if (cam)
        {
            float screenWidth = static_cast<float>(cam->GetScreenWidth());
            float screenHeight = static_cast<float>(cam->GetScreenHeight());

            m_defaultUI->SetSize({ 1206.f, 185.5f });
            m_defaultUI->SetPosition({ 363.0f, -455.0f });
            m_defaultUI->SetPivot({ 0.5f, 0.5f });
            m_defaultUI->SetAnchor({ 1.0f, 0.0f });
        }

        ////////////////////////////////////////////////////////////////////////////////

#pragma region 1. CreateBuild
        // 1) 건설 UI
        m_buildUI = CreateUIObject<Image>(L"UI_Build");
        m_buildUI->SetTextureName("ART_UIBuilding01");

        if (cam)
        {
            float screenWidth = static_cast<float>(cam->GetScreenWidth());
            float screenHeight = static_cast<float>(cam->GetScreenHeight());

            // 화면 크기로 설정
            m_buildUI->SetSize({ 1206.f, 185.5f });
            m_buildUI->SetPosition({ 363.0f, -446.5f });
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
        m_buildTypeText->SetColor(D2D1::ColorF(0x2F3315));
        m_buildTypeText->SetSize({ 300, 100 });
        m_buildTypeText->SetPosition({ -63.8f, -385.5f });

        // 코스트 텍스트
        m_costInfoText = CreateUIObject<Text>(L"UI_BuildCostInfoText");
        m_costInfoText->SetText(L"코스트 :");
        m_costInfoText->SetTextFormatName("Sebang_22");
        m_costInfoText->SetColor(D2D1::ColorF(0x2F3315));
        m_costInfoText->SetSize({ 300, 100 });
        m_costInfoText->SetPosition({ -126, -435 });


        JDGlobal::Contents::HouseStats stat;
        DataTableManager::Instance().GetHouseTable(stat);

        m_costText = CreateUIObject<Text>(L"UI_BuildCostText");
        m_costText->SetText(to_wstring(stat.m_upgradeCost[0].m_wood));
        m_costText->SetTextFormatName("Sebang_22");
        m_costText->SetColor(D2D1::ColorF(0x2F3315));
        m_costText->SetSize({ 300, 100 });
        m_costText->SetPosition({ 2, -432 });

        // 코스트 이미지
        m_costImage = CreateUIObject<Image>(L"UI_BuildCostImage");
        m_costImage->SetTextureName("ART_CostWood01");
        m_costImage->SetSize({ 50, 36 });
        m_costImage->SetPosition({ -46, -428 });
        m_costImage->SetAnchor({ 1.0f, 0.0f });

        // 효과 텍스트
        m_effectInfoText = CreateUIObject<Text>(L"UI_BuildEffectInfoText");
        m_effectInfoText->SetText(L"효과 :");
        m_effectInfoText->SetTextFormatName("Sebang_22");
        m_effectInfoText->SetColor(D2D1::ColorF(0x2F3315));
        m_effectInfoText->SetSize({ 300, 100 });
        m_effectInfoText->SetPosition({ -126, -479 });

        m_effectText = CreateUIObject<Text>(L"UI_BuildEffectText");
        m_effectText->SetText(L"/초");
        m_effectText->SetTextFormatName("Sebang_22");
        m_effectText->SetColor(D2D1::ColorF(0x2F3315));
        m_effectText->SetSize({ 300, 100 });
        m_effectText->SetPosition({ 2, -478 });

        // 효과 이미지
        m_effctImage = CreateUIObject<Image>(L"UI_BuildEffctImage");
        m_effctImage->SetTextureName("ART_CostFood01");
        m_effctImage->SetSizeToOriginal();
        m_effctImage->SetPosition({ -63, -461 });
        m_effctImage->SetAnchor({ 1.0f, 0.0f });
        m_effctImage->SetScale({ 0.5f, 0.5f });

        ////////////////////////////////////////
        // 건설 UI _ 주거지
        ////////////////////////////////////////
        m_buildHouse = CreateUIObject<Button>(L"UI_Build_House");
        m_buildHouse->SetTextureName("ART_BuildCabin01");
        m_buildHouse->SetText(L"");
        m_buildHouse->SetSize({ 166.f, 166.f });
        m_buildHouse->SetPosition({ 200.f, -430.f });

        // 주거지 버튼 클릭하면 실행될 이벤트
        m_buildHouse->AddOnClick("Clicked House", [this]()
            {
                if (isOpenOption) { return; }

                wcout << L"주거지 선택이되..................." << m_selectedCollider->GetOwner()->GetName() << endl;
                wcout << L"주거지 선택이되..................." << endl;
                wcout << L"주거지 선택이되..................." << endl;
                wcout << L"주거지 선택이되..................." << endl;

                if (m_selectedCollider) {
                    auto* boxCol = static_cast<JDComponent::BoxCollider*>(m_selectedCollider);

                    Grid* grid = dynamic_cast<Grid*>(boxCol->GetOwner());

                    // 그리드 건물 유무 확인
                    if (grid) {
                        if (!grid->HasBuilding() && grid->IsOccupied()) { // 그리드가 빌딩이 없고, 점유중이면
                            grid->SetHasBuilding(true);
                        }
                        else {
                            cout << "[Error] 점유되지 않은 구역입니다. 건물 설치 취소!!" << endl;
                            CloseGridCreateMenu();
                            return;
                        }
                    }
                    //콜라이더 월드 위치 및 스케일 가져와서 이미지에 적용
                    Vector2F tileWorldPos = m_selectedCollider->GetOwner()
                        ->GetComponent<Transform>()->GetWorldPosition();

                    House* building = CreateGameObject<House>(L"House_" + boxCol->GetOwner()->GetName());

                    if (building) {
                        //m_TutorialObjects.push_back(building);
                        grid->SetBuilding(building); // 그리드에 빌딩 할당
                        grid->SetHasBuilding(true); // 그리드 건물 상태 flag : true
                        std::cout << "건물 추가됨" << std::endl;

                        auto* tr = building->AddComponent<TextureRenderer>(
                            "ART_TileCabin01",
                            RenderLayerInfo{ SortingLayer::Grid, 0 }
                        );

                        tr->SetTextureName("ART_TileCabin01");
                        tr->SetSize({ boxCol->GetSize().x, boxCol->GetSize().y });

                        building->GetComponent<Transform>()->SetPosition(tileWorldPos);
                    }
                    else
                    {
                        std::cout << "건물 추가 에러발생" << std::endl;
                    }
                    // 6) 메뉴 닫기 & 버튼 숨기기
                    CloseGridCreateMenu();
                    /*m_Menu->SetActive(false);
                    for (auto* btn : m_menuButtons)
                        if (btn) btn->SetActive(false);*/

                        // 7) 선택 초기화
                    m_selectedTool = nullptr;
                    m_selectedCollider = nullptr;
                }
            });

        // 주거지 버튼 마우스를 올리면 실행될 이벤트
        m_buildHouse->AddOnEnter("Highlight On", [this]()
            {
                if (isOpenOption) { return; }

                ShowGridCreateInfo(JDGlobal::Contents::BuildingType::House, "50", "1/초");
            });

        // 주거지 버튼 마우스가 벗어나면 실행될 이벤트
        m_buildHouse->AddOnExit("Highlight Off", [this]()
            {
                if (isOpenOption) { return; }

                CloseGridCreateInfo();
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
        m_buildFishingspot->AddOnClick("Clicked FishingSpot", [this]()
            {
                if (isOpenOption) { return; }

                wcout << L"낚시터 선택이되..................." << m_selectedCollider->GetOwner()->GetName() << endl;
                wcout << L"낚시터 선택이되..................." << endl;
                wcout << L"낚시터 선택이되..................." << endl;
                wcout << L"낚시터 선택이되..................." << endl;

                if (m_selectedCollider)
                {
                    auto* boxCol = static_cast<JDComponent::BoxCollider*>(m_selectedCollider);

                    Grid* grid = dynamic_cast<Grid*>(boxCol->GetOwner());

                    // 그리드 건물 유무 확인
                    if (grid) {
                        if (!grid->HasBuilding() && grid->IsOccupied()) { // 그리드가 빌딩이 없고, 점유중이면
                            grid->SetHasBuilding(true);
                        }
                        else {
                            cout << "[Error] 점유되지 않은 구역입니다. 건물 설치 취소!!" << endl;
                            CloseGridCreateMenu();
                            return;
                        }
                    }
                    //콜라이더 월드 위치 및 스케일 가져와서 이미지에 적용
                    Vector2F tileWorldPos = m_selectedCollider->GetOwner()
                        ->GetComponent<Transform>()->GetWorldPosition();

                    FishingSpot* building = CreateGameObject<FishingSpot>(L"FishingSpot_" + boxCol->GetOwner()->GetName());

                    if (building) {
                        //m_TutorialObjects.push_back(building);
                        grid->SetBuilding(building); // 그리드에 빌딩 할당
                        grid->SetHasBuilding(true); // 그리드 건물 상태 flag : true
                        std::cout << "건물 추가됨" << std::endl;

                        auto* tr = building->AddComponent<TextureRenderer>(
                            "ART_TileFishing01",
                            RenderLayerInfo{ SortingLayer::Grid, 0 }
                        );

                        tr->SetTextureName("ART_TileFishing01");
                        tr->SetSize({ boxCol->GetSize().x, boxCol->GetSize().y });

                        building->GetComponent<Transform>()->SetPosition(tileWorldPos);
                    }
                    else
                    {
                        std::cout << "건물 추가 에러발생" << std::endl;
                    }
                    // 6) 메뉴 닫기 & 버튼 숨기기
                    CloseGridCreateMenu();
                    /*m_Menu->SetActive(false);
                    for (auto* btn : m_menuButtons)
                        if (btn) btn->SetActive(false);*/

                        // 7) 선택 초기화
                    m_selectedTool = nullptr;
                    m_selectedCollider = nullptr;
                }
            });

        // 낚시터 버튼 마우스를 올리면 실행될 이벤트
        m_buildFishingspot->AddOnEnter("Highlight On", [this]()
            {
                if (isOpenOption) { return; }

                ShowGridCreateInfo(JDGlobal::Contents::BuildingType::FishingSpot, "40", "2/초");
            });

        // 낚시터 버튼 마우스가 벗어나면 실행될 이벤트
        m_buildFishingspot->AddOnExit("Highlight Off", [this]()
            {
                if (isOpenOption) { return; }

                CloseGridCreateInfo();
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
        m_buildRumbermill->AddOnClick("Clicked LumberMill", [this]()
            {
                if (isOpenOption) { return; }

                wcout << L"제재소 선택이되..................." << m_selectedCollider->GetOwner()->GetName() << endl;
                wcout << L"제재소 선택이되..................." << endl;
                wcout << L"제재소 선택이되..................." << endl;
                wcout << L"제재소 선택이되..................." << endl;

                if (m_selectedCollider) {
                    auto* boxCol = static_cast<JDComponent::BoxCollider*>(m_selectedCollider);

                    Grid* grid = dynamic_cast<Grid*>(boxCol->GetOwner());

                    // 그리드 건물 유무 확인
                    if (grid) {
                        if (!grid->HasBuilding() && grid->IsOccupied()) { // 그리드가 빌딩이 없고, 점유중이면
                            grid->SetHasBuilding(true);
                        }
                        else {
                            cout << "[Error] 점유되지 않은 구역입니다. 건물 설치 취소!!" << endl;
                            CloseGridCreateMenu();
                            return;
                        }
                    }
                    //콜라이더 월드 위치 및 스케일 가져와서 이미지에 적용
                    Vector2F tileWorldPos = m_selectedCollider->GetOwner()
                        ->GetComponent<Transform>()->GetWorldPosition();

                    LumberMill* building = CreateGameObject<LumberMill>(L"LumberMill_" + boxCol->GetOwner()->GetName());

                    if (building) {
                        //m_TutorialObjects.push_back(building);
                        grid->SetBuilding(building); // 그리드에 빌딩 할당
                        grid->SetHasBuilding(true); // 그리드 건물 상태 flag : true
                        std::cout << "건물 추가됨" << std::endl;

                        auto* tr = building->AddComponent<TextureRenderer>(
                            "ART_TileLumbermill01",
                            RenderLayerInfo{ SortingLayer::Grid, 0 }
                        );

                        tr->SetTextureName("ART_TileLumbermill01");
                        tr->SetSize({ boxCol->GetSize().x, boxCol->GetSize().y });

                        building->GetComponent<Transform>()->SetPosition(tileWorldPos);
                    }
                    else
                    {
                        std::cout << "건물 추가 에러발생" << std::endl;
                    }
                    // 6) 메뉴 닫기 & 버튼 숨기기
                    CloseGridCreateMenu();
                    /*m_Menu->SetActive(false);
                    for (auto* btn : m_menuButtons)
                        if (btn) btn->SetActive(false);*/

                        // 7) 선택 초기화
                    m_selectedTool = nullptr;
                    m_selectedCollider = nullptr;
                }
            });

        // 제재소 버튼 마우스를 올리면 실행될 이벤트
        m_buildRumbermill->AddOnEnter("Highlight On", [this]()
            {
                if (isOpenOption) { return; }

                ShowGridCreateInfo(JDGlobal::Contents::BuildingType::LumberMill, "30", "3/초");
            });

        // 제재소 버튼 마우스가 벗어나면 실행될 이벤트
        m_buildRumbermill->AddOnExit("Highlight Off", [this]()
            {
                if (isOpenOption) { return; }

                CloseGridCreateInfo();
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
        m_buildMine->AddOnClick("Clicked Mine", [this]()
            {
                if (isOpenOption) { return; }

                wcout << L"광산 선택이되..................." << m_selectedCollider->GetOwner()->GetName() << endl;
                wcout << L"광산 선택이되..................." << endl;
                wcout << L"광산 선택이되..................." << endl;
                wcout << L"광산 선택이되..................." << endl;

                if (m_selectedCollider)
                {
                    auto* boxCol = static_cast<JDComponent::BoxCollider*>(m_selectedCollider);

                    Grid* grid = dynamic_cast<Grid*>(boxCol->GetOwner());

                    // 그리드 건물 유무 확인
                    if (grid) {
                        if (!grid->HasBuilding() && grid->IsOccupied()) { // 그리드가 빌딩이 없고, 점유중이면
                            grid->SetHasBuilding(true);
                        }
                        else {
                            cout << "[Error] 점유되지 않은 구역입니다. 건물 설치 취소!!" << endl;
                            CloseGridCreateMenu();
                            return;
                        }
                    }
                    //콜라이더 월드 위치 및 스케일 가져와서 이미지에 적용
                    Vector2F tileWorldPos = m_selectedCollider->GetOwner()
                        ->GetComponent<Transform>()->GetWorldPosition();

                    Mine* building = CreateGameObject<Mine>(L"Mine_" + boxCol->GetOwner()->GetName());

                    if (building) {
                        //m_TutorialObjects.push_back(building);
                        grid->SetBuilding(building); // 그리드에 빌딩 할당
                        grid->SetHasBuilding(true); // 그리드 건물 상태 flag : true
                        std::cout << "건물 추가됨" << std::endl;

                        auto* tr = building->AddComponent<TextureRenderer>(
                            "ART_TileMine01",
                            RenderLayerInfo{ SortingLayer::Grid, 0 }
                        );

                        tr->SetTextureName("ART_TileMine01");
                        tr->SetSize({ boxCol->GetSize().x, boxCol->GetSize().y });

                        building->GetComponent<Transform>()->SetPosition(tileWorldPos);
                    }
                    else
                    {
                        std::cout << "건물 추가 에러발생" << std::endl;
                    }
                    // 6) 메뉴 닫기 & 버튼 숨기기
                    CloseGridCreateMenu();
                    /*m_Menu->SetActive(false);
                    for (auto* btn : m_menuButtons)
                        if (btn) btn->SetActive(false);*/

                        // 7) 선택 초기화
                    m_selectedTool = nullptr;
                    m_selectedCollider = nullptr;
                }
            });

        // 광산 버튼 마우스를 올리면 실행될 이벤트
        m_buildMine->AddOnEnter("Highlight On", [this]()
            {
                if (isOpenOption) { return; }

                ShowGridCreateInfo(JDGlobal::Contents::BuildingType::Mine, "20", "4/초");
            });

        // 광산 버튼 마우스가 벗어나면 실행될 이벤트
        m_buildMine->AddOnExit("Highlight Off", [this]()
            {
                if (isOpenOption) { return; }

                CloseGridCreateInfo();
            });

        ////////////////////////////////////////////////////////////////////////////////
#pragma endregion

#pragma region 2. UpgradeBuild

        // 2) 건물 업그레이트 및 고양이 배치 UI
        m_upgradeUI = CreateUIObject<Image>(L"UI_Upgrade");
        m_upgradeUI->SetTextureName("ART_UICharSelect01");

        if (cam)
        {
            float screenWidth = static_cast<float>(cam->GetScreenWidth());
            float screenHeight = static_cast<float>(cam->GetScreenHeight());

            // 화면 크기로 설정
            m_upgradeUI->SetSize({ 1206.0f, 185.5f });
            m_upgradeUI->SetPosition({ 363.1f, -447.7f });
            m_upgradeUI->SetPivot({ 0.5f, 0.5f });
            m_upgradeUI->SetAnchor({ 1.0f, 0.0f });
        }


        ////////////////////////////////////////
        // 고양이 배치
        ////////////////////////////////////////

        // 고양이 배치
        m_catTypeText = CreateUIObject<Text>(L"UI_CatTypeText");
        m_catTypeText->SetText(L"배치할 고양이");
        m_catTypeText->SetTextFormatName("Sebang_Bold_24");
        m_catTypeText->SetColor(D2D1::ColorF(0x2F3315));
        m_catTypeText->SetSize({ 300, 100 });
        m_catTypeText->SetPosition({ -63.8f, -385.5f });

        // 고양이 배치 코스트 텍스트
        m_catCostInfoText = CreateUIObject<Text>(L"UI_CatCostInfoText");
        m_catCostInfoText->SetText(L"코스트 :");
        m_catCostInfoText->SetTextFormatName("Sebang_22");
        m_catCostInfoText->SetColor(D2D1::ColorF(0x2F3315));
        m_catCostInfoText->SetSize({ 300, 100 });
        m_catCostInfoText->SetPosition({ -126, -435 });

        m_catCostText = CreateUIObject<Text>(L"UI_CatCostText");
        m_catCostText->SetText(L"50");
        m_catCostText->SetTextFormatName("Sebang_20");
        m_catCostText->SetColor(D2D1::ColorF(0x2F3315));
        m_catCostText->SetSize({ 300, 100 });
        m_catCostText->SetPosition({ 2, -432 });

        // 코스트 이미지
        m_catCostImage = CreateUIObject<Image>(L"UI_CatCostImage");
        m_catCostImage->SetTextureName("ART_CostWood01");
        m_catCostImage->SetSize({ 50, 36 });
        m_catCostImage->SetPosition({ -46, -428 });
        m_catCostImage->SetAnchor({ 1.0f, 0.0f });

        // 고양이 효과 텍스트
        m_catEffectInfoText = CreateUIObject<Text>(L"UI_CatEffectInfoText");
        m_catEffectInfoText->SetText(L"효과 :");
        m_catEffectInfoText->SetTextFormatName("Sebang_22");
        m_catEffectInfoText->SetColor(D2D1::ColorF(0x2F3315));
        m_catEffectInfoText->SetSize({ 300, 100 });
        m_catEffectInfoText->SetPosition({ -126, -479 });

        m_catEffectText = CreateUIObject<Text>(L"UI_CatEffectText");
        m_catEffectText->SetText(L"1/초");
        m_catEffectText->SetTextFormatName("Sebang_20");
        m_catEffectText->SetColor(D2D1::ColorF(0x2F3315));
        m_catEffectText->SetSize({ 300, 100 });
        m_catEffectText->SetPosition({ 2, -478 });

        // 고양이 효과 이미지
        m_catEffctImage = CreateUIObject<Image>(L"UI_CatEffctImage");
        m_catEffctImage->SetTextureName("ART_CostFood01");
        m_catEffctImage->SetSizeToOriginal();
        m_catEffctImage->SetPosition({ -72, -461 });
        m_catEffctImage->SetAnchor({ 1.0f, 0.0f });
        m_catEffctImage->SetScale({ 0.5f, 0.5f });


        ////////////////////////////////////////
        // 고양이 선택 버튼
        ////////////////////////////////////////

        //////////
        // 코네 선택 버튼
        m_koneSetButton = CreateUIObject<Button>(L"UI_SetKoneButton");
        m_koneSetButton->SetTextureName("ART_CatKone01");
        m_koneSetButton->SetText(L"");
        m_koneSetButton->SetSize({ 278.f, 367.f });
        m_koneSetButton->SetPosition({ 62, -350 });
        m_koneSetButton->SetScale({ 0.5f, 0.5f });

        // 코네 선택 버튼 클릭하면 실행될 이벤트
        m_koneSetButton->AddOnClick("On Click", [this]()
            {
                if (isOpenOption) { return; }

                wcout << L"코네냥 선택이되..................." << m_selectedCollider->GetOwner()->GetName() << endl;
                wcout << L"코네냥 선택이되..................." << endl;
                wcout << L"코네냥 선택이되..................." << endl;
                wcout << L"코네냥 선택이되..................." << endl;

                if (m_selectedCollider)
                {
                    auto* boxCol = static_cast<JDComponent::BoxCollider*>(m_selectedCollider);
                    Grid* grid = dynamic_cast<Grid*>(boxCol->GetOwner());

                    // 그리드 건물 유무 확인
                    if (grid) {
                        if (!grid->IsOccupied()) {
                            cout << "[Error] 점유되지 않은 구역입니다. 건물 설치 취소!!" << endl;
                            CloseGridSettingMenu();
                            return;
                        }

                        if (grid->HasBuilding()) { // 그리드가 빌딩이 있고, 점유중이면
                            Building* building = dynamic_cast<Building*>(grid->GetBuilding());

                            if (building) {
                                building->ChangeCat(CatType::Kone);
                                building->PrintCat();
                            }

                            // 6) 메뉴 닫기 & 버튼 숨기기
                            CloseGridSettingMenu();
                            /*m_Menu->SetActive(false);
                            for (auto* btn : m_menuButtons)
                                if (btn) btn->SetActive(false);*/

                                // 7) 선택 초기화
                            m_selectedTool = nullptr;
                            m_selectedCollider = nullptr;
                        }
                    }
                }
            });

        // 코네 선택 버튼 마우스를 올리면 실행될 이벤트
        m_koneSetButton->AddOnEnter("Highlight On", [this]()
            {
                if (isOpenOption) { return; }

                ShowCatInfo(JDGlobal::Contents::CatType::Kone, "50", "1/초");
            });

        // 코네 선택 버튼 마우스가 벗어나면 실행될 이벤트
        m_koneSetButton->AddOnExit("Highlight Off", [this]()
            {
                if (isOpenOption) { return; }

                CloseCatInfo();
            });

        //////////
        // 나비냥 선택 버튼
        m_naviSetButton = CreateUIObject<Button>(L"UI_SetNaviButton");
        m_naviSetButton->SetTextureName("ART_CatNavi01");
        m_naviSetButton->SetText(L"");
        m_naviSetButton->SetSize({ 278.f, 348.f });
        m_naviSetButton->SetPosition({ 209.5f, -360.f });
        m_naviSetButton->SetScale({ 0.5f, 0.5f });

        // 나비냥 선택 버튼 클릭하면 실행될 이벤트
        m_naviSetButton->AddOnClick("On Click", [this]()
            {
                if (isOpenOption) { return; }

                wcout << L"나비냥 선택이되..................." << m_selectedCollider->GetOwner()->GetName() << endl;
                wcout << L"나비냥 선택이되..................." << endl;
                wcout << L"나비냥 선택이되..................." << endl;
                wcout << L"나비냥 선택이되..................." << endl;

                if (m_selectedCollider) {
                    auto* boxCol = static_cast<JDComponent::BoxCollider*>(m_selectedCollider);
                    Grid* grid = dynamic_cast<Grid*>(boxCol->GetOwner());

                    // 그리드 건물 유무 확인
                    if (grid) {
                        if (!grid->IsOccupied()) {
                            cout << "[Error] 점유되지 않은 구역입니다. 건물 설치 취소!!" << endl;
                            CloseGridSettingMenu();
                            return;
                        }

                        if (grid->HasBuilding()) { // 그리드가 빌딩이 있고, 점유중이면
                            Building* building = dynamic_cast<Building*>(grid->GetBuilding());

                            if (building) {
                                building->ChangeCat(CatType::Navi);
                            }

                            // 6) 메뉴 닫기 & 버튼 숨기기
                            CloseGridSettingMenu();
                            /*m_Menu->SetActive(false);
                            for (auto* btn : m_menuButtons)
                                if (btn) btn->SetActive(false);*/

                                // 7) 선택 초기화
                            m_selectedTool = nullptr;
                            m_selectedCollider = nullptr;
                        }
                    }
                }
            });

        // 나비냥 선택 버튼 마우스를 올리면 실행될 이벤트
        m_naviSetButton->AddOnEnter("Highlight On", [this]()
            {
                if (isOpenOption) { return; }

                ShowCatInfo(JDGlobal::Contents::CatType::Navi, "50", "1/초");
            });

        // 나비냥 선택 버튼 마우스가 벗어나면 실행될 이벤트
        m_naviSetButton->AddOnExit("Highlight Off", [this]()
            {
                if (isOpenOption) { return; }

                CloseCatInfo();
            });

        //////////
        // 펠리스 선택 버튼
        m_felisSetButton = CreateUIObject<Button>(L"UI_SetFelisButton");
        m_felisSetButton->SetTextureName("ART_CatFelis01");
        m_felisSetButton->SetText(L"");
        m_felisSetButton->SetSize({ 278.f, 348.f });
        m_felisSetButton->SetPosition({ 354, -360 });
        m_felisSetButton->SetScale({ 0.5f, 0.5f });

        // 펠리스 선택 버튼 클릭하면 실행될 이벤트
        m_felisSetButton->AddOnClick("On Click", [this]()
            {
                if (isOpenOption) { return; }

                wcout << L"펠리스냥 선택이되..................." << m_selectedCollider->GetOwner()->GetName() << endl;
                wcout << L"펠리스냥 선택이되..................." << endl;
                wcout << L"펠리스냥 선택이되..................." << endl;
                wcout << L"펠리스냥 선택이되..................." << endl;

                if (m_selectedCollider) {
                    auto* boxCol = static_cast<JDComponent::BoxCollider*>(m_selectedCollider);
                    Grid* grid = dynamic_cast<Grid*>(boxCol->GetOwner());

                    // 그리드 건물 유무 확인
                    if (grid) {
                        if (!grid->IsOccupied()) {
                            cout << "[Error] 점유되지 않은 구역입니다. 건물 설치 취소!!" << endl;
                            CloseGridSettingMenu();
                            return;
                        }

                        if (grid->HasBuilding()) { // 그리드가 빌딩이 있고, 점유중이면
                            Building* building = dynamic_cast<Building*>(grid->GetBuilding());

                            if (building) {
                                building->ChangeCat(CatType::Felis);
                            }

                            // 6) 메뉴 닫기 & 버튼 숨기기
                            CloseGridSettingMenu();
                            /*m_Menu->SetActive(false);
                            for (auto* btn : m_menuButtons)
                                if (btn) btn->SetActive(false);*/

                                // 7) 선택 초기화
                            m_selectedTool = nullptr;
                            m_selectedCollider = nullptr;
                        }
                    }
                }
            });

        // 펠리스 선택 버튼 마우스를 올리면 실행될 이벤트
        m_felisSetButton->AddOnEnter("Highlight On", [this]()
            {
                if (isOpenOption) { return; }

                ShowCatInfo(JDGlobal::Contents::CatType::Felis, "50", "1/초");
            });

        // 펠리스 선택 버튼 마우스가 벗어나면 실행될 이벤트
        m_felisSetButton->AddOnExit("Highlight Off", [this]()
            {
                if (isOpenOption) { return; }

                CloseCatInfo();
            });


        ////////////////////////////////////////
        // 건물 업그레이드 정보
        ////////////////////////////////////////

        // 건물 타입 텍스트
        m_builtTypeText = CreateUIObject<Text>(L"UI_BuilTTypeText");
        m_builtTypeText->SetText(L"업그레이드");
        m_builtTypeText->SetTextFormatName("Sebang_Bold_24");
        m_builtTypeText->SetColor(D2D1::ColorF(0x2F3315));
        m_builtTypeText->SetSize({ 300, 100 });
        m_builtTypeText->SetPosition({ 757.f, -385.5f });

        // 건물 업그레이드 코스트
        m_upgradeCostInfoText = CreateUIObject<Text>(L"UI_UpgradeCostInfoText");
        m_upgradeCostInfoText->SetText(L"코스트 :");
        m_upgradeCostInfoText->SetTextFormatName("Sebang_22");
        m_upgradeCostInfoText->SetColor(D2D1::ColorF(0x2F3315));
        m_upgradeCostInfoText->SetSize({ 300, 100 });
        m_upgradeCostInfoText->SetPosition({ 697, -435 });

        m_upgradeCostText = CreateUIObject<Text>(L"UI_UpgradeCostText");
        m_upgradeCostText->SetText(L"50");
        m_upgradeCostText->SetTextFormatName("Sebang_20");
        m_upgradeCostText->SetColor(D2D1::ColorF(0x2F3315));
        m_upgradeCostText->SetSize({ 300, 100 });
        m_upgradeCostText->SetPosition({ 824.5, -432 });

       
        int level = 0;
        BuildingType buldingType = BuildingType::FishingSpot;
        if (m_selectedCollider) {
            auto* boxCol = static_cast<JDComponent::BoxCollider*>(m_selectedCollider);
            Grid* grid = dynamic_cast<Grid*>(boxCol->GetOwner());

            // 그리드 건물 유무 확인
            if (grid) {
                if (!grid->IsOccupied()) {
                    cout << "[Error] 점유되지 않은 구역입니다. 건물 설치 취소!!" << endl;
                    CloseGridSettingMenu();
                    return;
                }

                if (grid->HasBuilding()) { // 그리드가 빌딩이 있고, 점유중이면
                    if(FishingSpot* building = dynamic_cast<FishingSpot*>(grid->GetBuilding())) {
                        level = building->GetLevel();
                        buldingType = BuildingType::FishingSpot;
                    }
                    else if (LumberMill* building = dynamic_cast<LumberMill*>(grid->GetBuilding())) {
                        level = building->GetLevel();
                        buldingType = BuildingType::LumberMill;
                    }
                    else if (Mine* building = dynamic_cast<Mine*>(grid->GetBuilding())) {
                        level = building->GetLevel();
                        buldingType = BuildingType::Mine;
                    }
                    else if (House* building = dynamic_cast<House*>(grid->GetBuilding())) {
                        level = building->GetLevel();
                        buldingType = BuildingType::House;
                    }

                }
            }
        }

        // 업그레이드 코스트 이미지
        m_upgradeCostImage = CreateUIObject<Image>(L"UI_UpgradeCostImage");
        m_upgradeCostImage->SetTextureName("ART_CostWood01");
        m_upgradeCostImage->SetSize({ 50, 36 });
        m_upgradeCostImage->SetPosition({ 778, -430 });
        m_upgradeCostImage->SetAnchor({ 1.0f, 0.0f });

        // 업그레이드 효과 텍스트
        m_upgradeEffectInfoText = CreateUIObject<Text>(L"UI_UpgradeEffectInfoText");
        m_upgradeEffectInfoText->SetText(L"효과 :");
        m_upgradeEffectInfoText->SetTextFormatName("Sebang_22");
        m_upgradeEffectInfoText->SetColor(D2D1::ColorF(0x2F3315));
        m_upgradeEffectInfoText->SetSize({ 300, 100 });
        m_upgradeEffectInfoText->SetPosition({ 702, -479 });


        m_upgradeEffectText = CreateUIObject<Text>(L"UI_UpgradeEffectText");
        switch (buldingType) {
        case BuildingType::House: m_upgradeEffectText->SetText(to_wstring(m_dataTableManager->GetHouseTableInfo().m_initPopulation[level]) + L"/초"); break;
        case BuildingType::FishingSpot: m_upgradeEffectText->SetText(to_wstring(m_dataTableManager->GetFishingSpotTableInfo().m_resourceGenPerSec[level].m_food)+L"/초"); break;
        case BuildingType::LumberMill: m_upgradeEffectText->SetText(to_wstring(m_dataTableManager->GetLumbermillTableInfo().m_resourceGenPerSec[level].m_wood) + L"/초"); break;
        case BuildingType::Mine: m_upgradeEffectText->SetText(to_wstring(m_dataTableManager->GetMineTableInfo().m_resourceGenPerSec[level].m_mineral) + L"/초"); break;
        }
        
        m_upgradeEffectText->SetTextFormatName("Sebang_20");
        m_upgradeEffectText->SetColor(D2D1::ColorF(0x2F3315));
        m_upgradeEffectText->SetSize({ 300, 100 });
        m_upgradeEffectText->SetPosition({ 827, -478 });

        // 업그레이드 효과 이미지
        m_upgradeEffctImage = CreateUIObject<Image>(L"UI_UpgradeEffctImage");
        switch (buldingType) {
        case BuildingType::House: m_upgradeEffctImage->SetTextureName("ART_CostPop01"); break;
        case BuildingType::FishingSpot: m_upgradeEffctImage->SetTextureName("ART_CostFood01"); break;
        case BuildingType::LumberMill: m_upgradeEffctImage->SetTextureName("ART_CostWood01"); break;
        case BuildingType::Mine: m_upgradeEffctImage->SetTextureName("ART_CostMineral01"); break;
        }
        
        m_upgradeEffctImage->SetSizeToOriginal();
        m_upgradeEffctImage->SetPosition({ 754, -460 });
        m_upgradeEffctImage->SetAnchor({ 1.0f, 0.0f });
        m_upgradeEffctImage->SetScale({ 0.5f, 0.5f });
       

        ////////////////////////////////////////
        // 건물 업그레이드 & 다운그레이드 버튼
        ////////////////////////////////////////

        //////////
        // 다운그레이드 버튼
        m_downgradeButton = CreateUIObject<Button>(L"UI_downgradeButton");
        m_downgradeButton->SetTextureName("ART_Downgrade01");
        m_downgradeButton->SetText(L"");
        m_downgradeButton->SetSize({ 56.f, 40.f });
        m_downgradeButton->SetPosition({ 670, -390 });

        // 다운그레이드 버튼 클릭하면 실행될 이벤트
        m_downgradeButton->AddOnClick("On Click", [this]()
            {
                if (isOpenOption) { return; }

                if (m_selectedCollider) {
                    wcout << L"다운그레이드 선택이되..................." << m_selectedCollider->GetOwner()->GetName() << endl;
                    wcout << L"다운그레이드 선택이되..................." << endl;
                    wcout << L"다운그레이드 선택이되..................." << endl;
                    wcout << L"다운그레이드 선택이되..................." << endl;

                    auto* boxCol = static_cast<JDComponent::BoxCollider*>(m_selectedCollider);
                    Grid* grid = dynamic_cast<Grid*>(boxCol->GetOwner());

                    if (!grid) return;

                    auto* building = dynamic_cast<Building*>(grid->GetBuilding());
                    if (building) {
                        if (building->GetLevel() > 0) building->LevelDown();
                        else {
                            cout << "빌딩 파괴!!!" << endl;
                            grid->SetHasBuilding(false);
                            grid->SetBuilding(nullptr);
                            this->DestroyObject(building);
                        }
                    }
                    else if (auto* house = dynamic_cast<House*>(grid->GetBuilding())) {
                        if (house->GetLevel() > 0) house->LevelDown();
                        else {
                            cout << "집 파괴!!!" << endl;
                            grid->SetHasBuilding(false);
                            grid->SetBuilding(nullptr);
                            this->DestroyObject(house);

                        }
                    }
                }
            });

        // 다운그레이드 버튼 마우스를 올리면 실행될 이벤트
        m_downgradeButton->AddOnEnter("Highlight On", [this]()
            {
                if (isOpenOption) { return; }
            });

        // 다운그레이드 버튼 마우스가 벗어나면 실행될 이벤트
        m_downgradeButton->AddOnExit("Highlight Off", [this]()
            {
                if (isOpenOption) { return; }
            });

        //////////
        // 업그레이드 버튼
        m_upgradeButton = CreateUIObject<Button>(L"UI_upgradeButton");
        m_upgradeButton->SetTextureName("ART_Upgrade01");
        m_upgradeButton->SetText(L"");
        m_upgradeButton->SetSize({ 56.f, 40.f });
        m_upgradeButton->SetPosition({ 858, -390 });

        // 업그레이드 버튼 클릭하면 실행될 이벤트
        m_upgradeButton->AddOnClick("OnClick", [this]()
            {
                if (isOpenOption) { return; }

                if (m_selectedCollider) {
                    wcout << L"업그레이드 선택이되..................." << m_selectedCollider->GetOwner()->GetName() << endl;
                    wcout << L"업그레이드 선택이되..................." << endl;
                    wcout << L"업그레이드 선택이되..................." << endl;
                    wcout << L"업그레이드 선택이되..................." << endl;

                    auto* boxCol = static_cast<JDComponent::BoxCollider*>(m_selectedCollider);
                    Grid* grid = dynamic_cast<Grid*>(boxCol->GetOwner());

                    if (!grid) return;

                    auto* building = dynamic_cast<Building*>(grid->GetBuilding());
                    if (building) {
                        building->LevelUp();
                    }
                    else if (auto* house = dynamic_cast<House*>(grid->GetBuilding())) {
                        house->LevelUp();
                    }
                }
            });

        // 업그레이드 버튼 마우스를 올리면 실행될 이벤트
        m_upgradeButton->AddOnEnter("Highlight On", [this]()
            {
                if (isOpenOption) { return; }
            });

        // 업그레이드 버튼 마우스가 벗어나면 실행될 이벤트
        m_upgradeButton->AddOnExit("Highlight Off", [this]()
            {
                if (isOpenOption) { return; }
            });

#pragma endregion

#pragma region 3. Away

        // 3) 징병 및 원정 UI
        m_awayUI = CreateUIObject<Image>(L"UI_Away");
        m_awayUI->SetTextureName("ART_UIRecruit01");

        if (cam)
        {
            float screenWidth = static_cast<float>(cam->GetScreenWidth());
            float screenHeight = static_cast<float>(cam->GetScreenHeight());

            // 화면 크기로 설정
            m_awayUI->SetSize({ 1206, 200 });
            m_awayUI->SetPosition({ 362.8f, -440.0f });
            m_awayUI->SetPivot({ 0.5f, 0.5f });
            m_awayUI->SetAnchor({ 1.0f, 0.0f });
        }

        //////////
        // 징병 _ 견습냥이 ( 이미지 & 텍스트 )
        m_trainerCatButton = CreateUIObject<Button>(L"UI_TrainerCatButton");
        //m_trainerCatButton->SetTextureName("ART_RecruitCat03");
        m_trainerCatButton->SetSize({ 128, 139 });
        m_trainerCatButton->SetPosition({ -111, -405 });
        m_trainerCatButton->SetAnchor({ 1.0f, 0.0f });
        m_trainerCatButton->SetScale({ 1.f, 1.f });

        // 견습냥이 버튼 클릭 시 실행될 이벤트
        m_trainerCatButton->AddOnClick("On Click", [this]()
            {
                if (isOpenOption) { return; }

                m_playerArmy.RecruitUnits(JDGameSystem::SordierType::Novice);
                UpdateAttackPowerText();
                std::cout << "Player Power: " << m_playerArmy.CalculateTotalPower() << std::endl;
            });

        // 견습냥이 버튼 마우스를 올리면 실행될 이벤트
        m_trainerCatButton->AddOnEnter("Highlight On", [this]()
            {
                if (isOpenOption) { return; }

                m_trainerCatButton->SetTextureColor(D2D1::ColorF(D2D1::ColorF::White, 0.8f));
            });

        // 견습냥이 버튼 마우스가 벗어나면 실행될 이벤트
        m_trainerCatButton->AddOnExit("Highlight Off", [this]()
            {
                if (isOpenOption) { return; }

                m_trainerCatButton->SetTextureColor(D2D1::ColorF(D2D1::ColorF::White, 1.f));
            });

        m_trainerCatName = CreateUIObject<Text>(L"UI_TrainerCatNameText");
        m_trainerCatName->SetText(L"견습냥이");
        m_trainerCatName->SetTextFormatName("Sebang_Bold_30");
        m_trainerCatName->SetColor(D2D1::ColorF(0x2F3315));
        m_trainerCatName->SetSize({ 300, 100 });
        m_trainerCatName->SetPosition({ -100, -503 });

        // 견습냥이 코스트 _ 01 ( Info & 이미지 & 텍스트 )
        m_trainerCatCostInfo = CreateUIObject<Text>(L"UI_TrainerCatCostInfoText");
        m_trainerCatCostInfo->SetText(L"코스트 :");
        m_trainerCatCostInfo->SetTextFormatName("Sebang_21");
        m_trainerCatCostInfo->SetColor(D2D1::ColorF(0x2F3315));
        m_trainerCatCostInfo->SetSize({ 90, 100 });
        m_trainerCatCostInfo->SetPosition({ 5, -380 });

        m_trainerCatCostImage01 = CreateUIObject<Image>(L"UI_TrainerCatCostImage01");
        m_trainerCatCostImage01->SetTextureName("ART_CostFood01");
        m_trainerCatCostImage01->SetSizeToOriginal();
        m_trainerCatCostImage01->SetScale({ 0.5f, 0.5f });
        m_trainerCatCostImage01->SetPosition({ 48, -361 });
        m_trainerCatCostImage01->SetAnchor({ 1.0f, 0.0f });

        m_trainerCatCostText01 = CreateUIObject<Text>(L"UI_TrainerCatCostText01");
        m_trainerCatCostText01->SetText(L"x200");
        m_trainerCatCostText01->SetTextFormatName("Sebang_16");
        m_trainerCatCostText01->SetColor(D2D1::ColorF(0x2F3315));
        m_trainerCatCostText01->SetSize({ 50, 100 });
        m_trainerCatCostText01->SetPosition({ 117, -376 });

        // 견습냥이 코스트 _ 02 ( 이미지 & 텍스트 )
        m_trainerCatCostImage02 = CreateUIObject<Image>(L"UI_TrainerCatCostImage02");
        m_trainerCatCostImage02->SetTextureName("ART_CostWood01");
        m_trainerCatCostImage02->SetSizeToOriginal();
        m_trainerCatCostImage02->SetScale({ 0.5f, 0.5f });
        m_trainerCatCostImage02->SetPosition({ 48, -404 });
        m_trainerCatCostImage02->SetAnchor({ 1.0f, 0.0f });

        m_trainerCatCostText02 = CreateUIObject<Text>(L"UI_TrainerCatCostText02");
        m_trainerCatCostText02->SetText(L"x50");
        m_trainerCatCostText02->SetTextFormatName("Sebang_16");
        m_trainerCatCostText02->SetColor(D2D1::ColorF(0x2F3315));
        m_trainerCatCostText02->SetSize({ 50, 100 });
        m_trainerCatCostText02->SetPosition({ 117, -422 });

        // 견습냥이 모집병력 Info & Text
        m_trainerCatRecruitInfo = CreateUIObject<Text>(L"UI_TrainerCatRecruitInfoText");
        m_trainerCatRecruitInfo->SetText(L"모집병력 :");
        m_trainerCatRecruitInfo->SetTextFormatName("Sebang_21");
        m_trainerCatRecruitInfo->SetColor(D2D1::ColorF(0x2F3315));
        m_trainerCatRecruitInfo->SetSize({ 90, 100 });
        m_trainerCatRecruitInfo->SetPosition({ 14, -462 });

        m_trainerCatRecruitText = CreateUIObject<Text>(L"UI_TrainerCatRecruitText");
        m_trainerCatRecruitText->SetText(L"10");
        m_trainerCatRecruitText->SetTextFormatName("Sebang_16");
        m_trainerCatRecruitText->SetColor(D2D1::ColorF(0x2F3315));
        m_trainerCatRecruitText->SetSize({ 50, 100 });
        m_trainerCatRecruitText->SetPosition({ 96, -462 });

        // 견습냥이 전투력 Info & Text
        m_trainerCatPowerInfo = CreateUIObject<Text>(L"UI_TrainerCatPowerInfoText");
        m_trainerCatPowerInfo->SetText(L"전투력 :");
        m_trainerCatPowerInfo->SetTextFormatName("Sebang_21");
        m_trainerCatPowerInfo->SetColor(D2D1::ColorF(0x2F3315));
        m_trainerCatPowerInfo->SetSize({ 90, 100 });
        m_trainerCatPowerInfo->SetPosition({ 5, -500 });

        m_trainerCatPowerText = CreateUIObject<Text>(L"UI_TrainerCatPowerText");
        m_trainerCatPowerText->SetText(L"10");
        m_trainerCatPowerText->SetTextFormatName("Sebang_16");
        m_trainerCatPowerText->SetColor(D2D1::ColorF(0x2F3315));
        m_trainerCatPowerText->SetSize({ 50, 100 });
        m_trainerCatPowerText->SetPosition({ 96, -500 });

        //////////
        // 징병 _ 숙련냥이 이미지
        m_expertCatButton = CreateUIObject<Button>(L"UI_ExpertCatButton");
        //m_expertCatButton->SetTextureName("ART_RecruitCat03");
        m_expertCatButton->SetSize({ 128, 139 });
        m_expertCatButton->SetPosition({ 245, -405 });
        m_expertCatButton->SetAnchor({ 1.0f, 0.0f });
        m_expertCatButton->SetScale({ 1.f, 1.f });

        // 숙련냥이 버튼 클릭 시 실행될 이벤트
        m_expertCatButton->AddOnClick("On Click", [this]()
            {
                if (isOpenOption) { return; }

                m_playerArmy.RecruitUnits(JDGameSystem::SordierType::Expert);
                UpdateAttackPowerText();
                std::cout << "Player Power: " << m_playerArmy.CalculateTotalPower() << std::endl;
            });

        // 숙련냥이 버튼 마우스를 올리면 실행될 이벤트
        m_expertCatButton->AddOnEnter("Highlight On", [this]()
            {
                if (isOpenOption) { return; }
                
                m_expertCatButton->SetTextureColor(D2D1::ColorF(D2D1::ColorF::White, 0.8f));
            });

        // 숙련냥이 버튼 마우스가 벗어나면 실행될 이벤트
        m_expertCatButton->AddOnExit("Highlight Off", [this]()
            {
                if (isOpenOption) { return; }

                m_expertCatButton->SetTextureColor(D2D1::ColorF(D2D1::ColorF::White, 1.f));
            });

        m_expertCatName = CreateUIObject<Text>(L"UI_ExpertCatNameText");
        m_expertCatName->SetText(L"숙련냥이");
        m_expertCatName->SetTextFormatName("Sebang_Bold_30");
        m_expertCatName->SetColor(D2D1::ColorF(0x2F3315));
        m_expertCatName->SetSize({ 300, 100 });
        m_expertCatName->SetPosition({ 258, -503 });

        // 숙련냥이 코스트 _ 01 ( Info & 이미지 & 텍스트 )
        m_expertCatCostInfo = CreateUIObject<Text>(L"UI_ExpertCatCostInfoText");
        m_expertCatCostInfo->SetText(L"코스트 :");
        m_expertCatCostInfo->SetTextFormatName("Sebang_21");
        m_expertCatCostInfo->SetColor(D2D1::ColorF(0x2F3315));
        m_expertCatCostInfo->SetSize({ 90, 100 });
        m_expertCatCostInfo->SetPosition({ 362, -380 });

        m_expertCatCostImage01 = CreateUIObject<Image>(L"UI_ExpertCatCostImage01");
        m_expertCatCostImage01->SetTextureName("ART_CostFood01");
        m_expertCatCostImage01->SetSizeToOriginal();
        m_expertCatCostImage01->SetScale({ 0.5f, 0.5f });
        m_expertCatCostImage01->SetPosition({ 406, -361 });
        m_expertCatCostImage01->SetAnchor({ 1.0f, 0.0f });

        m_expertCatCostText01 = CreateUIObject<Text>(L"UI_ExpertCatCostText01");
        m_expertCatCostText01->SetText(L"x100");
        m_expertCatCostText01->SetTextFormatName("Sebang_16");
        m_expertCatCostText01->SetColor(D2D1::ColorF(0x2F3315));
        m_expertCatCostText01->SetSize({ 50, 100 });
        m_expertCatCostText01->SetPosition({ 476, -376 });

        // 숙련냥이 코스트 _ 02 ( 이미지 & 텍스트 )
        m_expertCatCostImage02 = CreateUIObject<Image>(L"UI_ExpertCatCostImage02");
        m_expertCatCostImage02->SetTextureName("ART_CostMineral01");
        m_expertCatCostImage02->SetSizeToOriginal();
        m_expertCatCostImage02->SetScale({ 0.5f, 0.5f });
        m_expertCatCostImage02->SetPosition({ 406, -404 });
        m_expertCatCostImage02->SetAnchor({ 1.0f, 0.0f });

        m_expertCatCostText02 = CreateUIObject<Text>(L"UI_ExpertCatCostText02");
        m_expertCatCostText02->SetText(L"x50");
        m_expertCatCostText02->SetTextFormatName("Sebang_16");
        m_expertCatCostText02->SetColor(D2D1::ColorF(0x2F3315));
        m_expertCatCostText02->SetSize({ 50, 100 });
        m_expertCatCostText02->SetPosition({ 476, -422 });

        // 숙련냥이 모집병력 Info & Text
        m_expertCatRecruitInfo = CreateUIObject<Text>(L"UI_ExpertCatRecruitInfoText");
        m_expertCatRecruitInfo->SetText(L"모집병력 :");
        m_expertCatRecruitInfo->SetTextFormatName("Sebang_21");
        m_expertCatRecruitInfo->SetColor(D2D1::ColorF(0x2F3315));
        m_expertCatRecruitInfo->SetSize({ 90, 100 });
        m_expertCatRecruitInfo->SetPosition({ 372, -462 });

        m_expertCatRecruitText = CreateUIObject<Text>(L"UI_ExpertCatRecruitText");
        m_expertCatRecruitText->SetText(L"10");
        m_expertCatRecruitText->SetTextFormatName("Sebang_16");
        m_expertCatRecruitText->SetColor(D2D1::ColorF(0x2F3315));
        m_expertCatRecruitText->SetSize({ 50, 100 });
        m_expertCatRecruitText->SetPosition({ 455, -462 });

        // 숙련냥이 전투력 Info & Text
        m_expertCatPowerInfo = CreateUIObject<Text>(L"UI_ExpertCatPowerInfoText");
        m_expertCatPowerInfo->SetText(L"전투력 :");
        m_expertCatPowerInfo->SetTextFormatName("Sebang_21");
        m_expertCatPowerInfo->SetColor(D2D1::ColorF(0x2F3315));
        m_expertCatPowerInfo->SetSize({ 90, 100 });
        m_expertCatPowerInfo->SetPosition({ 362, -500 });

        m_expertCatPowerText = CreateUIObject<Text>(L"UI_ExpertCatPowerText");
        m_expertCatPowerText->SetText(L"10");
        m_expertCatPowerText->SetTextFormatName("Sebang_16");
        m_expertCatPowerText->SetColor(D2D1::ColorF(0x2F3315));
        m_expertCatPowerText->SetSize({ 50, 100 });
        m_expertCatPowerText->SetPosition({ 455, -500 });

        //////////
        // 원정 보내기 Info
        m_awayInfo = CreateUIObject<Text>(L"UI_AwayInfoText");
        m_awayInfo->SetText(L"원정 보내기");
        m_awayInfo->SetTextFormatName("Sebang_Bold_24");
        m_awayInfo->SetColor(D2D1::ColorF(0x2F3315));
        m_awayInfo->SetSize({ 300, 100 });
        m_awayInfo->SetPosition({ 760, -379 });

        // 원정 초급 Button
        m_awayBeginner = CreateUIObject<Button>(L"UI_AwayBeginnerButton");
        m_awayBeginner->SetTextureName("Art_Expedition_Button(Lv01)_Level");
        m_awayBeginner->SetText(L"");
        m_awayBeginner->SetSize({ 183.f, 42.f });
        m_awayBeginner->SetPosition({ 760, -425 });

        // 원정 초급 버튼 클릭하면 실행될 이벤트
        m_awayBeginner->AddOnClick("On Click", [this]()
            {
                if (isOpenOption) { return; }

                JDGameSystem::ExpeditionSystem::Instance().RollBonusType(); // 랜덤 보상 종류 결정.
                int idx = JDGameSystem::ExpeditionSystem::Instance().GetRandomResourceIndex();
                std::wstring bonusName;
                switch (idx) {
                case 0: bonusName = L"식량"; break;
                case 1: bonusName = L"목재"; break;
                case 2: bonusName = L"광물"; break;
                default: bonusName = L"알 수 없음"; break;
                }

                auto& expeditionSystem = JDGameSystem::ExpeditionSystem::Instance();
                auto cost = expeditionSystem.GetExpeditionInfo(JDGameSystem::ExpeditionGrade::Beginner).m_cost;
                int point = expeditionSystem.GetExpeditionInfo(JDGameSystem::ExpeditionGrade::Beginner).m_point;
                int rate = expeditionSystem.GetExpeditionInfo(JDGameSystem::ExpeditionGrade::Beginner).m_successRate;

                m_awayCostText01->SetText(std::to_wstring(cost.m_food));
                m_awayCostText02->SetText(std::to_wstring(cost.m_wood));
                m_awayCostText03->SetText(std::to_wstring(cost.m_mineral));

                m_awayAwardText01->SetText(std::to_wstring(point) + L" 원정포인트");
                m_awayAwardText02->SetText(std::to_wstring(rate) + L"% 확률로 " + bonusName + L" 추가 보상");

                ShowAwayPopup();

                m_awayPopupInfo->SetText(L"초급 원정");
                if (JDGameSystem::ExpeditionSystem::Instance().ReachedTheGoal()) {
                    m_awayButton->SetTextureName("Art_Expedition_Button(Cannot)");
                }
                else {
                    m_awayButton->SetTextureName("Art_Expedition_Button(Lv01)");
                }

                m_awayStar01->SetTextureName("Art_Expedition_Level_Empty");
                m_awayStar02->SetTextureName("Art_Expedition_Level_Empty");
                m_awayStar03->SetTextureName("Art_Expedition_Level_Full");
            });

        // 원정 초급 버튼 마우스를 올리면 실행될 이벤트
        m_awayBeginner->AddOnEnter("Highlight On", [this]()
            {
                if (isOpenOption) { return; }

                m_awayBeginner->SetTextureName("Art_Expedition_Button(Lv01)_Level_mouseover");
                m_awayBeginner->SetSize({ 171.f, 30.f });
            });

        // 원정 초급 버튼 마우스가 벗어나면 실행될 이벤트
        m_awayBeginner->AddOnExit("Highlight Off", [this]()
            {
                if (isOpenOption) { return; }

                m_awayBeginner->SetTextureName("Art_Expedition_Button(Lv01)_Level");
                m_awayBeginner->SetSize({ 183.f, 42.f });
            });

        // 중급 Button
        // 원정 중급 Button
        m_awayIntermediate = CreateUIObject<Button>(L"UI_AwayIntermediate");
        m_awayIntermediate->SetTextureName("Art_Expedition_Button(Lv02)_Level");
        m_awayIntermediate->SetText(L"");
        m_awayIntermediate->SetSize({ 183.f, 42.f });
        m_awayIntermediate->SetPosition({ 760, -463 });

        // 원정 중급 버튼 클릭하면 실행될 이벤트
        m_awayIntermediate->AddOnClick("On Click", [this]()
            {
                if (isOpenOption) { return; }

                JDGameSystem::ExpeditionSystem::Instance().RollBonusType(); // 랜덤 보상 종류 결정.
                int idx = JDGameSystem::ExpeditionSystem::Instance().GetRandomResourceIndex();
                std::wstring bonusName;
                switch (idx) {
                case 0: bonusName = L"식량"; break;
                case 1: bonusName = L"목재"; break;
                case 2: bonusName = L"광물"; break;
                default: bonusName = L"알 수 없음"; break;
                }

                auto& expeditionSystem = JDGameSystem::ExpeditionSystem::Instance();
                auto cost = expeditionSystem.GetExpeditionInfo(JDGameSystem::ExpeditionGrade::Intermediate).m_cost;
                int point = expeditionSystem.GetExpeditionInfo(JDGameSystem::ExpeditionGrade::Intermediate).m_point;
                int rate = expeditionSystem.GetExpeditionInfo(JDGameSystem::ExpeditionGrade::Intermediate).m_successRate;

                m_awayCostText01->SetText(std::to_wstring(cost.m_food));
                m_awayCostText02->SetText(std::to_wstring(cost.m_wood));
                m_awayCostText03->SetText(std::to_wstring(cost.m_mineral));

                m_awayAwardText01->SetText(std::to_wstring(point) + L" 원정포인트");
                m_awayAwardText02->SetText(std::to_wstring(rate) + L"% 확률로 " + bonusName + L" 추가 보상");

                ShowAwayPopup();
                m_awayPopupInfo->SetText(L"중급 원정");
                if (JDGameSystem::ExpeditionSystem::Instance().ReachedTheGoal()) {
                    m_awayButton->SetTextureName("Art_Expedition_Button(Cannot)");
                }
                else {
                    m_awayButton->SetTextureName("Art_Expedition_Button(Lv02)");
                }

                m_awayStar01->SetTextureName("Art_Expedition_Level_Empty");
                m_awayStar02->SetTextureName("Art_Expedition_Level_Full");
                m_awayStar03->SetTextureName("Art_Expedition_Level_Full");
            });

        // 원정 중급 버튼 마우스를 올리면 실행될 이벤트
        m_awayIntermediate->AddOnEnter("Highlight On", [this]()
            {
                if (isOpenOption) { return; }

                m_awayIntermediate->SetTextureName("Art_Expedition_Button(Lv02)_Level_mouseover");
                m_awayIntermediate->SetSize({ 171.f, 30.f });
            });

        // 원정 중급 버튼 마우스가 벗어나면 실행될 이벤트
        m_awayIntermediate->AddOnExit("Highlight Off", [this]()
            {
                if (isOpenOption) { return; }

                m_awayIntermediate->SetTextureName("Art_Expedition_Button(Lv02)_Level");
                m_awayIntermediate->SetSize({ 183.f, 42.f });
            });

        // 상급 Button
        m_awayAdvanced = CreateUIObject<Button>(L"UI_AwayAdvanced");
        m_awayAdvanced->SetTextureName("Art_Expedition_Button(Lv03)_Level");
        m_awayAdvanced->SetText(L"");
        m_awayAdvanced->SetSize({ 183.f, 42.f });
        m_awayAdvanced->SetPosition({ 760, -500 });

        // 원정 상급 버튼 클릭하면 실행될 이벤트
        m_awayAdvanced->AddOnClick("On Click", [this]()
            {
                if (isOpenOption) { return; }

                JDGameSystem::ExpeditionSystem::Instance().RollBonusType(); // 랜덤 보상 종류 결정.
                int idx = JDGameSystem::ExpeditionSystem::Instance().GetRandomResourceIndex();
                std::wstring bonusName;
                switch (idx) {
                case 0: bonusName = L"식량"; break;
                case 1: bonusName = L"목재"; break;
                case 2: bonusName = L"광물"; break;
                default: bonusName = L"알 수 없음"; break;
                }

                auto& expeditionSystem = JDGameSystem::ExpeditionSystem::Instance();
                auto cost = expeditionSystem.GetExpeditionInfo(JDGameSystem::ExpeditionGrade::Higher).m_cost;
                int point = expeditionSystem.GetExpeditionInfo(JDGameSystem::ExpeditionGrade::Higher).m_point;
                int rate = expeditionSystem.GetExpeditionInfo(JDGameSystem::ExpeditionGrade::Higher).m_successRate;


                m_awayCostText01->SetText(std::to_wstring(cost.m_food));
                m_awayCostText02->SetText(std::to_wstring(cost.m_wood));
                m_awayCostText03->SetText(std::to_wstring(cost.m_mineral));

                m_awayAwardText01->SetText(std::to_wstring(point) + L" 원정포인트");
                m_awayAwardText02->SetText(std::to_wstring(rate) + L"% 확률로 " + bonusName + L" 추가 보상");

                ShowAwayPopup();
                m_awayPopupInfo->SetText(L"상급 원정");
                if (JDGameSystem::ExpeditionSystem::Instance().ReachedTheGoal()) {
                    m_awayButton->SetTextureName("Art_Expedition_Button(Cannot)");
                }
                else {
                    m_awayButton->SetTextureName("Art_Expedition_Button(Lv03)");
                }

                m_awayStar01->SetTextureName("Art_Expedition_Level_Full");
                m_awayStar02->SetTextureName("Art_Expedition_Level_Full");
                m_awayStar03->SetTextureName("Art_Expedition_Level_Full");
            });

        // 원정 상급 버튼 마우스를 올리면 실행될 이벤트
        m_awayAdvanced->AddOnEnter("Highlight On", [this]()
            {
                if (isOpenOption) { return; }

                m_awayAdvanced->SetTextureName("Art_Expedition_Button(Lv03)_Level_mouseover");
                m_awayAdvanced->SetSize({ 171.f, 30.f });
                 
            });

        // 원정 상급 버튼 마우스가 벗어나면 실행될 이벤트
        m_awayAdvanced->AddOnExit("Highlight Off", [this]()
            {
                if (isOpenOption) { return; }

                m_awayAdvanced->SetTextureName("Art_Expedition_Button(Lv03)_Level");
                m_awayAdvanced->SetSize({ 183.f, 42.f });

            });

        //////////
        // 원정 정보 팝업 Image
        m_awayPopupUI = CreateUIObject<Image>(L"UI_AwayPopupUI");
        m_awayPopupUI->SetTextureName("ART_Information_Box_Expedition");
        m_awayPopupUI->SetSize({ 406, 277 });
        m_awayPopupUI->SetPosition({ 757, -113 });
        m_awayPopupUI->SetAnchor({ 1.0f, 0.0f });

        // 원정 정보 Info
        m_awayPopupInfo = CreateUIObject<Text>(L"UI_AwayPopupInfoText");
        m_awayPopupInfo->SetText(L"초급 원정");
        m_awayPopupInfo->SetTextFormatName("Sebang_Bold_30");
        m_awayPopupInfo->SetColor(D2D1::ColorF(0x2F3315));
        m_awayPopupInfo->SetSize({ 300, 100 });
        m_awayPopupInfo->SetPosition({ 766, -6 });

        // 원정 비용 Info
        m_awayCostInfo = CreateUIObject<Text>(L"UI_AwayCostInfoText");
        m_awayCostInfo->SetText(L"비용");
        m_awayCostInfo->SetTextFormatName("Sebang_Bold_22");
        m_awayCostInfo->SetColor(D2D1::ColorF(0x2F3315));
        m_awayCostInfo->SetSize({ 300, 100 });
        m_awayCostInfo->SetPosition({ 610.0f, -65.0f });

        // 원정 코스트 _ 01 ( 이미지 & 텍스트 )
        m_awayCostImage01 = CreateUIObject<Image>(L"UI_AwayCostImage01");
        m_awayCostImage01->SetTextureName("ART_CostFood01");
        m_awayCostImage01->SetSizeToOriginal();
        m_awayCostImage01->SetScale({ 0.5f, 0.5f });
        m_awayCostImage01->SetPosition({ 650, -48 });
        m_awayCostImage01->SetAnchor({ 1.0f, 0.0f });

        m_awayCostText01 = CreateUIObject<Text>(L"UI_AwayCostText01");
        m_awayCostText01->SetText(L"150");
        m_awayCostText01->SetTextFormatName("Sebang_16");
        m_awayCostText01->SetColor(D2D1::ColorF(0x2F3315));
        m_awayCostText01->SetSize({ 50, 100 });
        m_awayCostText01->SetPosition({ 724, -70 });

        // 원정 코스트 _ 02 ( 이미지 & 텍스트 )
        m_awayCostImage02 = CreateUIObject<Image>(L"UI_AwayCostImage02");
        m_awayCostImage02->SetTextureName("ART_CostWood01");
        m_awayCostImage02->SetSizeToOriginal();
        m_awayCostImage02->SetScale({ 0.5f, 0.5f });
        m_awayCostImage02->SetPosition({ 750, -48 });
        m_awayCostImage02->SetAnchor({ 1.0f, 0.0f });

        m_awayCostText02 = CreateUIObject<Text>(L"UI_AwayCostText02");
        m_awayCostText02->SetText(L"150");
        m_awayCostText02->SetTextFormatName("Sebang_16");
        m_awayCostText02->SetColor(D2D1::ColorF(0x2F3315));
        m_awayCostText02->SetSize({ 50, 100 });
        m_awayCostText02->SetPosition({ 820, -70 });

        // 원정 코스트 _ 03 ( 이미지 & 텍스트 )
        m_awayCostImage03 = CreateUIObject<Image>(L"UI_AwayCostImage03");
        m_awayCostImage03->SetTextureName("ART_CostMineral01");
        m_awayCostImage03->SetSizeToOriginal();
        m_awayCostImage03->SetScale({ 0.5f, 0.5f });
        m_awayCostImage03->SetPosition({ 852, -48 });
        m_awayCostImage03->SetAnchor({ 1.0f, 0.0f });

        m_awayCostText03 = CreateUIObject<Text>(L"UI_AwayCostText03");
        m_awayCostText03->SetText(L"150");
        m_awayCostText03->SetTextFormatName("Sebang_16");
        m_awayCostText03->SetColor(D2D1::ColorF(0x2F3315));
        m_awayCostText03->SetSize({ 50, 100 });
        m_awayCostText03->SetPosition({ 926, -70 });

        /////
        // 보상 정보 Info
        m_awayAwardInfo = CreateUIObject<Text>(L"UI_AwayAwardInfo");
        m_awayAwardInfo->SetText(L"보상");
        m_awayAwardInfo->SetTextFormatName("Sebang_Bold_22");
        m_awayAwardInfo->SetColor(D2D1::ColorF(0x2F3315));
        m_awayAwardInfo->SetSize({ 300, 100 });
        m_awayAwardInfo->SetPosition({ 610.0f, -140.0f });

        // 보상 정보 Text
        m_awayAwardText01 = CreateUIObject<Text>(L"UI_AwayAwardText01");
        m_awayAwardText01->SetText(L"N 원정포인트");
        m_awayAwardText01->SetTextFormatName("Sebang_20");
        m_awayAwardText01->SetColor(D2D1::ColorF(0x2F3315));
        m_awayAwardText01->SetSize({ 300, 50 });
        m_awayAwardText01->SetPosition({ 708, -127 });

        // 추가 보상 정보 Text
        m_awayAwardText02 = CreateUIObject<Text>(L"UI_AwayAwardText02");
        m_awayAwardText02->SetText(L"N% 확률로 ㅇㅇㅇ 추가 보상");
        m_awayAwardText02->SetTextFormatName("Sebang_20");
        m_awayAwardText02->SetColor(D2D1::ColorF(0x2F3315));
        m_awayAwardText02->SetSize({ 300, 50 });
        m_awayAwardText02->SetPosition({ 760, -162 });

        /////
        // 병력 보내기 Button
        m_awayButton = CreateUIObject<Button>(L"UI_AwayButton");
        m_awayButton->SetTextureName("Art_Expedition_Button(Lv01)");
        m_awayButton->SetText(L"");
        m_awayButton->SetSize({ 135.5f, 47.0f });
        m_awayButton->SetPosition({ 768, -216 });

        // 병력 보내기 버튼 클릭하면 실행될 이벤트
        m_awayButton->AddOnClick("On Click", [this]()
            {
                if (isOpenOption) { return; }

                // TODO 초중급 나눠야하는데 우선 이미지로 판별할까?
                if (m_awayButton->GetTextureName() == "Art_Expedition_Button(Lv01)_mouseover")
                {
                    auto& sys = JDGameSystem::ExpeditionSystem::Instance();
                    if (sys.SendExpedition(JDGameSystem::ExpeditionGrade::Beginner)) CreateExpedition();
                }

                else if (m_awayButton->GetTextureName() == "Art_Expedition_Button(Lv02)_mouseover")
                {
                    auto& sys = JDGameSystem::ExpeditionSystem::Instance();
                    if (sys.SendExpedition(JDGameSystem::ExpeditionGrade::Intermediate)) CreateExpedition();
                }

                else if (m_awayButton->GetTextureName() == "Art_Expedition_Button(Lv03)_mouseover")
                {
                    auto& sys = JDGameSystem::ExpeditionSystem::Instance();
                    if (sys.SendExpedition(JDGameSystem::ExpeditionGrade::Higher)) CreateExpedition();
                }
            });

        // 병력 보내기 버튼 마우스를 올리면 실행될 이벤트
        m_awayButton->AddOnEnter("Highlight On", [this]()
            {
                if (isOpenOption) { return; }

                // TODO 초중급 나눠야하는데 우선 이미지로 판별할까?
                if (m_awayButton->GetTextureName() == "Art_Expedition_Button(Lv01)")
                {
                    m_awayButton->SetTextureName("Art_Expedition_Button(Lv01)_mouseover");
                }

                else if (m_awayButton->GetTextureName() == "Art_Expedition_Button(Lv02)")
                {
                    m_awayButton->SetTextureName("Art_Expedition_Button(Lv02)_mouseover");
                }

                else if (m_awayButton->GetTextureName() == "Art_Expedition_Button(Lv03)")
                {
                    m_awayButton->SetTextureName("Art_Expedition_Button(Lv03)_mouseover");
                }
                else if (m_awayButton->GetTextureName() == "Art_Expedition_Button(Cannot)")
                {
                    m_awayButton->SetTextureName("Art_Expedition_Button(Cannot)_mouseover");
                }

                m_awayButton->SetSize({ 123.5f, 35.0f });
            });

        // 병력 보내기 버튼 마우스가 벗어나면 실행될 이벤트
        m_awayButton->AddOnExit("Highlight Off", [this]()
            {
                if (isOpenOption) { return; }

                // TODO 초중급 나눠야하는데 우선 이미지로 판별할까?
                if (m_awayButton->GetTextureName() == "Art_Expedition_Button(Lv01)_mouseover")
                {
                    m_awayButton->SetTextureName("Art_Expedition_Button(Lv01)");
                }

                else if (m_awayButton->GetTextureName() == "Art_Expedition_Button(Lv02)_mouseover")
                {
                    m_awayButton->SetTextureName("Art_Expedition_Button(Lv02)");
                }

                else if (m_awayButton->GetTextureName() == "Art_Expedition_Button(Lv03)_mouseover")
                {
                    m_awayButton->SetTextureName("Art_Expedition_Button(Lv03)");
                }
                else if (m_awayButton->GetTextureName() == "Art_Expedition_Button(Cannot)_mouseover")
                {
                    m_awayButton->SetTextureName("Art_Expedition_Button(Cannot)");
                }
                m_awayButton->SetSize({ 135.5f, 47.0f });

            });

        // 병력 보내기 팝업 별 이미지
        m_awayStar01 = CreateUIObject<Image>(L"UI_AwayStar01");
        m_awayStar01->SetTextureName("Art_Expedition_Level_Full");
        m_awayStar01->SetSizeToOriginal();
        m_awayStar01->SetScale({ 0.4f, 0.4f });
        m_awayStar01->SetPosition({ 835, 20 });
        m_awayStar01->SetAnchor({ 1.0f, 0.0f });

        m_awayStar02 = CreateUIObject<Image>(L"UI_AwayStar02");
        m_awayStar02->SetTextureName("Art_Expedition_Level_Full");
        m_awayStar02->SetSizeToOriginal();
        m_awayStar02->SetScale({ 0.4f, 0.4f });
        m_awayStar02->SetPosition({ 875, 20 });
        m_awayStar02->SetAnchor({ 1.0f, 0.0f });

        m_awayStar03 = CreateUIObject<Image>(L"UI_AwayStar03");
        m_awayStar03->SetTextureName("Art_Expedition_Level_Full");
        m_awayStar03->SetSizeToOriginal();
        m_awayStar03->SetScale({ 0.4f, 0.4f });
        m_awayStar03->SetPosition({ 915, 20 });
        m_awayStar03->SetAnchor({ 1.0f, 0.0f });

#pragma endregion

        //// 4) 그리드 클릭하면 나오는 메뉴
        //m_Menu = CreateUIObject<Image>(L"MenuBackground");

        ////auto& uptr = m_uiObjects.back();
        ////UIObject* rawPtr = uptr.get();
        ////m_TutorialUIObjects.push_back(rawPtr);

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
        //        //m_TutorialUIObjects.push_back(m_uiObjects.back().get());
        //        m_gridCreateButtons.push_back(btn);

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
        //        //m_TutorialUIObjects.push_back(m_uiObjects.back().get());
        //        m_gridSettingButtons.push_back(btn);

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
        // TODO : 함수명 맞추기
        InitGridCreateMenu();
        InitGridSettingMenu();
        InitAwayMenu();

        //// TODO: 테스트
        //// ShowUpgradeMenu();
        //ShowAwayMenu();


        // [DEBUG] 메뉴 버튼 상태 체크
        std::cout << "===== UI Button Debug =====" << std::endl;

        // 1) m_uiObjects 전체 개수
        std::cout << "[DEBUG] m_uiObjects.size(): " << m_uiObjects.size() << std::endl;

        // 2) m_menuButtons 상태 확인
        std::cout << "[DEBUG] m_menuButtons.size(): " << m_menuButtons.size() << std::endl;
        for (size_t i = 0; i < m_menuButtons.size(); ++i)
        {
            auto* btn = m_menuButtons[i];
            bool foundInUIObjects = false;
            for (auto& uptr : m_uiObjects)
            {
                if (uptr.get() == btn)
                {
                    foundInUIObjects = true;
                    break;
                }
            }

            std::wcout << L"  [" << i << L"] "
                << btn->GetName()
                << L" | In m_uiObjects: " << (foundInUIObjects ? "YES" : "NO")
                << L" | Active: " << (btn->IsActive() ? "true" : "false")
                << std::endl;
        }

        // 3) m_emptyButtons 상태 확인
        //std::cout << "[DEBUG] m_emptyButtons.size(): " << m_gridCreateButtons.size() << std::endl;
        for (size_t i = 0; i < m_gridCreateButtons.size(); ++i)
        {
            auto* btn = m_gridCreateButtons[i];
            bool foundInUIObjects = false;
            for (auto& uptr : m_uiObjects)
            {
                if (uptr.get() == btn)
                {
                    foundInUIObjects = true;
                    break;
                }
            }

            std::wcout << L"  [" << i << L"] "
                << btn->GetName()
                << L" | In m_uiObjects: " << (foundInUIObjects ? "YES" : "NO")
                << L" | Active: " << (btn->IsActive() ? "true" : "false")
                << std::endl;
        }
    }

    void GameScene::GameSpeedButtonUpdate()
    {
        // 0 배속일 떄
        if (GetTimeScale() == 0.0f)
        {
            m_stopButton->SetTextureName("ART_Pause01_ing");
            m_playButton->SetTextureName("ART_Play01");
            m_speedButton->SetTextureName("ART_QuickPlay01");
        }
        else if (GetTimeScale() == 1.0f)
        {
            m_stopButton->SetTextureName("ART_Pause01");
            m_playButton->SetTextureName("ART_Play01_ing");
            m_speedButton->SetTextureName("ART_QuickPlay01");
        }
        else if (GetTimeScale() == 2.0f)
        {
            m_stopButton->SetTextureName("ART_Pause01");
            m_playButton->SetTextureName("ART_Play01");
            m_speedButton->SetTextureName("ART_QuickPlay01_ing");
        }
        else if (GetTimeScale() == 4.0f)
        {
            m_stopButton->SetTextureName("ART_Pause01");
            m_playButton->SetTextureName("ART_Play01");
            m_speedButton->SetTextureName("ART_QuickPlay02_ing");
        }
        else
        {
            m_stopButton->SetTextureName("ART_Pause01");
            m_playButton->SetTextureName("ART_Play01");
            m_speedButton->SetTextureName("ART_QuickPlay03_ing");
        }
    }

    void GameScene::DateUIUpdate()
    {
        m_yearText->SetText(std::to_wstring(m_date.year));
        m_monthText->SetText(std::to_wstring(m_date.month));
        m_dayText->SetText(std::to_wstring(m_date.day));
    }
  
    void GameScene::SpawnNextWaveIndicator(int wavePower)
    {
        const Vector2F startPosIcon{ 570.0f, 455.0f };
        const Vector2F startPosText{ 570.0f, 442.0f };

        auto* icon = CreateUIObject<Image>(L"UI_NextWaveIcon");
        icon->SetTextureName("ART_Monster001"); 
        icon->SetSize({ 50, 45 });
        //icon->SetAnchor({ 1.0f, 0.0f });
        icon->SetPosition(startPosIcon);
        icon->SetActive(true);

        auto* text = CreateUIObject<Text>(L"UI_NextWavePowerText");
        text->SetText(std::to_wstring(wavePower));
        text->SetColor(D2D1::ColorF(0x69512C));
        text->SetSize({ 60, 45 });
        //text->SetAnchor({ 1.0f, 0.0f });
        text->SetTextFormatName("Sebang_Bold_14");
        text->SetPosition(startPosText);
        text->SetActive(true);

        NextWaveIndicator ind;
        ind.waveIcon = icon;
        ind.powerText = text;
        ind.stepsMoved = 0;
        ind.srcDay = WaveManager::Instance().GetCurrDay();
        ind.lastMovedDay = ind.srcDay;

        m_nextWaveIndicators.push_back(ind);
    }
  
    void GameScene::AdvanceNextWaveIndicators()
    {
        constexpr float kPerDay = -5.76f; // 하루 이동량
        constexpr int   kMaxDays = 80;    // 총 수명 80일

        const int today = WaveManager::Instance().GetCurrDay();

        auto eraseBeg = std::remove_if(m_nextWaveIndicators.begin(), m_nextWaveIndicators.end(),
            [&](NextWaveIndicator& ind)
            {
                if (!ind.waveIcon || !ind.powerText) return true;

                // 스킵된 날짜가 있으면 그만큼 한 번에 반영
                int daysToApply = today - ind.lastMovedDay;
                if (daysToApply > 0) {
                    float deltaX = kPerDay * static_cast<float>(daysToApply);

                    if (auto* trI = ind.waveIcon->GetComponent<JDComponent::D2DTM::RectTransform>()) {
                        auto p = trI->GetPosition();
                        trI->SetPosition({ p.x + deltaX, p.y });
                    }
                    if (auto* trT = ind.powerText->GetComponent<JDComponent::D2DTM::RectTransform>()) {
                        auto p = trT->GetPosition();
                        trT->SetPosition({ p.x + deltaX, p.y });
                    }

                    ind.lastMovedDay = today;
                }

                // 생성일로부터 80일 지나면 제거
                if (today - ind.srcDay >= kMaxDays) {
                    ind.waveIcon->SetActive(false);
                    ind.powerText->SetActive(false);
                    SceneBase::DestroyObject(ind.waveIcon);
                    SceneBase::DestroyObject(ind.powerText);
                    return true;
                }
                return false;
            });

        m_nextWaveIndicators.erase(eraseBeg, m_nextWaveIndicators.end());
    }

    bool GameScene::CheckEnding()
    {
        // 남은 웨이브가 없는지.
        bool noWavesLeft = (WaveManager::Instance().GetRemainingWaves() == 0);

        // 현재 전투 오브젝트가 없는지.
        bool noBattle = (m_battleObject == nullptr);

        // 공격 중인 적이 없는지.
        bool noAttackers = m_attackers.empty();

        // 살아있는 적 유닛이 전혀 없는지.
        bool noEnemiesAlive = true;
        for (auto* enemy : m_enemies) {
            if (enemy && enemy->IsActive()) {
                noEnemiesAlive = false;
                break;
            }
        }

        // 모든 조건이 충족되면 엔딩.
        return (noWavesLeft && noBattle && noAttackers && noEnemiesAlive);
    }

    bool GameScene::ResolveGameEnding()
    {
        if (m_isEnding) return true; // 엔딩을 한번 실행했으면 끝.

        if (m_wallHealth == 0) {
            StartEnding(EndingType::Bad);
            std::cout << "[GameScene] 배드엔딩이다!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
            m_isEnding = true;
            return true;
        }
        else if (CheckEnding()) {
            if (JDGameSystem::ExpeditionSystem::Instance().ReachedTheGoal()) {
                StartEnding(EndingType::Good);
                std::cout << "[GameScene] 굿엔딩이다!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
            }
            else {
                StartEnding(EndingType::None);
                std::cout << "[GameScene] 엔딩이다!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
            }
            m_isEnding = true;

            return true;
        }
        return false;
    }

    void GameScene::RegisterUnit(JDGlobal::Base::GameTag tag, JDGameObject::GameObjectBase* obj)
    {
        if (!obj) return;
        if (tag == JDGlobal::Base::GameTag::Player) m_players.push_back(obj);
        else if (tag == JDGlobal::Base::GameTag::Enemy) m_enemies.push_back(obj);
    }

    void GameScene::UnregisterUnit(JDGameObject::GameObjectBase* obj)
    {
        auto erasePtr = [&](auto& vec) {
            vec.erase(std::remove(vec.begin(), vec.end(), obj), vec.end());
            };
        erasePtr(m_players);
        erasePtr(m_enemies);
    }

    JDGameObject::GameObjectBase* GameScene::AttachObject(JDGameObject::GameObjectBase* follower, JDGameObject::GameObjectBase* host, Vector2F offset)
    {
        if (!follower || !host) return nullptr;
        m_attachments.push_back({ follower, host, offset });
        // 처음 위치 동기화
        if (auto* ht = host->GetComponent<Transform>())
            if (auto* ft = follower->GetComponent<Transform>())
                ft->SetPosition(ht->GetPosition() + offset);
        return follower;
    }

    void GameScene::UpdateAttachments(float dt)
    {
        for (auto it = m_attachments.begin(); it != m_attachments.end();) {
            auto& a = *it;
            if (!a.host || !a.follower || !a.host->IsActive() || !a.follower->IsActive()) {
                it = m_attachments.erase(it);
                continue;
            }
            auto* ht = a.host->GetComponent<Transform>();
            auto* ft = a.follower->GetComponent<Transform>();
            if (ht && ft) ft->SetPosition(ht->GetPosition() + a.offset);
            ++it;
        }
    }

    void GameScene::DetachByHost(JDGameObject::GameObjectBase* host)
    {
        // m_attachments.erase(std::remove_if(m_attachments.begin(), m_attachments.end(),
        //     [&](const Attachment& a) { return a.host == host; }), m_attachments.end());

        std::vector<JDGameObject::GameObjectBase*> toDestroy;
        m_attachments.erase(
            std::remove_if(m_attachments.begin(), m_attachments.end(),
                [&](const Attachment& a) {
                    if (a.host == host) {
                        if (a.follower) toDestroy.push_back(a.follower);
                        return true;
                    }
                    return false;
                }), m_attachments.end());

        for (auto* f : toDestroy) {
            SceneBase::DestroyObject(f);
        }
    }

    JDGameObject::GameObjectBase* GameScene::CreateHeadMarker(JDGameObject::GameObjectBase* host)
    {
        if (!host) return nullptr;

        using namespace JDGameObject;
        using namespace JDComponent;

        auto* imageGO = CreateGameObject<GameObject>(L"MarkImage");
        imageGO->SetTag(JDGlobal::Base::GameTag::None);
        if (host->GetTag() == JDGlobal::Base::GameTag::Enemy) {
            imageGO->AddComponent<JDComponent::TextureRenderer>("ART_Enemy_Mark", RenderLayerInfo{ SortingLayer::BackGround, 5 });
        }
        else if (host->GetTag() == JDGlobal::Base::GameTag::Player) {
            imageGO->AddComponent<JDComponent::TextureRenderer>("ART_Player_Mark", RenderLayerInfo{ SortingLayer::BackGround, 5 });
        }

        auto imgRender = imageGO->GetComponent<JDComponent::TextureRenderer>();
        imgRender->SetSize({ 100, 50 });

        auto* txtGO = CreateGameObject<GameObject>(L"PowerText");
        txtGO->SetTag(JDGlobal::Base::GameTag::None);
        auto* tr = txtGO->AddComponent<JDComponent::TextRenderer>(
            L"",                                 // 초기 텍스트
            D2D1::SizeF(100.f, 20.f),             // 레이아웃 크기
            RenderLayerInfo{ SortingLayer::BackGround, 10 } // 레이어 정보
        );

        auto txtRender = txtGO->GetComponent<JDComponent::TextRenderer>();
        txtRender->SetText(L"");
        txtRender->SetTextFormatName("Sebang_20");
        //txtRender->SetColor(D2D1::ColorF(0xD6BD94));

        Vector2F offset{ 0, -30.0f }; // 기본값
        if (auto* col = host->GetComponent<JDComponent::BoxCollider>())
        {
            auto half = col->GetHalfSize();
            offset.y = -half.y; 
        }
        AttachObject(imageGO, host, offset + Vector2F{ 0, 30.0f });
        AttachObject(txtGO, host, offset + Vector2F{ 0, 15.0f });

        return txtGO; 
    }
  
    void GameScene::UpdateAwayPointUI()
    {
        int expeditionPoints = JDGameSystem::ExpeditionSystem::Instance().GetExpeditionPoints();
        int goalPoints = JDGameSystem::ExpeditionSystem::Instance().GetGoalPoints();

        m_awayCurValue->SetText(std::to_wstring(expeditionPoints));
        m_awayMaxValue->SetText(std::to_wstring(goalPoints));
    }

    void GameScene::CreateBarrackUI()
    {
        //////////
        // (게임오브젝트로 UI 만들어야함)
        // 
        // 병영 텍스쳐
        m_barrackUI = CreateGameObject<GameObject>(L"GameUI_BarrackHP");
        m_barrackUI->SetTag(JDGameObject::GameObject::Tag::UI);
        m_barrackUI->AddComponent<Editor_Clickable>();
        m_barrackUI->AddComponent<JDComponent::TextureRenderer>("ART_Barracks_HP", RenderLayerInfo{SortingLayer::BackGround, 5});

        m_barrackUI->GetComponent<JDComponent::Transform>()->SetPosition({ -211, 276 });
        m_barrackUI->GetComponent<JDComponent::TextureRenderer>()->SetSize({166, 80});

        //////////
        // 병영 체력 Cur / Div / Max 텍스트
        // 현재 체력 Cur
        m_barrackCurText = CreateGameObject<GameObject>(L"GameUI_BarrackCurHP");
        m_barrackCurText->SetTag(JDGameObject::GameObject::Tag::UI);
        m_barrackCurText->AddComponent<Editor_Clickable>();
        m_barrackCurText->AddComponent<JDComponent::TextRenderer>(
            L"200",
            D2D1::SizeF(100.f, 100.f),
            RenderLayerInfo{ SortingLayer::BackGround, 10 }
        );
        
        m_barrackCurText->GetComponent<JDComponent::Transform>()->SetPosition({ -200, 273 });

        auto barrackCurTextRender = m_barrackCurText->GetComponent<JDComponent::TextRenderer>();
        barrackCurTextRender->SetText(L"200");
        barrackCurTextRender->SetTextFormatName("Sebang_12");
        barrackCurTextRender->SetColor(D2D1::ColorF(0xD6BD94));

        // 구분선 Div
        m_barrackDivText = CreateGameObject<GameObject>(L"GameUI_BarrackDivHP");
        m_barrackDivText->SetTag(JDGameObject::GameObject::Tag::UI);
        m_barrackDivText->AddComponent<Editor_Clickable>();
        m_barrackDivText->AddComponent<JDComponent::TextRenderer>(
            L"/",
            D2D1::SizeF(100.f, 100.f),
            RenderLayerInfo{ SortingLayer::BackGround, 10 }
        );
        
        m_barrackDivText->GetComponent<JDComponent::Transform>()->SetPosition({ -185, 273 });

        auto barrackDivTextRender = m_barrackDivText->GetComponent<JDComponent::TextRenderer>();
        barrackDivTextRender->SetText(L"/");
        barrackDivTextRender->SetTextFormatName("Sebang_12");
        barrackDivTextRender->SetColor(D2D1::ColorF(0xD6BD94));

        // 최대체력 Max
        m_barrackMaxText = CreateGameObject<GameObject>(L"GameUI_BarrackMaxHP");
        m_barrackMaxText->SetTag(JDGameObject::GameObject::Tag::UI);
        m_barrackMaxText->AddComponent<Editor_Clickable>();
        m_barrackMaxText->AddComponent<JDComponent::TextRenderer>(
            L"999",
            D2D1::SizeF(100.f, 100.f),
            RenderLayerInfo{ SortingLayer::BackGround, 10 }
        );
        
        m_barrackMaxText->GetComponent<JDComponent::Transform>()->SetPosition({ -170, 273 });

        auto barrackMaxTextRender = m_barrackMaxText->GetComponent<JDComponent::TextRenderer>();
        barrackMaxTextRender->SetText(L"999");
        barrackMaxTextRender->SetTextFormatName("Sebang_12");
        barrackMaxTextRender->SetColor(D2D1::ColorF(0xD6BD94));

        //////////
        // 전투력 Text
        m_attackPowerText = CreateGameObject<GameObject>(L"GameUI_AttackPower");
        m_attackPowerText->SetTag(JDGameObject::GameObject::Tag::UI);
        m_attackPowerText->AddComponent<Editor_Clickable>();
        m_attackPowerText->AddComponent<JDComponent::TextRenderer>(
            L"1234567",
            D2D1::SizeF(100.f, 100.f),
            RenderLayerInfo{ SortingLayer::BackGround, 10 }
        );

        m_attackPowerText->GetComponent<JDComponent::Transform>()->SetPosition({ -184, 253 });

        auto attackPowerTextRender = m_attackPowerText->GetComponent<JDComponent::TextRenderer>();
        attackPowerTextRender->SetText(L"2731245");
        attackPowerTextRender->SetTextFormatName("Sebang_12");
        attackPowerTextRender->SetColor(D2D1::ColorF(0xD6BD94));
    }

    void GameScene::UpdateBarrackUI()
    {
        auto barrackTextRender = m_barrackCurText->GetComponent<JDComponent::TextRenderer>();
        if (!barrackTextRender) return;
        barrackTextRender->SetText(std::to_wstring(m_wallHealth));
    }

    void GameScene::UpdateAttackPowerText()
    {
        auto attackPowerTextRender = m_attackPowerText->GetComponent<JDComponent::TextRenderer>();
        if (!attackPowerTextRender) return;
        attackPowerTextRender->SetText(std::to_wstring(m_playerArmy.CalculateTotalPower()));
    }

    void GameScene::ShowClickedBarrackObj()
    {
        if (!m_barracksObject) return;

        auto btr = m_barracksObject->GetComponent<TextureRenderer>();
        if (!btr) return;

        if (m_isBarracksSelected)
        {
            btr->SetTextureName("ART_Barracks02_mouse_over");
        }
        else
        {
            btr->SetTextureName("ART_Barracks02_mouseout_");
        }
    }

    void GameScene::SetBarracksSelected(bool on)
    {
        m_isBarracksSelected = on;
        if (on) {
            isAway = true;
            ShowAwayMenu();
        }
        else {
            isAway = false;
            CloseAwayMenu();
        }
    }

    void GameScene::ChangeAwayCatImage()
    {

        CatType catType = ResourceSystem::Instance().GetNation();
        std::cout << static_cast<int>(catType) << std::endl;

        switch (catType) {
        case CatType::Felis:
            m_trainerCatButton->SetTextureName("Norway_nov_Frame 1");
            m_expertCatButton->SetTextureName("Norway_ex_Frame 1");
            break;
        case CatType::Navi:
            m_trainerCatButton->SetTextureName("Scottish_nov_Frame 1");
            m_expertCatButton->SetTextureName("Scottish_ex_Frame 1");
            break;
        case CatType::Kone:
            m_trainerCatButton->SetTextureName("Russ_nov_Frame 1");
            m_expertCatButton->SetTextureName("Russ_ex_Frame 1");
            break;
        default:
            break;
        } 

        m_trainerCatButton->SetSizeToOriginal();
        m_expertCatButton->SetSizeToOriginal();
    }

    void GameScene::CreateOptionUI()
    {
        //////////////////////////////////////////////////////////////////////////////////

        // 옵션창
        m_optionUI = CreateUIObject<Image>(L"Option_Popup");
        m_optionUI->SetTextureName("ART_BG01_OPACITY");
        m_optionUI->SetColor(D2D1::ColorF(D2D1::ColorF::White, 0.65f));
        m_optionUI->SetActive(false);

        auto cam = D2DRenderer::Instance().GetCamera();
        if (cam)
        {
            float screenWidth = static_cast<float>(cam->GetScreenWidth());
            float screenHeight = static_cast<float>(cam->GetScreenHeight());

            // 화면 크기로 설정
            m_optionUI->SetSize(Vector2F{ screenWidth, screenHeight });
            m_optionUI->SetPosition({ 0.0f,0.0f });
            m_optionUI->SetPivot({ 0.5f, 0.5f });
        }

        //////////////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////////////
        // 볼륨 조절 창
        //////////////////////////////////////////////////////////////////////////////////
        m_optionVolume = CreateUIObject<Image>(L"Option_Volume");
        m_optionVolume->SetTextureName("ART_Volume01_mousedown");
        m_optionVolume->SetActive(false);
        m_optionVolume->SetSize({ 1920, 1080 });
        m_optionVolume->SetPosition({ 0.0f,0.0f });
        m_optionVolume->SetPivot({ 0.5f, 0.5f });

        //////////////////////////////////////////////////////////////////////////////////
        // BGM 볼륨 조절 슬라이더
        //////////////////////////////////////////////////////////////////////////////////

        // 마스터 사운드
        m_masterSlider = CreateUIObject<Slider>(L"Slider_MasterVolume");
        m_masterSlider->Assemble(this); // 씬의 도움을 받아 슬라이더 자식들을 조립합니다.

        m_masterSlider->SetBackgroundImage("ART_VolumeSlider01");
        m_masterSlider->SetFillImage("ART_VolumeSlider01");
        m_masterSlider->SetHandleImage("ART_VolumeCat01");

        m_masterSlider->SetSize({ 600, 8 });
        m_masterSlider->SetRootSize({ 600, 60 });
        m_masterSlider->SetPosition({ 150, 170 });
        m_masterSlider->SetFillImagePosition({ -300, 0 });

        m_masterSlider->SetHandleImageSize({ 57.f, 52.f });

        m_masterSlider->SetValue(AudioManager::Instance().GetMusicVolume());
        m_masterSlider->AddOnValueChanged("Set Master Volume", [](float newValue) {
            AudioManager::Instance().SetMasterVolume(newValue);
            });

        m_masterSlider->SetActiveSlider(false);

        // 배경음
        m_bgmSlider = CreateUIObject<Slider>(L"Slider_BGM");
        m_bgmSlider->Assemble(this); // 씬의 도움을 받아 슬라이더 자식들을 조립합니다.

        m_bgmSlider->SetBackgroundImage("ART_VolumeSlider02");
        m_bgmSlider->SetFillImage("ART_VolumeSlider02");
        m_bgmSlider->SetHandleImage("ART_VolumeCat02");

        m_bgmSlider->SetSize({ 600, 8 });
        m_bgmSlider->SetRootSize({ 600, 60 });
        m_bgmSlider->SetPosition({ 150, 28 });
        m_bgmSlider->SetFillImagePosition({ -300, 0 });

        m_bgmSlider->SetHandleImageSize({ 39.5f, 35.f });

        m_bgmSlider->SetValue(AudioManager::Instance().GetMusicVolume());
        m_bgmSlider->AddOnValueChanged("Set BGM Volume", [](float newValue) {
            AudioManager::Instance().SetMusicVolume(newValue);
            });

        m_bgmSlider->SetActiveSlider(false);

        //////////////////////////////////////////////////////////////////////////////////

        // 효과음
        m_sfxSlider = CreateUIObject<Slider>(L"Slider_SFX");
        m_sfxSlider->Assemble(this); // 씬의 도움을 받아 슬라이더 자식들을 조립합니다.

        m_sfxSlider->SetBackgroundImage("ART_VolumeSlider02");
        m_sfxSlider->SetFillImage("ART_VolumeSlider02");
        m_sfxSlider->SetHandleImage("ART_VolumeCat02");

        m_sfxSlider->SetSize({ 600, 8 });
        m_sfxSlider->SetRootSize({ 600, 60 });
        m_sfxSlider->SetPosition({ 150, -42 });
        m_sfxSlider->SetFillImagePosition({ -300, 0 });

        m_sfxSlider->SetHandleImageSize({ 39.5f, 35.f });

        m_sfxSlider->SetValue(AudioManager::Instance().GetSFXVolume());
        m_sfxSlider->AddOnValueChanged("Set SFX Volume", [](float newValue) {
            AudioManager::Instance().SetSFXVolume(newValue);
            });

        m_sfxSlider->SetActiveSlider(false);

        //////////////////////////////////////////////////////////////////////////////////

        // 컨트롤 조절 창
        m_optionControl = CreateUIObject<Image>(L"Option_Control");
        m_optionControl->SetTextureName("ART_Control01_mousedown");
        m_optionControl->SetSize({ 1920, 1080 });
        m_optionControl->SetPosition({ 0.0f, 0.0f });
        m_optionControl->SetPivot({ 0.5f, 0.5f });
        m_optionControl->SetActive(false);

        m_stopKeyText = CreateUIObject<Text>(L"StopKey_Text");
        m_stopKeyText->SetText(L"1");
        m_stopKeyText->SetSize({ 115.5f, 23.f });
        m_stopKeyText->SetPosition({ 113, 184 });
        m_stopKeyText->SetTextFormatName("Sebang_Bold_32");
        m_stopKeyText->SetActive(false);

        m_playKeyText = CreateUIObject<Text>(L"PlayKey_Text");
        m_playKeyText->SetText(L"2");
        m_playKeyText->SetSize({ 115.5f, 23.f });
        m_playKeyText->SetPosition({ 113, 100 });
        m_playKeyText->SetTextFormatName("Sebang_Bold_32");
        m_playKeyText->SetActive(false);

        m_speedKeyText = CreateUIObject<Text>(L"SpeedKey_Text");
        m_speedKeyText->SetText(L"3");
        m_speedKeyText->SetSize({ 115.5f, 23.f });
        m_speedKeyText->SetPosition({ 113, 16 });
        m_speedKeyText->SetTextFormatName("Sebang_Bold_32");
        m_speedKeyText->SetActive(false);

        //////////////////////////////////////////////////////////////////////////////////

        // 크레딧 보기 창
        m_optionCredit = CreateUIObject<Image>(L"Option_Credit");
        m_optionCredit->SetTextureName("ART_Credit01_mousedown");
        m_optionCredit->SetSize({ 1920, 1080 });
        m_optionCredit->SetPosition({ 0.0f,0.0f });
        m_optionCredit->SetPivot({ 0.5f, 0.5f });
        m_optionCredit->SetActive(false);

        //////////////////////////////////////////////////////////////////////////////////

        // 옵션 선택 버튼 ( 볼륨 )
        m_selectVolumeDummyText = CreateUIObject<Text>(L"VolumeDummy_Text");
        m_selectVolumeDummyText->SetText(L"VOLUME");
        m_selectVolumeDummyText->SetSize({ 115.5f, 23.f });
        m_selectVolumeDummyText->SetPosition({ -497.f, 244.f });
        m_selectVolumeDummyText->SetTextFormatName("Sebang_Bold_24");
        m_selectVolumeDummyText->SetColor(D2D1::ColorF(0xD6BD94));
        m_selectVolumeDummyText->SetActive(false);

        m_selectVolume = CreateUIObject<Button>(L"SelectVolume_Button");
        m_selectVolume->SetTextureName("Test");
        m_selectVolume->SetTextureColor(D2D1::ColorF(D2D1::ColorF::White, 0.f));
        m_selectVolume->SetText(L"");
        m_selectVolume->SetSize({ 180, 70 });
        m_selectVolume->SetPosition({ -497.4f, 244.1f });
        m_selectVolume->SetActive(false);

        m_selectVolume->AddOnClick("OpenVolumeUI", [this]() {
            AudioManager::Instance().PlaySFX("SFX_Button_Click", &sfxChannel);
            m_optionVolume->SetActive(true);
            m_optionControl->SetActive(false);
            m_optionCredit->SetActive(false);

            m_selectVolumeDummyText->SetColor(D2D1::ColorF(0xD6BD94));
            m_selectControlDummyText->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
            m_selectCreditDummyText->SetColor(D2D1::ColorF(D2D1::ColorF::Black));

            m_masterSlider->SetActiveSlider(true);
            m_bgmSlider->SetActiveSlider(true);
            m_sfxSlider->SetActiveSlider(true);

            m_stopKeyText->SetActive(false);
            m_playKeyText->SetActive(false);
            m_speedKeyText->SetActive(false);
            });

        // 옵션 선택 버튼 ( 컨트롤 )
        m_selectControlDummyText = CreateUIObject<Text>(L"ControlDummy_Text");
        m_selectControlDummyText->SetText(L"CONTROL");
        m_selectControlDummyText->SetSize({ 115.5f, 23.f });
        m_selectControlDummyText->SetPosition({ -497.f, 172.f });
        m_selectControlDummyText->SetTextFormatName("Sebang_Bold_24");
        m_selectControlDummyText->SetActive(false);

        m_selectControl = CreateUIObject<Button>(L"SelectControl_Button");
        m_selectControl->SetTextureName("Test");
        m_selectControl->SetTextureColor(D2D1::ColorF(D2D1::ColorF::White, 0.f));
        m_selectControl->SetText(L"");
        m_selectControl->SetSize({ 180, 70 });
        m_selectControl->SetPosition({ -497.4f, 172.7f });
        m_selectControl->SetActive(false);

        m_selectControl->AddOnClick("OpenControlUI", [this]() {
            AudioManager::Instance().PlaySFX("SFX_Button_Click", &sfxChannel);
            m_optionVolume->SetActive(false);
            m_optionControl->SetActive(true);
            m_optionCredit->SetActive(false);

            m_selectVolumeDummyText->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
            m_selectControlDummyText->SetColor(D2D1::ColorF(0xD6BD94));
            m_selectCreditDummyText->SetColor(D2D1::ColorF(D2D1::ColorF::Black));

            m_masterSlider->SetActiveSlider(false);
            m_bgmSlider->SetActiveSlider(false);
            m_sfxSlider->SetActiveSlider(false);

            m_stopKeyText->SetActive(true);
            m_playKeyText->SetActive(true);
            m_speedKeyText->SetActive(true);
            });

        // 옵션 선택 버튼 ( 크레딧 )
        m_selectCreditDummyText = CreateUIObject<Text>(L"CreditDummy_Text");
        m_selectCreditDummyText->SetText(L"CREDITS");
        m_selectCreditDummyText->SetSize({ 115.5f, 23.f });
        m_selectCreditDummyText->SetPosition({ -497.f, 101.f });
        m_selectCreditDummyText->SetTextFormatName("Sebang_Bold_24");
        m_selectCreditDummyText->SetActive(false);

        m_selectCredit = CreateUIObject<Button>(L"SelectCredit_Button");
        m_selectCredit->SetTextureName("Test");
        m_selectCredit->SetTextureColor(D2D1::ColorF(D2D1::ColorF::White, 0.f));
        m_selectCredit->SetText(L"");
        m_selectCredit->SetSize({ 180, 70 });
        m_selectCredit->SetPosition({ -497.4f, 101.6f });
        m_selectCredit->SetActive(false);

        m_selectCredit->AddOnClick("OpenCreditUI", [this]() {
            AudioManager::Instance().PlaySFX("SFX_Button_Click", &sfxChannel);
            m_optionVolume->SetActive(false);
            m_optionControl->SetActive(false);
            m_optionCredit->SetActive(true);

            m_selectVolumeDummyText->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
            m_selectControlDummyText->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
            m_selectCreditDummyText->SetColor(D2D1::ColorF(0xD6BD94));

            m_masterSlider->SetActiveSlider(false);
            m_bgmSlider->SetActiveSlider(false);
            m_sfxSlider->SetActiveSlider(false);

            m_stopKeyText->SetActive(false);
            m_playKeyText->SetActive(false);
            m_speedKeyText->SetActive(false);
            });

        //////////////////////////////////////////////////////////////////////////////////

        /////
        // 옵션 닫기 버튼 ( Close )
        m_closeOption = CreateUIObject<Button>(L"CloseSetting_Button");
        m_closeOption->SetTextureName("ART_Back01_mouseout");
        m_closeOption->SetText(L"");
        m_closeOption->SetSize({ 66, 60 });
        m_closeOption->SetPosition({ -550, 340 });
        m_closeOption->SetActive(false);

        // 1. OnClick: 클릭하면 실행될 이벤트
        m_closeOption->AddOnClick("Close Option", [this]() {
            AudioManager::Instance().PlaySFX("SFX_Button_Click", &sfxChannel);
            CloseOptionUI();
            });

        // 2. OnEnter: 마우스를 올리면 텍스처 변경
        m_closeOption->AddOnEnter("Highlight On", [this]() {
            // 텍스트 변경
            m_closeOption->SetTextureName("ART_Back01_mouseover");
            });

        // 3. OnExit: 마우스가 벗어나면 원래 텍스처로 복원
        m_closeOption->AddOnExit("Highlight Off", [this]() {
            // 텍스트 변경
            m_closeOption->SetTextureName("ART_Back01_mouseout");
            });

        /////
        // 타이틀 이동 버튼
        m_backToTitle = CreateUIObject<Button>(L"BackToTitle_Button");
        m_backToTitle->SetTextureName("Art_Button_Mouseover02");
        m_backToTitle->SetText(L"BACK TO TITLE");
        m_backToTitle->SetTextFormatName("Sebang_Bold_20");
        m_backToTitle->SetTextColor(D2D1::ColorF(0x576023));
        m_backToTitle->SetSize({ 174, 54 });
        m_backToTitle->SetPosition({ -505, -300 });
        m_backToTitle->SetTextureColor(D2D1::ColorF(D2D1::ColorF::White, 0.0f));
        m_backToTitle->SetActive(false);

        // 1. OnClick: 클릭하면 실행될 이벤트
        m_backToTitle->AddOnClick("Back To Title", [this]() {
            // SceneManager를 이용해 다음 씬으로 넘어갑니다.
            SceneManager::Instance().ChangeScene("TitleScene");
            AudioManager::Instance().PlaySFX("SFX_Button_Click", &sfxChannel);
            });

        // 2. OnEnter: 마우스를 올리면 텍스처 변경
        m_backToTitle->AddOnEnter("Highlight On", [this]() {

            });

        // 3. OnExit: 마우스가 벗어나면 원래 텍스처로 복원
        m_backToTitle->AddOnExit("Highlight Off", [this]() {

            });

        /////
        // 게임 종료 버튼
        m_quitGame = CreateUIObject<Button>(L"QuitGame_Button");
        m_quitGame->SetTextureName("Art_Button_Mouseover02");
        m_quitGame->SetText(L"QUIT GAME");
        m_quitGame->SetTextFormatName("Sebang_Bold_22");
        m_quitGame->SetTextColor(D2D1::ColorF(0xD6BD94));
        m_quitGame->SetSize({ 174, 54 });
        m_quitGame->SetPosition({ -505, -354 });
        m_quitGame->SetActive(false);

        // 1. OnClick: 클릭하면 실행될 이벤트
        m_quitGame->AddOnClick("Quit Game", [this]() {
            AudioManager::Instance().PlaySFX("SFX_Button_Click", &sfxChannel);
            // 1. 메인 윈도우의 핸들(HWND)을 가져옵니다.
            HWND mainWindowHandle = JDGlobal::Window::WindowSize::Instance().GetHWND();

            // 2. PostMessage를 사용해 WM_CLOSE 메시지를 보냅니다.
            if (mainWindowHandle)
            {
                PostMessage(mainWindowHandle, WM_CLOSE, 0, 0);
            }
            });

        // 2. OnEnter: 마우스를 올리면 텍스처 변경
        m_quitGame->AddOnEnter("Highlight On", [this]() {

            });

        // 3. OnExit: 마우스가 벗어나면 원래 텍스처로 복원
        m_quitGame->AddOnExit("Highlight Off", [this]() {

            });

        //////////////////////////////////////////////////////////////////////////////////
    }

    void GameScene::ShowOptionUI()
    {
        isOpenOption = true;
        m_prevGameSpeed = GetTimeScale();
        SetTimeScale(0.0f);

        if (m_optionUI) {
            m_optionUI->SetActive(true);
            m_optionVolume->SetActive(true);

            m_selectVolume->SetActive(true);
            m_selectControl->SetActive(true);
            m_selectCredit->SetActive(true);

            m_closeOption->SetActive(true);
            m_backToTitle->SetActive(true);
            m_quitGame->SetActive(true);

            m_selectVolumeDummyText->SetActive(true);
            m_selectControlDummyText->SetActive(true);
            m_selectCreditDummyText->SetActive(true);

            m_selectVolumeDummyText->SetColor(D2D1::ColorF(0xD6BD94));
            m_selectControlDummyText->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
            m_selectCreditDummyText->SetColor(D2D1::ColorF(D2D1::ColorF::Black));

            m_masterSlider->SetActiveSlider(true);
            m_bgmSlider->SetActiveSlider(true);
            m_sfxSlider->SetActiveSlider(true);
        }
    }
    void GameScene::CloseOptionUI()
    {
        isOpenOption = false;
        SetTimeScale(m_prevGameSpeed);

        if (m_optionUI) {
            m_optionUI->SetActive(false);
            m_optionVolume->SetActive(false);
            m_optionControl->SetActive(false);
            m_optionCredit->SetActive(false);

            m_selectVolume->SetActive(false);
            m_selectControl->SetActive(false);
            m_selectCredit->SetActive(false);

            m_closeOption->SetActive(false);
            m_backToTitle->SetActive(false);
            m_quitGame->SetActive(false);

            m_selectVolumeDummyText->SetActive(false);
            m_selectControlDummyText->SetActive(false);
            m_selectCreditDummyText->SetActive(false);

            m_masterSlider->SetActiveSlider(false);
            m_bgmSlider->SetActiveSlider(false);
            m_sfxSlider->SetActiveSlider(false);

            m_stopKeyText->SetActive(false);
            m_playKeyText->SetActive(false);
            m_speedKeyText->SetActive(false);
        }
    }
    void GameScene::CreateFillter()
    {
        // 2) 필터
        m_fillter = CreateUIObject<Image>(L"Fillter_Image");
        m_fillter->SetTextureName("ART_BattleMap01_Example");
        m_fillter->SetColor(D2D1::ColorF(D2D1::ColorF::White, 0.65f));

        auto cam = D2DRenderer::Instance().GetCamera();
        if (cam)
        {
            float screenWidth = static_cast<float>(cam->GetScreenWidth());
            float screenHeight = static_cast<float>(cam->GetScreenHeight());

            // 화면 크기로 설정
            m_fillter->SetSize(Vector2F{ screenWidth, screenHeight });
            m_fillter->SetPosition({ 0.0f,0.0f });
            m_fillter->SetPivot({ 0.5f, 0.5f });
        }
    }

    void GameScene::FinalizeGameScene()
    {
        CloseOptionUI();
        CloseGridCreateMenu();
        CloseGridCreateInfo();
        CloseGridSettingMenu();
        CloseCatInfo();
        CloseAwayMenu();
        CloseAwayPopup();

        //FMOD::Channel* bgmChannel = nullptr;
        //FMOD::Channel* sfxChannel = nullptr;

        //std::unique_ptr<ParticleSystem> m_lightParticles;
        //Vector2F                        m_emitterPos;

        //CameraFader  m_fader;

        m_isOpen.clear();
        m_showedDays.clear();
        // m_camera;

        //JDGameSystem::ArmySystem m_playerArmy; // 병영.
        //JDGameSystem::ArmySystem m_enemyArmy; // 해당 날짜에 등장하는 적.

        //JDGameSystem::UnitCounts m_playerBattleArmy;
        //JDGameSystem::UnitCounts m_enemyBattleArmy;

        m_playerTotalPower = 0;
        m_enemyTotalPower = 0;
        m_elapsedTime = 0.0f;
        m_btlElapsedTime = 0.0f;

        m_targetEnemy = nullptr;
        m_playerObject = nullptr;
        m_barracksObject = nullptr;
        m_battleObject = nullptr;
        m_expeditionObject = nullptr;

        m_currentWaypointIndex = 0;
        m_waypoints = { Vector2F{ 255.0f, -135.0f },
                        Vector2F{ 755.0f, -115.0f },
                        Vector2F{ 1010.0f, -175.0f } };
        m_attackers.clear();

        m_wallHealth = 999;
        m_isBarracksSelected = false;

        m_date = { 1, 1, 1 };
        m_nextWaveIndicators.clear();

        m_players.clear();
        m_enemies.clear();

        m_attachments.clear();

        m_gridCreateButtons.clear();
        m_gridSettingButtons.clear();
        m_GirdClicked = false;

        m_Menu = nullptr;
        m_menuButtons.clear();
        m_selectedTool = nullptr;
        m_selectedCollider = nullptr;

        ////////////////////////////////////////////////////////////////////////////////

        // [상단] 자원
        m_buttonPop = nullptr;           // 인구 UI
        m_curPopText = nullptr;           // 인구 보유량
        m_maxPopText = nullptr;           // 인구 최대치

        m_buttonFood = nullptr;         // 음식 UI
        m_curFoodText = nullptr;          // 음식 보유량
        m_resFoodText = nullptr;          // 음식 재생량

        m_buttonWood = nullptr;         // 목재 UI
        m_curWoodText = nullptr;          // 목재 보유량
        m_resWoodText = nullptr;          // 목재 재생량

        m_buttonMineral = nullptr;      // 광물 UI
        m_curMineralText = nullptr;       // 광물 보유량
        m_resMineralText = nullptr;       // 광물 재생량

        ////////////////////////////////////////////////////////////////////////////////

        // [상단] 몬스터 웨이브
        m_monsterWaveBackground = nullptr;

        ////////////////////////////////////////////////////////////////////////////////

        // [상단] 날짜 패널
        m_datePanel = nullptr;
        m_yearText = nullptr;
        m_monthText = nullptr;
        m_dayText = nullptr;

        m_stopButton = nullptr;         // 일시정지 버튼
        m_playButton = nullptr;         // 재생 버튼
        m_speedButton = nullptr;        // 배속 버튼
        m_optionButton = nullptr;       // 옵션 버튼

        ////////////////////////////////////////////////////////////////////////////////

        // 원정 포인트 UI
        m_away = nullptr;
        m_awayCurValue = nullptr;
        m_awayDivText = nullptr;          // "/" 이렇게 구분하는 텍스트
        m_awayMaxValue = nullptr;

        // 병영 Info UI
        m_barrackUI = nullptr;
        m_barrackCurText = nullptr;
        m_barrackDivText = nullptr;
        m_barrackMaxText = nullptr;
        m_attackPowerText = nullptr;

        ////////////////////////////////////////////////////////////////////////////////

        // 그리드와 상호작용 중 인지 확인하기 위한 플래그
        isGridBuild = false;
        isGridSetting = false;
        isAway = false;

        // [하단] 건물 상호작용 UI
        m_defaultUI = nullptr;           // 0. 기본 UI
        m_buildUI = nullptr;             // 1. 건설 UI
        m_upgradeUI = nullptr;           // 2. 업그레이드 및 고양이 배치 UI
        m_awayUI = nullptr;              // 3. 징병 및 원정 UI

        ////////////////////////////////////////////////////////////////////////////////

        // 0. 기본 UI

        // 1. 건설 UI ( 주거지, 낚시터, 제재소, 광산, (연구실) )
        m_buildHouse = nullptr;
        m_buildFishingspot = nullptr;
        m_buildRumbermill = nullptr;
        m_buildMine = nullptr;
        // Button* m_buildLab = nullptr;

        // 1. 건설 Info
        m_buildTypeText = nullptr;

        m_costInfoText = nullptr;
        m_costText = nullptr;
        m_costImage = nullptr;

        m_effectInfoText = nullptr;
        m_effectText = nullptr;
        m_effctImage = nullptr;

        ////////////////////////////////////////////////////////////////////////////////

        // 2. 업그레이드 Info
        // 고양이 배치
        m_catTypeText = nullptr;

        m_catCostInfoText = nullptr;
        m_catCostText = nullptr;
        m_catCostImage = nullptr;

        m_catEffectInfoText = nullptr;
        m_catEffectText = nullptr;
        m_catEffctImage = nullptr;

        // 고양이 선택 버튼
        m_naviSetButton = nullptr;
        m_felisSetButton = nullptr;
        m_koneSetButton = nullptr;

        // 업그레이드 건물 정보
        m_builtTypeText = nullptr;

        m_upgradeCostInfoText = nullptr;
        m_upgradeCostText = nullptr;
        m_upgradeCostImage = nullptr;

        m_upgradeEffectInfoText = nullptr;
        m_upgradeEffectText = nullptr;
        m_upgradeEffctImage = nullptr;

        m_downgradeButton = nullptr;
        m_upgradeButton = nullptr;

        ////////////////////////////////////////////////////////////////////////////////

        /////
        // 견습 냥이
        m_trainerCatButton = nullptr;
        m_trainerCatName = nullptr;

        m_trainerCatCostInfo = nullptr;
        m_trainerCatCostImage01 = nullptr;
        m_trainerCatCostText01 = nullptr;
        m_trainerCatCostImage02 = nullptr;
        m_trainerCatCostText02 = nullptr;

        m_trainerCatRecruitInfo = nullptr;
        m_trainerCatRecruitText = nullptr;

        m_trainerCatPowerInfo = nullptr;
        m_trainerCatPowerText = nullptr;

        /////
        // 숙련 냥이
        m_expertCatButton = nullptr;
        m_expertCatName = nullptr;

        m_expertCatCostInfo = nullptr;
        m_expertCatCostImage01 = nullptr;
        m_expertCatCostText01 = nullptr;
        m_expertCatCostImage02 = nullptr;
        m_expertCatCostText02 = nullptr;

        m_expertCatRecruitInfo = nullptr;
        m_expertCatRecruitText = nullptr;

        m_expertCatPowerInfo = nullptr;
        m_expertCatPowerText = nullptr;

        /////
        // 원정 보내기
        m_awayInfo = nullptr;

        m_awayBeginner = nullptr;           // 초급
        m_awayIntermediate = nullptr;       // 중급
        m_awayAdvanced = nullptr;           // 상급

        /////
        // 원정 팝업
        m_awayPopupUI = nullptr;
        m_awayPopupInfo = nullptr;

        m_awayCostInfo = nullptr;
        m_awayCostImage01 = nullptr;
        m_awayCostText01 = nullptr;
        m_awayCostImage02 = nullptr;
        m_awayCostText02 = nullptr;
        m_awayCostImage03 = nullptr;
        m_awayCostText03 = nullptr;

        m_awayAwardInfo = nullptr;
        m_awayAwardText01 = nullptr;
        m_awayAwardText02 = nullptr;

        m_awayButton = nullptr;

        m_awayStar01 = nullptr;
        m_awayStar02 = nullptr;
        m_awayStar03 = nullptr;

        ////////////////////////////////////////////////////////////////////////////////
        // 옵션창

        isOpenOption = false;

        m_optionUI = nullptr;
        m_optionVolume = nullptr;
        m_optionControl = nullptr;
        m_optionCredit = nullptr;

        // 옵션 닫기 버튼
        m_closeOption = nullptr;
        m_backToTitle = nullptr;
        m_quitGame = nullptr;

        // 옵션 선택 실제 버튼
        m_selectVolume = nullptr;
        m_selectControl = nullptr;
        m_selectCredit = nullptr;

        // 옵션 선택 더미 텍스트
        m_selectVolumeDummyText = nullptr;
        m_selectControlDummyText = nullptr;
        m_selectCreditDummyText = nullptr;

        // 볼륨 선택 슬라이더
        m_masterSlider = nullptr;
        m_bgmSlider = nullptr;
        m_sfxSlider = nullptr;

        // 배속 키 텍스트
        m_stopKeyText = nullptr;
        m_playKeyText = nullptr;
        m_speedKeyText = nullptr;

        m_prevGameSpeed = 0.0f;

        ////////////////////////////////////////////////////////////////////////////////

        m_fillter = nullptr;
    }
    
    void GameScene::CreateEndingUI()
    {
        auto cam = D2DRenderer::Instance().GetCamera();

        //m_spear = CreateGameObject<Player>(L"SPEAR");
        //m_spear->GetComponent<Transform>()->SetPosition({ 0.0f, 0.0f });
        ////boxObj3->AddComponent<Editor_Clickable>();
        ////boxObj3->AddComponent<TextureRenderer>("Test", RenderLayerInfo{ SortingLayer::BackGround, 1 });
        //m_spear->AddComponent<AnimationRender>("GrayBird", 0.5, RenderLayerInfo{ SortingLayer::BackGround, 2 });

        /*auto frames = AssetManager::Instance().GetAnimationRender("GrayBird");
        if (frames && !frames->frames.empty()) {
            auto first = frames->frames[0].srcRect;
            float width = first.right - first.left;
            float height = first.bottom - first.top;
            Vector2F halfSize{ width * 0.5f, height * 0.5f };
            birdObj->AddComponent<BoxCollider>(halfSize);
            birdObj->AddComponent<Editor_Clickable>();
        }*/

        {
            m_spear = CreateGameObject<Player>(L"SPEAR");
            auto* tr = m_spear->GetComponent<Transform>();
            tr->SetPosition({ 0.f, 0.f });
            tr->SetScale({ 4.0f, 4.0f });
            
            m_spearAnim = m_spear->AddComponent<AnimationRender>(
                "ART_SpearSprite01", 0.7f, RenderLayerInfo{ SortingLayer::Front, 10 });

            if (m_spearAnim) {
                m_spearAnim->SetClipName("ART_SpearSprite01"); // 내부 클립명이 다르면 거기에 맞게
                m_spearAnim->SetEnabled(false);                // 초기엔 비활성
            }
            m_spear->SetActive(false);                        // 오브젝트도 숨김
        }

        {
            m_retry = CreateUIObject<Image>(L"ART_RETRY02");
            m_retry->SetTextureName("ART_RETRY02");
            m_retry->SetSizeToOriginal();
            Vector2F size = m_retry->GetSize();
            m_retry->SetSize(size / 2);
            m_retry->SetPosition({ 0.0f,0.0f });
            m_retry->SetPivot({ 0.0f, 0.5f });
            m_retry->SetActive(false);
        }

        {
            m_goMenu = CreateUIObject<Image>(L"ART_BACK_TO_MENU02");
            m_goMenu->SetTextureName("ART_BACK_TO_MENU02");
            m_goMenu->SetSizeToOriginal();
            Vector2F size = m_goMenu->GetSize();
            m_goMenu->SetSize(size / 2);
            m_goMenu->SetPosition({ 0.0f,0.0f });
            m_goMenu->SetPivot({ 1.0f, 0.5f });
            m_goMenu->SetActive(false);
        }
        
        {
            m_endingUI = CreateUIObject<Image>(L"ART_NomalEnd01");
            m_endingUI->SetTextureName("ART_NomalEnd01");
            m_endingUI->SetSizeToOriginal();
            Vector2F size = m_endingUI->GetSize();
            m_endingUI->SetSize(size / 2);
            m_endingUI->SetPosition({ 0.0f,0.0f });
            m_endingUI->SetPivot({ 0.5f, 0.5f });
            m_endingUI->SetActive(false);
        }

        {
            m_selecRetry = CreateUIObject<Button>(L"RETRY_BUTTON");
            m_selecRetry->SetTextureName("Art_Opacity");
            m_selecRetry->SetTextureColor(D2D1::ColorF(0xD6BD94, 0.0f));
            m_selecRetry->SetActive(false);
            m_selecRetry->SetSize({ 250, 150 });
            m_selecRetry->SetPosition({ -350, -300 });
            
            // 리트라이 버튼 클릭하면 실행될 이벤트
            m_selecRetry->AddOnClick("Quit Game", [this]()
                {
                    if (isOpenOption) { return; }
                    SceneManager::Instance().ChangeScene("GameScene");
                });


            // 리트라이 버튼 마우스를 올리면 실행될 이벤트
            m_selecRetry->AddOnEnter("Highlight On", [this]()
                {
                    if (isOpenOption) { return; }

                    m_retry->SetTextureName("ART_RETRY01");
                    Vector2F oldtm = m_retry->GetComponent<RectTransform>()->GetPosition();
                    m_retry->SetPosition({ oldtm.x , oldtm.y - 50 });
                });

            // 리트라이 버튼 마우스가 벗어나면 실행될 이벤트
            m_selecRetry->AddOnExit("Highlight Off", [this]()
                {
                    if (isOpenOption) { return; }
                    Vector2F oldtm = m_retry->GetComponent<RectTransform>()->GetPosition();
                    m_retry->SetTextureName("ART_RETRY02");
                    m_retry->SetPosition({ oldtm.x , oldtm.y + 50 });
                });
        }
            
        

        {
            m_selecGoMenu = CreateUIObject<Button>(L"MENU_BUTTON");
            m_selecGoMenu->SetTextureName("Art_Opacity");
            m_selecGoMenu->SetTextureColor(D2D1::ColorF(0xD6BD94, 0.0f));
            m_selecGoMenu->SetActive(false);
            m_selecGoMenu->SetSize({ 250, 150 });
            m_selecGoMenu->SetPosition({ 350, -300 });

            m_selecGoMenu->AddOnClick("Quit Game", [this]()
                {
                    if (isOpenOption) { return; }
                    SceneManager::Instance().ChangeScene("TitleScene");
                });


            // 리트라이 버튼 마우스를 올리면 실행될 이벤트
            m_selecGoMenu->AddOnEnter("Highlight On", [this]()
                {
                    if (isOpenOption) { return; }

                    m_goMenu->SetTextureName("ART_BACK_TO_MENU01");
                    Vector2F oldtm = m_goMenu->GetComponent<RectTransform>()->GetPosition();
                    m_goMenu->SetPosition({ oldtm.x , oldtm.y - 50 });
                });

            // 리트라이 버튼 마우스가 벗어나면 실행될 이벤트
            m_selecGoMenu->AddOnExit("Highlight Off", [this]()
                {
                    if (isOpenOption) { return; }
                    Vector2F oldtm = m_goMenu->GetComponent<RectTransform>()->GetPosition();
                    m_goMenu->SetTextureName("ART_BACK_TO_MENU02");
                    m_goMenu->SetPosition({ oldtm.x , oldtm.y + 50 });
                });
        }
    }

    void GameScene::StartEnding(EndingType type)
    {
        SetTimeScale(1.0f);
        
        m_endingType = type;
        m_endAnimStarted = true;
        m_endAnimDone = false;
        m_endAnimTime = 0.0f;

        // 연출: 배드엔딩은 검은 페이드, 굿엔딩은 흰 페이드(원하면 바꿔도 됨)
        if (type == EndingType::Bad)
            m_fader.FadeIn(0.1f, 0.5f, D2D1::ColorF(0, 0, 0, 1));
        else
            m_fader.FadeIn(0.1f, 0.5f, D2D1::ColorF(1, 1, 1, 1));

        // 시작/타겟 위치(예: 위에서 내려와 중앙 근처에 정지)
        m_endStartPos = { 0.0f, +600.0f };
        m_endTargetPos = { 0.0f,  +100.0f };

        m_retryStartPos = { m_endStartPos.x - 450.0f, m_endStartPos.y};
        m_retryTargetPos = { m_endTargetPos.x - 450.0f, m_endTargetPos.y - 50 };

        m_menuStartPos = { m_endStartPos.x + 450.0f, m_endStartPos.y};
        m_menuTargetPos = { m_endTargetPos.x + 450.0f, m_endTargetPos.y - 50 };

        // UI 활성화 + 시작 위치
        m_endingUI->SetActive(true);
        m_retry->SetActive(true);
        m_goMenu->SetActive(true);
        if (m_selecRetry)  m_selecRetry->SetActive(true);
        if (m_selecGoMenu) m_selecGoMenu->SetActive(true);
        
        // 타입별 메인 배너 텍스처(자산 이름은 프로젝트에 맞게)
        if (type == EndingType::Bad) {
            m_endingUI->SetTextureName("ART_BadEnd01");
        }
        else if(type == EndingType::Good){
            m_endingUI->SetTextureName("ART_GoodEnd01"); 
        }
        else {
            m_endingUI->SetTextureName("ART_NomalEnd01");
        }

        m_endingUI->SetPosition(m_endStartPos);
        m_retry->SetPosition(m_retryStartPos);
        m_goMenu->SetPosition(m_menuStartPos);
    }
}
