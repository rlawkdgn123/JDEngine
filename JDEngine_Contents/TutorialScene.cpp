#include "pch.h"
#include "framework.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "RectTransform.h"
#include "D2DTransform.h"
#include "Texture.h"
#include "BuildSystem.h"
#include "TutorialScene.h"
#include "CombatSystem.h"
#include "WaveManager.h"
#include "ExpeditionSystem.h"
#include "Grid.h"
#include "ParticleSystem.h"

using namespace std;
using namespace JDGameObject::Content;
using JDComponent::AnimationRender;
using JDComponent::D2DTM::RectTransform;
using JDComponent::TextureRenderer;


namespace JDScene {

    // TestScene
    void TutorialScene::OnEnter() {
        system("cls");

        using namespace JDGameObject;
        using namespace JDComponent;
        using JDScene::TutorialScene;

        m_buildSystem = make_unique<BuildSystem>();

        // 그리드 클릭했을 때, 건설

        // 게임 맵 생성
        CreateGameMap();

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

        // 자원 세팅.
        auto& rs = ResourceSystem::Instance();
        rs.SetTotalResource(Resource(1000, 1000, 1000));
        rs.SetMaxPopulation(500);
        rs.SetCurPopulation(100);

        m_playerArmy.OverrideUnitCounts({ 100, 100 });

        // 병영.
        m_barracksObject = CreateStructure(L"barracksObj", JDGlobal::Base::GameTag::Barracks, { -210.0f, 75.0f }, "house");

        // 성벽.
        // m_wallObject = CreateStructure(L"wallObj", JDGlobal::Base::GameTag::Wall, { -300.0f, 75.0f }, "house");

        /////////////////////////////////////////////////////////////////////////////\
        
        // [추가] 튜토리얼 시작
        m_currentStep = TutorialStep::STEP_1_NOTICE_A;
        EnterStep1_NoticeA(); // 첫 번째 튜토리얼 단계 시작

        /////////////////////////////////////////////////////////////////////////////
        //AudioManager::Instance().PlayBGM("BGM_Battle", &bgmChannel);
        AudioManager::Instance().PlayBGM("BGM_Fiield", &bgmChannel);

        torches.push_back(makeTorch(L"Torch01", { -100.f,100.f }, "basic_Torch", "Torch", "Torch", { 0, 0 }, 0.20f));
        torches.push_back(makeTorch(L"Torch02", { 50.f,100.f }, "basic_Torch", "Torch", "Torch", { 0, 0 }, 0.20f));
        torches.push_back(makeTorch(L"Torch03", { 00.f,100.f }, "basic_Torch", "Torch", "Torch", { 0, 0 }, 0.20f));
        torches.push_back(makeTorch(L"Torch04", { -50.f,100.f }, "basic_Torch", "Torch", "Torch", { 0, 0 }, 0.20f));
        torches.push_back(makeTorch(L"Torch05", { -100.f,100.f }, "basic_Torch", "Torch", "Torch", { 0, 0 }, 0.20f));
        torches.push_back(makeTorch(L"Torch06", { -150.f,100.f }, "basic_Torch", "Torch", "Torch", { 0, 0 }, 0.20f));
        torches.push_back(makeTorch(L"Torch07", { -200.f,100.f }, "basic_Torch", "Torch", "Torch", { 0, 0 }, 0.20f));

        frogs.push_back(makeActor(L"Frog01", { 100.f, 100.f }, "basic_Frog", "Frog1", "FrogSound", { 960, 360 }));
        frogs.push_back(makeActor(L"Frog02", { 160.f, 100.f }, "basic_Frog", "Frog1", "RatHide", { 960, 360 }));
        frogs.push_back(makeActor(L"Frog03", { 220.f, 100.f }, "basic_Frog", "Frog1", "RatHide", { 960, 360 }));
        frogs.push_back(makeActor(L"Frog04", { 280.f, 100.f }, "basic_Frog", "Frog1", "FrogSound", { 960, 360 }));

        rats.push_back(makeActor(L"Rat01", { 00.f, 100.f }, "basic_Rat", "Rat", "RatHide", { 960, 355 }));
        rats.push_back(makeActor(L"Rat02", { 60.f, 100.f }, "basic_Rat", "Rat", "RatHide", { 960, 355 }));
        rats.push_back(makeActor(L"Rat03", { 120.f, 100.f }, "basic_Rat", "Rat", "RatHide", { 960, 355 }));

        //파티클 초기화
        m_mouseParticles = std::make_unique<ParticleSystem>(
            D2DRenderer::Instance().GetD2DContext()
        );
        m_leafParticles = std::make_unique<ParticleSystem>(
            D2DRenderer::Instance().GetD2DContext()
        );
        m_smokeParticles = std::make_unique<ParticleSystem>(
            D2DRenderer::Instance().GetD2DContext()
        );
        m_footdustParticles = std::make_unique<ParticleSystem>(
            D2DRenderer::Instance().GetD2DContext()
        );
        m_waveParticles = std::make_unique<ParticleSystem>(
            D2DRenderer::Instance().GetD2DContext()
        );
    }

    void TutorialScene::OnLeave() {
        //cout << "[TestScene] OnLeave()\n";
        if (bgmChannel) {
            bgmChannel->stop(); // FMOD에서 채널을 멈춤
            bgmChannel = nullptr; // 포인터도 초기화 (안전)
        }
    }

    void TutorialScene::Update(float deltaTime) {
        SceneBase::Update(deltaTime);

        UpdateResourceUI();

        // 튜토리얼 로직 업데이트
        UpdateTutorial(deltaTime);

        // 화면 크기(파티클 off-screen 처리용)
        auto cam = D2DRenderer::Instance().GetCamera();
        const float W = cam ? (float)cam->GetScreenWidth() : 1920.f;
        const float H = cam ? (float)cam->GetScreenHeight() : 1080.f;

        // 1) 개구리/쥐 상태 타이머 갱신 (애니 끝나면 텍스처 복귀, 락 해제 등)
        for (auto& f : frogs) updateActor(f, deltaTime);
        for (auto& r : rats)  updateActor(r, deltaTime);

        // 2) 토치: (필요시) 애니 상태 갱신 + 연기 스폰/업데이트
        //    - 연기 스폰은 클릭에서 on/off만 바꾸고, 여기서 주기적으로 생성
        for (auto& t : torches)
        {
            updateActor(t.actor, deltaTime); // 토치도 애니에서 텍스처로 복귀 같은 공통 로직 쓰고 싶으면 유지

            // 개별 토치의 스모크 발생/누적/스폰 + 스모크 PS 업데이트
            updateTorchSmoke(t, m_smokeParticles.get(), deltaTime, W, H);
        }

        // 3) 다른 파티클 시스템 업데이트 (스모크는 위에서 이미 업데이트됨)
        if (m_smokeParticles)  m_smokeParticles->UpdateSmoke(deltaTime, W, H);
        if (m_leafParticles)  m_leafParticles->UpdateLeaves(deltaTime, W, H);
        if (m_waveParticles)  m_waveParticles->UpdateWaterWave(deltaTime);
        //if (m_footdustParticles) m_footdustParticles->UpdateFootDust(deltaTime, W, H);
        ////////////////////////////////////////////////////////////////////////////////

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
    }

    void TutorialScene::FixedUpdate(float fixedDeltaTime) {
        SceneBase::FixedUpdate(fixedDeltaTime);
        //cout << "[TestScene] FixedUpdate() - dt: " << fixedDeltaTime << "\n";
    }

    void TutorialScene::LateUpdate(float deltaTime) {
        SceneBase::LateUpdate(deltaTime);
        //cout << "[TestScene] LateUpdate() - dt: " << deltaTime << "\n";
    }

    void TutorialScene::Render(float deltaTime) {
        SceneBase::Render(deltaTime);

        m_fader.Update(deltaTime);

        auto camera = D2DRenderer::Instance().GetCamera();

        if (camera)
        {
            D2DRenderer::Instance().SetTransform(camera->GetViewMatrix());
        }
        else
        {
            D2DRenderer::Instance().SetTransform(D2D1::Matrix3x2F::Identity());
        }

        auto ctx = D2DRenderer::Instance().GetD2DContext();

        float screenW = static_cast<float>(camera->GetScreenWidth());
        float screenH = static_cast<float>(camera->GetScreenHeight());

        //게임 오브젝트 렌더
        //TODO:: 오브젝트 리소스 리젠 페이드아웃
        for (auto& obj : m_gameObjects) {
            if (auto grid = dynamic_cast<Grid*>(obj.get())) {
                if (grid->HasBuilding()) {
                    //auto resText = CreateUIObject<Text>(L"resText");
                    //resText->SetText(L"agadfngldfgjkdf");
                    //resText->SetPosition({0, 0});
                    //resText->SetColor(D2D1::ColorF(D2D1::ColorF::White, 1.f)); // 화이트여야
                }
            }
            D2DRenderer::Instance().RenderGameObject(*obj, deltaTime);
        }

        DrawColider();

        // ====================================================================
        // 파티클 렌더링
        // ====================================================================

        // 1. 파티클을 위한 기본 변환 행렬(baseTransform)을 계산합니다.
        //    (카메라 뷰 행렬 * 화면 중앙 행렬)
        const auto viewMatrix = camera->GetViewMatrix();
        const auto centerMatrix = JDComponent::D2DTM::Transform::GetScreenCenterTranslation(); // static 함수로 만들었음.
        const auto particleBaseTransform = viewMatrix * centerMatrix;

        if (m_waveParticles)
        {
            m_waveParticles->RenderWaterWave(ctx, particleBaseTransform);
        }
        if (m_leafParticles)
        {
            m_leafParticles->RenderLeaves(ctx, particleBaseTransform);
        }
        if (m_smokeParticles)
        {
            m_smokeParticles->RenderSmoke(ctx, particleBaseTransform);
        }





        for (auto& uiObj : m_uiObjects)
        {
            D2DRenderer::Instance().RenderUIObject(*uiObj);
        }

        D2D1_MATRIX_3X2_F view;
        ctx->GetTransform(&view);
        ctx->SetTransform(D2D1::Matrix3x2F::Identity());

        m_fader.Render(ctx, { (float)screenW, (float)screenH });

        ctx->SetTransform(view);
        // 카메라 뷰가 걸려 있는 상태이므로, 
        // 페이드를 스크린 공간에서 그리려면 Transform을 초기화

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
    }
    void TutorialScene::ProcessDayTimer(float deltaTime)
    {
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
                //float windowWidth = JDGlobal::Window::WindowSize::Instance().GetWidth();
                //SpawnWaveEnemy({ (windowWidth + 100.0F) / 2.0f, 100.0f });
                SpawnWaveEnemy({ 1010.0f, 245.0f });
                AddEnemyDay(warningDay);
                std::cout << "[GameScene] 적 스폰됨." << std::endl;
            }
        }
    }
    void TutorialScene::ProcessBattle(float deltaTime)
    {
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
    void TutorialScene::SpawnWaveEnemy(const Vector2F& pos)
    {
        auto* enemyObj = CreateSoldierUnit(m_enemyArmy.GetUnitCounts(), JDGlobal::Base::GameTag::Enemy,
            JDGlobal::Contents::State::Move, pos, "f1");

        m_enemyArmy.OverrideUnitCounts({ 0, 0 });
    }
    void TutorialScene::SpawnPlayerArmy(const Vector2F& pos)
    {
        auto* playerObj = CreateSoldierUnit(m_playerArmy.GetUnitCounts(), JDGlobal::Base::GameTag::Player,
            JDGlobal::Contents::State::Move, pos, "f1");
        m_playerArmy.OverrideUnitCounts({ 0, 0 });
        if (!m_playerObject && !m_battleObject) m_playerObject = playerObj;
    }
    void TutorialScene::SpawnBattleObject(const Vector2F& pos)
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
    void TutorialScene::MoveEnemyObjects(float deltaTime)
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
            /*if (m_wallObject) {
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
            }*/

            Vector2F diffPos = m_wallPos - transform->GetPosition();

            if (diffPos.Length() <= 10.0f) {
                objPtr->SetState(JDGlobal::Contents::State::Idle);

                auto it = std::find_if(m_attackers.begin(), m_attackers.end(),
                    [&](auto& a) { return a.enemy == objPtr.get(); });
                if (it == m_attackers.end()) { m_attackers.push_back({ objPtr.get(), 0.0f }); }
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
    void TutorialScene::MovePlayerObjects(float deltaTime)
    {
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
    void TutorialScene::MoveExpedition(float deltaTime)
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

        if (dist < 10.0f) { // �� �߰� ������ �����ϸ� ��ǥ ������ ���� ������ �ٲ۴�.
            m_currentWaypointIndex = (m_currentWaypointIndex + 1) % m_waypoints.size();

            if (m_currentWaypointIndex == 0) { // ���� ������ ������ ���������� ���� ����� �����ϰ�, �����븦 �����Ѵ�. 
                JDGameSystem::ExpeditionSystem::Instance().ResolveExpedition();
                SafeDestroy(m_expeditionObject);
                std::cout << "[GameScene] ���� ��." << std::endl;
            }
        }
        else {
            Vector2F direction = diff.Normalized();
            current += direction * speed * deltaTime;
            transform->SetPosition(current);
        }
    }
    void TutorialScene::AttackWall(float deltaTime)
    {
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
    void TutorialScene::SafeDestroy(GameObjectBase* obj)
    {
        if (!obj) return;

        if (m_targetEnemy == obj) m_targetEnemy = nullptr;
        if (m_playerObject == obj) m_playerObject = nullptr;
        if (m_barracksObject == obj) m_barracksObject = nullptr;
        if (m_battleObject == obj) m_battleObject = nullptr;
        // if (m_wallObject == obj) m_wallObject = nullptr;
        if (m_expeditionObject == obj) m_expeditionObject = nullptr;
        if (GetSelectedObject() == obj) SetSelectedObject(nullptr);

        m_attackers.erase(
            std::remove_if(m_attackers.begin(), m_attackers.end(),
                [&](auto& a) { return a.enemy == obj; }),
            m_attackers.end()
        );

        SceneBase::DestroyObject(obj);
    }
    void TutorialScene::ClickUpdate()
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

                // Frogs
                for (auto& f : frogs) {
                    if (f.col && f.col->IsMouseOver(worldMousePos)) {

                        SetSelectedObject(f.go);
                        clicked = true;
                        onClickFrog(f, m_waveParticles.get(), rng, worldMousePos);
                        break;
                    }
                }

                // Rats
                if (!clicked) {
                    for (auto& r : rats) {
                        if (r.col && r.col->IsMouseOver(worldMousePos)) {
                            SetSelectedObject(r.go);
                            clicked = true;
                            std::cout << "[CLICK] rat hit, m_leafParticles=" << m_leafParticles.get() << "\n";
                            onClickRat(r, m_leafParticles.get(), rng, worldMousePos);
                            break;
                        }
                    }
                }

                // Torches
                if (!clicked) {
                    for (auto& t : torches) {
                        if (t.actor.col && t.actor.col->IsMouseOver(worldMousePos)) {
                            SetSelectedObject(t.actor.go);
                            clicked = true;
                            toggleTorch(t);                             // 애니/사운드/연기 토글
                            break;
                        }
                    }
                }

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

                            if (collider->IsOpen())
                            {
                                std::cout << "[DEBUG] right ID: " << id << std::endl;

                                m_selectedCollider = collider;

                                //auto* boxCol = static_cast<JDComponent::BoxCollider*>(collider);

                                if (grid->IsOccupied()) {
                                    if (grid->HasBuilding()) {
                                        cout << "HasBuilding() : True" << endl;
                                        isGridSetting = true;
                                        ShowGridSettingMenu();
                                    }
                                    else {
                                        cout << "HasBuilding() : False" << endl;
                                        isGridBuild = true;
                                        ShowGridCreateMenu();
                                    }
                                }
                                else {
                                    if (grid->IsExpanded()) {
                                        cout << "확장 가능한 미점유 지역입니다." << endl;
                                    }
                                    else {
                                        cout << "확장 불가능한 미점유 지역입니다." << endl;
                                    }
                                }


                            }

                            collider->SetOpen(true);
                            std::cout << "[DEBUG] left ID: " << id << std::endl;
                        }

                        // 태그 기반 처리
                        auto* gameObj = GetSelectedObject();
                        if (!gameObj) return;

                        auto tag = gameObj->GetTag();
                        bool isEnemyTag = (tag == JDGlobal::Base::GameTag::Enemy);

                        if (tag == JDGlobal::Base::GameTag::Barracks) {
                            m_isBarracksSelected = !m_isBarracksSelected;
                            std::cout << "[GameScene] 병영 클릭. 선택 상태: " << m_isBarracksSelected << std::endl;

                            // UI 로직
                            if (m_isBarracksSelected)
                            {
                                isAway = true;
                                ShowAwayMenu();
                            }
                            else
                            {
                                isAway = false;
                                CloseAwayMenu();
                            }
                        }
                        else if (m_isBarracksSelected && !m_battleObject && (isEnemyTag && !m_targetEnemy) && m_playerArmy.GetTotalUnits() > 0) {
                            m_targetEnemy = gameObj;
                            Vector2F pos = m_barracksObject->GetComponent<Transform>()->GetPosition();
                            SpawnPlayerArmy(pos);
                            //m_isBarracksSelected = false;
                            std::cout << "[GameScene] 병영 클릭 -> 적 클릭." << std::endl;
                        }
                        else if (m_isBarracksSelected && tag == JDGlobal::Base::GameTag::BattleAnim && m_playerArmy.GetTotalUnits() > 0) {
                            Vector2F pos = m_barracksObject->GetComponent<Transform>()->GetPosition();
                            SpawnPlayerArmy(pos);
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
                std::cout << "  [BACKGROUND CLICKED] Creating particle at World Pos: (" << worldMousePos.x << ", " << worldMousePos.y << ")\n\n";

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

                    m_isBarracksSelected = false;
                }
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

    JDGameObject::GameObjectBase* TutorialScene::CreateSoldierUnit(const JDGameSystem::UnitCounts& units, JDGlobal::Base::GameTag tag, JDGlobal::Contents::State state, const Vector2F& pos, const std::string& textureName)
    {
        auto* obj = CreateGameObject<Soldier>((tag == JDGlobal::Base::GameTag::Player) ? L"playerObj" : L"enemyObj");
        obj->SetUnitCounts(units);

        JDGameSystem::ArmySystem army;
        army.OverrideUnitCounts(units);
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
            auto* texRenderer = obj->GetComponent<TextureRenderer>();
            if (texRenderer) {
                texRenderer->SetFlipX(true); // 좌우 반전!
            }
        }

        return obj;
    }

    JDGameObject::GameObjectBase* TutorialScene::CreateStructure(const std::wstring& name, JDGlobal::Base::GameTag tag, const Vector2F& pos, const std::string& textureName)
    {
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

    JDGameObject::GameObjectBase* TutorialScene::CreateUIButton(const std::wstring& name, const Vector2F& pos, const std::string& textureName, const std::string& clickEventName, std::function<void()> callback)
    {
        auto* btn = CreateUIObject<Button>(name);
        btn->SetTextureName(textureName);
        btn->SetSizeToOriginal();
        btn->SetPosition(pos);
        btn->AddOnClick(clickEventName, callback);

        return btn;
    }

    void TutorialScene::CreateExpedition()
    {
        auto* obj = CreateGameObject<Soldier>(L"playerObj");

        obj->SetTag(JDGlobal::Base::GameTag::Player);
        obj->SetState(JDGlobal::Contents::State::Idle);
        obj->AddComponent<JDComponent::TextureRenderer>("f1", RenderLayerInfo{ SortingLayer::Cat, 1 });

        // auto bitmap = static_cast<ID2D1Bitmap*>(AssetManager::Instance().GetTexture("f1"));
        // Vector2F size = { bitmap->GetSize().width / 2.0f, bitmap->GetSize().height / 2.0f };
        // obj->AddComponent<JDComponent::BoxCollider>(size);
        // obj->GetComponent<JDComponent::BoxCollider>()->SetOpen(true);

        Vector2F pos = m_barracksObject->GetComponent<Transform>()->GetPosition();
        obj->GetComponent<Transform>()->SetPosition(pos);
        obj->AddComponent<JDComponent::SFX>("Step");

        m_expeditionObject = obj;

        std::cout << "[tutoScene] 원정대 생성." << std::endl;
    }

    void TutorialScene::UpdateResourceUI() {

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
    }

    void TutorialScene::InitGridCreateMenu()
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

    void TutorialScene::ShowGridCreateMenu()
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

    void TutorialScene::CloseGridCreateMenu()
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

    void TutorialScene::InitGridSettingMenu()
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

    //void TutorialScene::ShowUpgradeMenu()
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

    void TutorialScene::CloseGridSettingMenu()
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

    //void TutorialScene::ShowBuildInfo(JDGlobal::Contents::BuildingType buildType, std::string costText, std::string effectText)
    void TutorialScene::ShowGridCreateInfo(JDGlobal::Contents::BuildingType buildType, std::string costText, std::string effectText)
    {
        ChangeGridCreateInfo(buildType, costText, effectText);

        m_costInfoText->SetActive(true);
        m_costText->SetActive(true);
        m_costImage->SetActive(true);

        m_effectInfoText->SetActive(true);
        m_effectText->SetActive(true);
        m_effctImage->SetActive(true);
    }

    void TutorialScene::CloseGridCreateInfo()
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

    void TutorialScene::ShowCatInfo(JDGlobal::Contents::CatType catType, std::string costText, std::string effectText)
    {
        ChangeUpgradeInfo(catType, costText, effectText);

        m_catCostInfoText->SetActive(true);
        m_catCostText->SetActive(true);
        m_catCostImage->SetActive(true);

        m_catEffectInfoText->SetActive(true);
        m_catEffectText->SetActive(true);
        m_catEffctImage->SetActive(true);
    }

    void TutorialScene::CloseCatInfo()
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

    void TutorialScene::ChangeUpgradeInfo(JDGlobal::Contents::CatType catType, std::string costText, std::string effectText)
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

    void TutorialScene::InitAwayMenu()
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

        m_awayAwardInfo->SetActive(false);
        m_awayAwardText01->SetActive(false);
        m_awayAwardText02->SetActive(false);

        m_awayButton->SetActive(false);
    }

    void TutorialScene::ShowAwayMenu()
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

        m_awayAwardInfo->SetActive(false);
        m_awayAwardText01->SetActive(false);
        m_awayAwardText02->SetActive(false);

        m_awayButton->SetActive(false);
        */
    }

    void TutorialScene::CloseAwayMenu()
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

        m_awayAwardInfo->SetActive(false);
        m_awayAwardText01->SetActive(false);
        m_awayAwardText02->SetActive(false);

        m_awayButton->SetActive(false);

    }

    void TutorialScene::ShowAwayPopup()
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

        m_awayAwardInfo->SetActive(true);
        m_awayAwardText01->SetActive(true);
        m_awayAwardText02->SetActive(true);

        m_awayButton->SetActive(true);
    }

    void TutorialScene::CloseAwayPopup()
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

        m_awayAwardInfo->SetActive(false);
        m_awayAwardText01->SetActive(false);
        m_awayAwardText02->SetActive(false);

        m_awayButton->SetActive(false);
    }

    // void TutorialScene::ChangeBuildInfo(JDGlobal::Contents::BuildingType buildType, std::string costText, std::string effectText)
    void TutorialScene::ChangeGridCreateInfo(JDGlobal::Contents::BuildingType buildType, std::string costText, std::string effectText)
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

    void TutorialScene::ShowGridSettingMenu() {
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

    void TutorialScene::CreateGameMap()
    {
        using namespace JDGameObject;
        using namespace JDComponent;
        using JDScene::TutorialScene;

        // =====================================================================
        // 게임 오브젝트 생성 (World Space)
        // =====================================================================

        // 배경
        /*auto* map_backGround = CreateGameObject<GameObject>(L"Map_Background");
        map_backGround->AddComponent<Transform>()->SetPosition(Vector2F{ 0.f, 0.f });
        auto map_backGroundTexture = map_backGround->AddComponent<TextureRenderer>("BACK_GROUND_OPACITY", RenderLayerInfo{ SortingLayer::BackGround, 1 });
        map_backGroundTexture->SetSize({ 3840, 2160 });*/

        //// 배경 풀 밭
        //auto* map_battle = CreateGameObject<GameObject>(L"Map_Field");
        //map_battle->AddComponent<Transform>()->SetPosition(Vector2F{ 0.f, 0.f });
        //auto map_battleTexture = map_battle->AddComponent<TextureRenderer>("ART_BattleMap01", RenderLayerInfo{ SortingLayer::BackGround, 1 });
        //// auto map_battleTexture = map_battle->AddComponent<TextureRenderer>("BATTLE_MAP(old)", RenderLayerInfo{ SortingLayer::BackGround, 1 });
        //map_battleTexture->SetSize({ JDGlobal::Window::WindowSize::Instance().GetSize() });

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
        map_castleTR->SetPosition(Vector2F{ -560.f, -13.f });

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
        map_barrack02TR->SetPosition(Vector2F{ -196.5f, 80.0f });

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
        m_stopButton->SetSize({ 40, 47 });
        m_stopButton->SetPosition({ 675.f, 475.f });

        // 정지 버튼 클릭하면 실행될 이벤트
        m_stopButton->AddOnClick("Quit Game", [this]()
            {
                SetTimeScale(0.0f);
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
        m_playButton->SetPosition({ 760.f, 475.f });

        // 재생 버튼 클릭하면 실행될 이벤트
        m_playButton->AddOnClick("Quit Game", [this]()
            {
                SetTimeScale(1.0f);
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
                SetTimeScale(2.0f);
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
        m_optionButton->SetPosition({ 923, 482 });

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

        // 원정 이미지
        m_away = CreateUIObject<Image>(L"UI_Away");
        m_away->SetTextureName("ART_Away01");
        m_away->SetSize({ 339.5f, 71.5f });
        m_away->SetPosition({ 778.7f, -313.3f });
        m_away->SetAnchor({ 1.0f, 1.0f });

        m_awayGauge = CreateUIObject<Image>(L"UI_AwayGauge");
        m_awayGauge->SetTextureName("ART_AwayGauge01");
        m_awayGauge->SetSize({ 316.f, 20.f });
        m_awayGauge->SetPosition({ 778.7f, -327.0f });
        m_awayGauge->SetAnchor({ 1.0f, 1.0f });

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
        m_buildTypeText->SetColor(D2D1::ColorF(0x69512C));
        m_buildTypeText->SetSize({ 300, 100 });
        m_buildTypeText->SetPosition({ -63.8f, -385.5f });

        // 코스트 텍스트
        m_costInfoText = CreateUIObject<Text>(L"UI_BuildCostInfoText");
        m_costInfoText->SetText(L"코스트 :");
        m_costInfoText->SetTextFormatName("Sebang_22");
        m_costInfoText->SetColor(D2D1::ColorF(0x69512C));
        m_costInfoText->SetSize({ 300, 100 });
        m_costInfoText->SetPosition({ -126, -435 });


        JDGlobal::Contents::HouseStats stat;
        DataTableManager::Instance().GetHouseTable(stat);

        m_costText = CreateUIObject<Text>(L"UI_BuildCostText");
        m_costText->SetText(to_wstring(stat.m_upgradeCost[0].m_wood));
        m_costText->SetTextFormatName("Sebang_22");
        m_costText->SetColor(D2D1::ColorF(0x69512C));
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
        m_effectInfoText->SetColor(D2D1::ColorF(0x69512C));
        m_effectInfoText->SetSize({ 300, 100 });
        m_effectInfoText->SetPosition({ -126, -479 });

        m_effectText = CreateUIObject<Text>(L"UI_BuildEffectText");
        m_effectText->SetText(L"1/초");
        m_effectText->SetTextFormatName("Sebang_22");
        m_effectText->SetColor(D2D1::ColorF(0x69512C));
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
        m_buildHouse->AddOnClick("Clicked House", [this]() {
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
                        "ART_BuildCabin01",
                        RenderLayerInfo{ SortingLayer::Building, 0 }
                    );

                    tr->SetTextureName("ART_BuildCabin01");
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
                ShowGridCreateInfo(JDGlobal::Contents::BuildingType::House, "50", "1/초");
            });

        // 주거지 버튼 마우스가 벗어나면 실행될 이벤트
        m_buildHouse->AddOnExit("Highlight Off", [this]()
            {
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
                wcout << L"낚시터 선택이되..................." << m_selectedCollider->GetOwner()->GetName() << endl;
                wcout << L"낚시터 선택이되..................." << endl;
                wcout << L"낚시터 선택이되..................." << endl;
                wcout << L"낚시터 선택이되..................." << endl;

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

                    FishingSpot* building = CreateGameObject<FishingSpot>(L"FishingSpot_" + boxCol->GetOwner()->GetName());

                    if (building) {
                        //m_TutorialObjects.push_back(building);
                        grid->SetBuilding(building); // 그리드에 빌딩 할당
                        grid->SetHasBuilding(true); // 그리드 건물 상태 flag : true
                        std::cout << "건물 추가됨" << std::endl;

                        auto* tr = building->AddComponent<TextureRenderer>(
                            "ART_BuildFishing01",
                            RenderLayerInfo{ SortingLayer::Building, 0 }
                        );

                        tr->SetTextureName("ART_BuildFishing01");
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
                ShowGridCreateInfo(JDGlobal::Contents::BuildingType::FishingSpot, "40", "2/초");
            });

        // 낚시터 버튼 마우스가 벗어나면 실행될 이벤트
        m_buildFishingspot->AddOnExit("Highlight Off", [this]()
            {
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
        m_buildRumbermill->AddOnClick("Clicked LumberMill", [this]() {
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
                        "ART_BuildLumbermill01",
                        RenderLayerInfo{ SortingLayer::Building, 0 }
                    );

                    tr->SetTextureName("ART_BuildLumbermill01");
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
                ShowGridCreateInfo(JDGlobal::Contents::BuildingType::LumberMill, "30", "3/초");
            });

        // 제재소 버튼 마우스가 벗어나면 실행될 이벤트
        m_buildRumbermill->AddOnExit("Highlight Off", [this]()
            {
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
        m_buildMine->AddOnClick("Clicked Mine", [this]() {
            wcout << L"광산 선택이되..................." << m_selectedCollider->GetOwner()->GetName() << endl;
            wcout << L"광산 선택이되..................." << endl;
            wcout << L"광산 선택이되..................." << endl;
            wcout << L"광산 선택이되..................." << endl;

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

                Mine* building = CreateGameObject<Mine>(L"Mine_" + boxCol->GetOwner()->GetName());

                if (building) {
                    //m_TutorialObjects.push_back(building);
                    grid->SetBuilding(building); // 그리드에 빌딩 할당
                    grid->SetHasBuilding(true); // 그리드 건물 상태 flag : true
                    std::cout << "건물 추가됨" << std::endl;

                    auto* tr = building->AddComponent<TextureRenderer>(
                        "ART_BuildMine01",
                        RenderLayerInfo{ SortingLayer::Building, 0 }
                    );

                    tr->SetTextureName("ART_BuildMine01");
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
                ShowGridCreateInfo(JDGlobal::Contents::BuildingType::Mine, "20", "4/초");
            });

        // 광산 버튼 마우스가 벗어나면 실행될 이벤트
        m_buildMine->AddOnExit("Highlight Off", [this]()
            {
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
        m_catTypeText->SetColor(D2D1::ColorF(0x69512C));
        m_catTypeText->SetSize({ 300, 100 });
        m_catTypeText->SetPosition({ -63.8f, -385.5f });

        // 고양이 배치 코스트 텍스트
        m_catCostInfoText = CreateUIObject<Text>(L"UI_CatCostInfoText");
        m_catCostInfoText->SetText(L"코스트 :");
        m_catCostInfoText->SetTextFormatName("Sebang_22");
        m_catCostInfoText->SetColor(D2D1::ColorF(0x69512C));
        m_catCostInfoText->SetSize({ 300, 100 });
        m_catCostInfoText->SetPosition({ -126, -435 });

        m_catCostText = CreateUIObject<Text>(L"UI_CatCostText");
        m_catCostText->SetText(L"50");
        m_catCostText->SetTextFormatName("Sebang_20");
        m_catCostText->SetColor(D2D1::ColorF(0x69512C));
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
        m_catEffectInfoText->SetColor(D2D1::ColorF(0x69512C));
        m_catEffectInfoText->SetSize({ 300, 100 });
        m_catEffectInfoText->SetPosition({ -126, -479 });

        m_catEffectText = CreateUIObject<Text>(L"UI_CatEffectText");
        m_catEffectText->SetText(L"1/초");
        m_catEffectText->SetTextFormatName("Sebang_20");
        m_catEffectText->SetColor(D2D1::ColorF(0x69512C));
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
        m_koneSetButton->AddOnClick("On Click", [this]() {
            wcout << L"코네냥 선택이되..................." << m_selectedCollider->GetOwner()->GetName() << endl;
            wcout << L"코네냥 선택이되..................." << endl;
            wcout << L"코네냥 선택이되..................." << endl;
            wcout << L"코네냥 선택이되..................." << endl;

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
                ShowCatInfo(JDGlobal::Contents::CatType::Kone, "50", "1/초");
            });

        // 코네 선택 버튼 마우스가 벗어나면 실행될 이벤트
        m_koneSetButton->AddOnExit("Highlight Off", [this]()
            {
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
                ShowCatInfo(JDGlobal::Contents::CatType::Navi, "50", "1/초");
            });

        // 나비냥 선택 버튼 마우스가 벗어나면 실행될 이벤트
        m_naviSetButton->AddOnExit("Highlight Off", [this]()
            {
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
                ShowCatInfo(JDGlobal::Contents::CatType::Felis, "50", "1/초");
            });

        // 펠리스 선택 버튼 마우스가 벗어나면 실행될 이벤트
        m_felisSetButton->AddOnExit("Highlight Off", [this]()
            {
                CloseCatInfo();
            });


        ////////////////////////////////////////
        // 건물 업그레이드 정보
        ////////////////////////////////////////

        // 건물 타입 텍스트
        m_builtTypeText = CreateUIObject<Text>(L"UI_BuilTTypeText");
        m_builtTypeText->SetText(L"건물 정보");
        m_builtTypeText->SetTextFormatName("Sebang_Bold_24");
        m_builtTypeText->SetColor(D2D1::ColorF(0x69512C));
        m_builtTypeText->SetSize({ 300, 100 });
        m_builtTypeText->SetPosition({ 757.f, -385.5f });

        // 건물 업그레이드 코스트
        m_upgradeCostInfoText = CreateUIObject<Text>(L"UI_UpgradeCostInfoText");
        m_upgradeCostInfoText->SetText(L"코스트 :");
        m_upgradeCostInfoText->SetTextFormatName("Sebang_22");
        m_upgradeCostInfoText->SetColor(D2D1::ColorF(0x69512C));
        m_upgradeCostInfoText->SetSize({ 300, 100 });
        m_upgradeCostInfoText->SetPosition({ 697, -435 });

        m_upgradeCostText = CreateUIObject<Text>(L"UI_UpgradeCostText");
        m_upgradeCostText->SetText(L"50");
        m_upgradeCostText->SetTextFormatName("Sebang_20");
        m_upgradeCostText->SetColor(D2D1::ColorF(0x69512C));
        m_upgradeCostText->SetSize({ 300, 100 });
        m_upgradeCostText->SetPosition({ 824.5, -432 });

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
        m_upgradeEffectInfoText->SetColor(D2D1::ColorF(0x69512C));
        m_upgradeEffectInfoText->SetSize({ 300, 100 });
        m_upgradeEffectInfoText->SetPosition({ 702, -479 });

        m_upgradeEffectText = CreateUIObject<Text>(L"UI_UpgradeEffectText");
        m_upgradeEffectText->SetText(L"1/초");
        m_upgradeEffectText->SetTextFormatName("Sebang_20");
        m_upgradeEffectText->SetColor(D2D1::ColorF(0x69512C));
        m_upgradeEffectText->SetSize({ 300, 100 });
        m_upgradeEffectText->SetPosition({ 827, -478 });

        // 업그레이드 효과 이미지
        m_upgradeEffctImage = CreateUIObject<Image>(L"UI_UpgradeEffctImage");
        m_upgradeEffctImage->SetTextureName("ART_CostFood01");
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
        m_downgradeButton->AddOnClick("Quit Game", [this]()
            {
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

            });

        // 다운그레이드 버튼 마우스가 벗어나면 실행될 이벤트
        m_downgradeButton->AddOnExit("Highlight Off", [this]()
            {

            });

        //////////
        // 업그레이드 버튼
        m_upgradeButton = CreateUIObject<Button>(L"UI_upgradeButton");
        m_upgradeButton->SetTextureName("ART_Upgrade01");
        m_upgradeButton->SetText(L"");
        m_upgradeButton->SetSize({ 56.f, 40.f });
        m_upgradeButton->SetPosition({ 858, -390 });

        // 업그레이드 버튼 클릭하면 실행될 이벤트
        m_upgradeButton->AddOnClick("Quit Game", [this]()
            {
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

            });

        // 업그레이드 버튼 마우스가 벗어나면 실행될 이벤트
        m_upgradeButton->AddOnExit("Highlight Off", [this]()
            {

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
        m_trainerCatButton->SetTextureName("ART_RecruitCat03");
        m_trainerCatButton->SetSize({ 256, 279 });
        m_trainerCatButton->SetPosition({ -178, -340 });
        m_trainerCatButton->SetAnchor({ 1.0f, 0.0f });
        m_trainerCatButton->SetScale({ 0.5f, 0.5f });

        // 견습냥이 버튼 클릭 시 실행될 이벤트
        m_trainerCatButton->AddOnClick("On Click", [this]()
            {
                m_playerArmy.RecruitUnits(JDGameSystem::SordierType::Novice);
                std::cout << "Player Power: " << m_playerArmy.CalculateTotalPower() << std::endl;
            });

        // 견습냥이 버튼 마우스를 올리면 실행될 이벤트
        m_trainerCatButton->AddOnEnter("Highlight On", [this]()
            {

            });

        // 견습냥이 버튼 마우스가 벗어나면 실행될 이벤트
        m_trainerCatButton->AddOnExit("Highlight Off", [this]()
            {

            });

        m_trainerCatName = CreateUIObject<Text>(L"UI_TrainerCatNameText");
        m_trainerCatName->SetText(L"견습냥이");
        m_trainerCatName->SetTextFormatName("Sebang_Bold_30");
        m_trainerCatName->SetColor(D2D1::ColorF(0x69512C));
        m_trainerCatName->SetSize({ 300, 100 });
        m_trainerCatName->SetPosition({ -100, -503 });

        // 견습냥이 코스트 _ 01 ( Info & 이미지 & 텍스트 )
        m_trainerCatCostInfo = CreateUIObject<Text>(L"UI_TrainerCatCostInfoText");
        m_trainerCatCostInfo->SetText(L"코스트 :");
        m_trainerCatCostInfo->SetTextFormatName("Sebang_21");
        m_trainerCatCostInfo->SetColor(D2D1::ColorF(0x69512C));
        m_trainerCatCostInfo->SetSize({ 300, 100 });
        m_trainerCatCostInfo->SetPosition({ 5, -380 });

        m_trainerCatCostImage01 = CreateUIObject<Image>(L"UI_TrainerCatCostImage01");
        m_trainerCatCostImage01->SetTextureName("ART_CostWood01");
        m_trainerCatCostImage01->SetSizeToOriginal();
        m_trainerCatCostImage01->SetScale({ 0.5f, 0.5f });
        m_trainerCatCostImage01->SetPosition({ 48, -361 });
        m_trainerCatCostImage01->SetAnchor({ 1.0f, 0.0f });

        m_trainerCatCostText01 = CreateUIObject<Text>(L"UI_TrainerCatCostText01");
        m_trainerCatCostText01->SetText(L"x50");
        m_trainerCatCostText01->SetTextFormatName("Sebang_16");
        m_trainerCatCostText01->SetColor(D2D1::ColorF(0x69512C));
        m_trainerCatCostText01->SetSize({ 300, 100 });
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
        m_trainerCatCostText02->SetColor(D2D1::ColorF(0x69512C));
        m_trainerCatCostText02->SetSize({ 300, 100 });
        m_trainerCatCostText02->SetPosition({ 117, -422 });

        // 견습냥이 모집병력 Info & Text
        m_trainerCatRecruitInfo = CreateUIObject<Text>(L"UI_TrainerCatRecruitInfoText");
        m_trainerCatRecruitInfo->SetText(L"모집병력 :");
        m_trainerCatRecruitInfo->SetTextFormatName("Sebang_21");
        m_trainerCatRecruitInfo->SetColor(D2D1::ColorF(0x69512C));
        m_trainerCatRecruitInfo->SetSize({ 300, 100 });
        m_trainerCatRecruitInfo->SetPosition({ 14, -462 });

        m_trainerCatRecruitText = CreateUIObject<Text>(L"UI_TrainerCatRecruitText");
        m_trainerCatRecruitText->SetText(L"10");
        m_trainerCatRecruitText->SetTextFormatName("Sebang_16");
        m_trainerCatRecruitText->SetColor(D2D1::ColorF(0x69512C));
        m_trainerCatRecruitText->SetSize({ 300, 100 });
        m_trainerCatRecruitText->SetPosition({ 96, -462 });

        // 견습냥이 전투력 Info & Text
        m_trainerCatPowerInfo = CreateUIObject<Text>(L"UI_TrainerCatPowerInfoText");
        m_trainerCatPowerInfo->SetText(L"전투력 :");
        m_trainerCatPowerInfo->SetTextFormatName("Sebang_21");
        m_trainerCatPowerInfo->SetColor(D2D1::ColorF(0x69512C));
        m_trainerCatPowerInfo->SetSize({ 300, 100 });
        m_trainerCatPowerInfo->SetPosition({ 5, -500 });

        m_trainerCatPowerText = CreateUIObject<Text>(L"UI_TrainerCatPowerText");
        m_trainerCatPowerText->SetText(L"10");
        m_trainerCatPowerText->SetTextFormatName("Sebang_16");
        m_trainerCatPowerText->SetColor(D2D1::ColorF(0x69512C));
        m_trainerCatPowerText->SetSize({ 300, 100 });
        m_trainerCatPowerText->SetPosition({ 96, -500 });

        //////////
        // 징병 _ 숙련냥이 이미지
        m_expertCatButton = CreateUIObject<Button>(L"UI_ExpertCatButton");
        m_expertCatButton->SetTextureName("ART_RecruitCat03");
        m_expertCatButton->SetSize({ 256, 279 });
        m_expertCatButton->SetPosition({ 182, -340 });
        m_expertCatButton->SetAnchor({ 1.0f, 0.0f });
        m_expertCatButton->SetScale({ 0.5f, 0.5f });

        // 견습냥이 버튼 클릭 시 실행될 이벤트
        m_expertCatButton->AddOnClick("On Click", [this]()
            {
                m_playerArmy.RecruitUnits(JDGameSystem::SordierType::Expert);
                std::cout << "Player Power: " << m_playerArmy.CalculateTotalPower() << std::endl;
            });

        // 견습냥이 버튼 마우스를 올리면 실행될 이벤트
        m_expertCatButton->AddOnEnter("Highlight On", [this]()
            {

            });

        // 견습냥이 버튼 마우스가 벗어나면 실행될 이벤트
        m_expertCatButton->AddOnExit("Highlight Off", [this]()
            {

            });

        m_expertCatName = CreateUIObject<Text>(L"UI_ExpertCatNameText");
        m_expertCatName->SetText(L"숙련냥이");
        m_expertCatName->SetTextFormatName("Sebang_Bold_30");
        m_expertCatName->SetColor(D2D1::ColorF(0x69512C));
        m_expertCatName->SetSize({ 300, 100 });
        m_expertCatName->SetPosition({ 258, -503 });

        // 숙련냥이 코스트 _ 01 ( Info & 이미지 & 텍스트 )
        m_expertCatCostInfo = CreateUIObject<Text>(L"UI_ExpertCatCostInfoText");
        m_expertCatCostInfo->SetText(L"코스트 :");
        m_expertCatCostInfo->SetTextFormatName("Sebang_21");
        m_expertCatCostInfo->SetColor(D2D1::ColorF(0x69512C));
        m_expertCatCostInfo->SetSize({ 300, 100 });
        m_expertCatCostInfo->SetPosition({ 362, -380 });

        m_expertCatCostImage01 = CreateUIObject<Image>(L"UI_ExpertCatCostImage01");
        m_expertCatCostImage01->SetTextureName("ART_CostWood01");
        m_expertCatCostImage01->SetSizeToOriginal();
        m_expertCatCostImage01->SetScale({ 0.5f, 0.5f });
        m_expertCatCostImage01->SetPosition({ 406, -361 });
        m_expertCatCostImage01->SetAnchor({ 1.0f, 0.0f });

        m_expertCatCostText01 = CreateUIObject<Text>(L"UI_ExpertCatCostText01");
        m_expertCatCostText01->SetText(L"x50");
        m_expertCatCostText01->SetTextFormatName("Sebang_16");
        m_expertCatCostText01->SetColor(D2D1::ColorF(0x69512C));
        m_expertCatCostText01->SetSize({ 300, 100 });
        m_expertCatCostText01->SetPosition({ 476, -376 });

        // 숙련냥이 코스트 _ 02 ( 이미지 & 텍스트 )
        m_expertCatCostImage02 = CreateUIObject<Image>(L"UI_ExpertCatCostImage02");
        m_expertCatCostImage02->SetTextureName("ART_CostWood01");
        m_expertCatCostImage02->SetSizeToOriginal();
        m_expertCatCostImage02->SetScale({ 0.5f, 0.5f });
        m_expertCatCostImage02->SetPosition({ 406, -404 });
        m_expertCatCostImage02->SetAnchor({ 1.0f, 0.0f });

        m_expertCatCostText02 = CreateUIObject<Text>(L"UI_ExpertCatCostText02");
        m_expertCatCostText02->SetText(L"x50");
        m_expertCatCostText02->SetTextFormatName("Sebang_16");
        m_expertCatCostText02->SetColor(D2D1::ColorF(0x69512C));
        m_expertCatCostText02->SetSize({ 300, 100 });
        m_expertCatCostText02->SetPosition({ 476, -422 });

        // 숙련냥이 모집병력 Info & Text
        m_expertCatRecruitInfo = CreateUIObject<Text>(L"UI_ExpertCatRecruitInfoText");
        m_expertCatRecruitInfo->SetText(L"모집병력 :");
        m_expertCatRecruitInfo->SetTextFormatName("Sebang_21");
        m_expertCatRecruitInfo->SetColor(D2D1::ColorF(0x69512C));
        m_expertCatRecruitInfo->SetSize({ 300, 100 });
        m_expertCatRecruitInfo->SetPosition({ 372, -462 });

        m_expertCatRecruitText = CreateUIObject<Text>(L"UI_ExpertCatRecruitText");
        m_expertCatRecruitText->SetText(L"10");
        m_expertCatRecruitText->SetTextFormatName("Sebang_16");
        m_expertCatRecruitText->SetColor(D2D1::ColorF(0x69512C));
        m_expertCatRecruitText->SetSize({ 300, 100 });
        m_expertCatRecruitText->SetPosition({ 455, -462 });

        // 숙련냥이 전투력 Info & Text
        m_expertCatPowerInfo = CreateUIObject<Text>(L"UI_ExpertCatPowerInfoText");
        m_expertCatPowerInfo->SetText(L"전투력 :");
        m_expertCatPowerInfo->SetTextFormatName("Sebang_21");
        m_expertCatPowerInfo->SetColor(D2D1::ColorF(0x69512C));
        m_expertCatPowerInfo->SetSize({ 300, 100 });
        m_expertCatPowerInfo->SetPosition({ 362, -500 });

        m_expertCatPowerText = CreateUIObject<Text>(L"UI_ExpertCatPowerText");
        m_expertCatPowerText->SetText(L"10");
        m_expertCatPowerText->SetTextFormatName("Sebang_16");
        m_expertCatPowerText->SetColor(D2D1::ColorF(0x69512C));
        m_expertCatPowerText->SetSize({ 300, 100 });
        m_expertCatPowerText->SetPosition({ 455, -500 });

        //////////
        // 원정 보내기 Info
        m_awayInfo = CreateUIObject<Text>(L"UI_AwayInfoText");
        m_awayInfo->SetText(L"원정 보내기");
        m_awayInfo->SetTextFormatName("Sebang_Bold_24");
        m_awayInfo->SetColor(D2D1::ColorF(0x69512C));
        m_awayInfo->SetSize({ 300, 100 });
        m_awayInfo->SetPosition({ 760, -379 });

        // 원정 초급 Button
        m_awayBeginner = CreateUIObject<Button>(L"UI_AwayBeginnerButton");
        m_awayBeginner->SetTextureName("초급 복사 2");
        m_awayBeginner->SetText(L"");
        m_awayBeginner->SetSize({ 174.8f, 33.0f });
        m_awayBeginner->SetPosition({ 760, -425 });

        // 원정 초급 버튼 클릭하면 실행될 이벤트
        m_awayBeginner->AddOnClick("On Click", [this]()
            {
                ShowAwayPopup();
                m_awayPopupInfo->SetText(L"초급 원정");
                m_awayButton->SetTextureName("병력 보내기  복사 2");
            });

        // 원정 초급 버튼 마우스를 올리면 실행될 이벤트
        m_awayBeginner->AddOnEnter("Highlight On", [this]()
            {

            });

        // 원정 초급 버튼 마우스가 벗어나면 실행될 이벤트
        m_awayBeginner->AddOnExit("Highlight Off", [this]()
            {

            });

        // 중급 Button
        // 원정 중급 Button
        m_awayIntermediate = CreateUIObject<Button>(L"UI_AwayIntermediate");
        m_awayIntermediate->SetTextureName("중급 복사 2");
        m_awayIntermediate->SetText(L"");
        m_awayIntermediate->SetSize({ 174.8f, 33.0f });
        m_awayIntermediate->SetPosition({ 760, -463 });

        // 원정 중급 버튼 클릭하면 실행될 이벤트
        m_awayIntermediate->AddOnClick("On Click", [this]()
            {
                ShowAwayPopup();
                m_awayPopupInfo->SetText(L"중급 원정");
                m_awayButton->SetTextureName("병력 보내기  복사 4");
            });

        // 원정 중급 버튼 마우스를 올리면 실행될 이벤트
        m_awayIntermediate->AddOnEnter("Highlight On", [this]()
            {

            });

        // 원정 중급 버튼 마우스가 벗어나면 실행될 이벤트
        m_awayIntermediate->AddOnExit("Highlight Off", [this]()
            {

            });

        // 상급 Button
        m_awayAdvanced = CreateUIObject<Button>(L"UI_AwayAdvanced");
        m_awayAdvanced->SetTextureName("상급 복사 2");
        m_awayAdvanced->SetText(L"");
        m_awayAdvanced->SetSize({ 174.8f, 33.0f });
        m_awayAdvanced->SetPosition({ 760, -500 });

        // 원정 상급 버튼 클릭하면 실행될 이벤트
        m_awayAdvanced->AddOnClick("On Click", [this]()
            {
                ShowAwayPopup();
                m_awayPopupInfo->SetText(L"상급 원정");
                m_awayButton->SetTextureName("병력 보내기  복사 6");
            });

        // 원정 상급 버튼 마우스를 올리면 실행될 이벤트
        m_awayAdvanced->AddOnEnter("Highlight On", [this]()
            {

            });

        // 원정 상급 버튼 마우스가 벗어나면 실행될 이벤트
        m_awayAdvanced->AddOnExit("Highlight Off", [this]()
            {

            });

        //////////
        // 원정 정보 팝업 Image
        m_awayPopupUI = CreateUIObject<Image>(L"UI_AwayPopupUI");
        m_awayPopupUI->SetTextureName("ART_CHAT");      // TODO : 실제 리소스로 바꿔야함.
        m_awayPopupUI->SetSize({ 386, 297 });
        m_awayPopupUI->SetPosition({ 770, -113 });
        m_awayPopupUI->SetAnchor({ 1.0f, 0.0f });

        // 원정 정보 Info
        m_awayPopupInfo = CreateUIObject<Text>(L"UI_AwayPopupInfoText");
        m_awayPopupInfo->SetText(L"초급 원정");
        m_awayPopupInfo->SetTextFormatName("Sebang_Bold_30");
        m_awayPopupInfo->SetColor(D2D1::ColorF(0x69512C));
        m_awayPopupInfo->SetSize({ 300, 100 });
        m_awayPopupInfo->SetPosition({ 766, -6 });

        // 원정 비용 Info
        m_awayCostInfo = CreateUIObject<Text>(L"UI_AwayCostInfoText");
        m_awayCostInfo->SetText(L"비용");
        m_awayCostInfo->SetTextFormatName("Sebang_Bold_22");
        m_awayCostInfo->SetColor(D2D1::ColorF(0x69512C));
        m_awayCostInfo->SetSize({ 300, 100 });
        m_awayCostInfo->SetPosition({ 622.5f, -65.0f });

        // 원정 코스트 _ 01 ( 이미지 & 텍스트 )
        m_awayCostImage01 = CreateUIObject<Image>(L"UI_AwayCostImage01");
        m_awayCostImage01->SetTextureName("ART_CostWood01");
        m_awayCostImage01->SetSizeToOriginal();
        m_awayCostImage01->SetScale({ 0.5f, 0.5f });
        m_awayCostImage01->SetPosition({ 666, -48 });
        m_awayCostImage01->SetAnchor({ 1.0f, 0.0f });

        m_awayCostText01 = CreateUIObject<Text>(L"UI_AwayCostText01");
        m_awayCostText01->SetText(L"50");
        m_awayCostText01->SetTextFormatName("Sebang_16");
        m_awayCostText01->SetColor(D2D1::ColorF(0x69512C));
        m_awayCostText01->SetSize({ 300, 100 });
        m_awayCostText01->SetPosition({ 737, -70 });

        // 원정 코스트 _ 02 ( 이미지 & 텍스트 )
        m_awayCostImage02 = CreateUIObject<Image>(L"UI_AwayCostImage02");
        m_awayCostImage02->SetTextureName("ART_CostWood01");
        m_awayCostImage02->SetSizeToOriginal();
        m_awayCostImage02->SetScale({ 0.5f, 0.5f });
        m_awayCostImage02->SetPosition({ 762, -48 });
        m_awayCostImage02->SetAnchor({ 1.0f, 0.0f });

        m_awayCostText02 = CreateUIObject<Text>(L"UI_AwayCostText02");
        m_awayCostText02->SetText(L"x50");
        m_awayCostText02->SetTextFormatName("Sebang_16");
        m_awayCostText02->SetColor(D2D1::ColorF(0x69512C));
        m_awayCostText02->SetSize({ 300, 100 });
        m_awayCostText02->SetPosition({ 831, -70 });

        /////
        // 보상 정보 Info
        m_awayAwardInfo = CreateUIObject<Text>(L"UI_AwayAwardInfo");
        m_awayAwardInfo->SetText(L"보상");
        m_awayAwardInfo->SetTextFormatName("Sebang_Bold_22");
        m_awayAwardInfo->SetColor(D2D1::ColorF(0x69512C));
        m_awayAwardInfo->SetSize({ 300, 100 });
        m_awayAwardInfo->SetPosition({ 622.5f, -140.0f });

        // 보상 정보 Text
        m_awayAwardText01 = CreateUIObject<Text>(L"UI_AwayAwardText01");
        m_awayAwardText01->SetText(L"N 원정포인트");
        m_awayAwardText01->SetTextFormatName("Sebang_20");
        m_awayAwardText01->SetColor(D2D1::ColorF(0x69512C));
        m_awayAwardText01->SetSize({ 300, 100 });
        m_awayAwardText01->SetPosition({ 729, -125 });

        // 추가 보상 정보 Text
        m_awayAwardText02 = CreateUIObject<Text>(L"UI_AwayAwardText02");
        m_awayAwardText02->SetText(L"N% 확률로 ㅇㅇㅇ 추가 보상");
        m_awayAwardText02->SetTextFormatName("Sebang_20");
        m_awayAwardText02->SetColor(D2D1::ColorF(0x69512C));
        m_awayAwardText02->SetSize({ 300, 100 });
        m_awayAwardText02->SetPosition({ 791, -162 });

        /////
        // 병력 보내기 Button
        m_awayButton = CreateUIObject<Button>(L"UI_AwayButton");
        m_awayButton->SetTextureName("병력 보내기  복사 2");
        m_awayButton->SetText(L"");
        m_awayButton->SetSize({ 125.f, 35.0f });
        m_awayButton->SetPosition({ 768, -216 });

        // 병력 보내기 버튼 클릭하면 실행될 이벤트
        m_awayButton->AddOnClick("On Click", [this]()
            {
                // TODO 초중급 나눠야하는데 우선 이미지로 판별할까?
                if (m_awayButton->GetTextureName() == "병력 보내기  복사 2")
                {
                    auto& sys = JDGameSystem::ExpeditionSystem::Instance();
                    if (sys.SendExpedition(JDGameSystem::ExpeditionGrade::Beginner)) CreateExpedition();
                }

                else if (m_awayButton->GetTextureName() == "병력 보내기  복사 4")
                {
                    auto& sys = JDGameSystem::ExpeditionSystem::Instance();
                    if (sys.SendExpedition(JDGameSystem::ExpeditionGrade::Intermediate)) CreateExpedition();
                }

                else if (m_awayButton->GetTextureName() == "병력 보내기  복사 6")
                {
                    auto& sys = JDGameSystem::ExpeditionSystem::Instance();
                    if (sys.SendExpedition(JDGameSystem::ExpeditionGrade::Higher)) CreateExpedition();
                }
            });

        // 병력 보내기 버튼 마우스를 올리면 실행될 이벤트
        m_awayButton->AddOnEnter("Highlight On", [this]()
            {

            });

        // 병력 보내기 버튼 마우스가 벗어나면 실행될 이벤트
        m_awayButton->AddOnExit("Highlight Off", [this]()
            {

            });

#pragma endregion

        // 2) 필터
        m_fillter = CreateUIObject<Image>(L"Fillter_Image");
        m_fillter->SetTextureName("BATTLE_MAP_3_Exam");
        m_fillter->SetColor(D2D1::ColorF(D2D1::ColorF::White, 0.65f));

        if (cam)
        {
            float screenWidth = static_cast<float>(cam->GetScreenWidth());
            float screenHeight = static_cast<float>(cam->GetScreenHeight());

            // 화면 크기로 설정
            m_fillter->SetSize(Vector2F{ screenWidth, screenHeight });
            m_fillter->SetPosition({ 0.0f,0.0f });
            m_fillter->SetPivot({ 0.5f, 0.5f });
        }

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


        // 4) 그리드 클릭하면 나오는 메뉴
        m_Menu = CreateUIObject<Image>(L"MenuBackground");

        //auto& uptr = m_uiObjects.back();
        //UIObject* rawPtr = uptr.get();
        //m_TutorialUIObjects.push_back(rawPtr);

        m_Menu->SetTextureName("Menu");
        m_Menu->SetSizeToOriginal();
        //m_Menu->SetSize({});
        m_Menu->SetActive(false);

        auto* bgRect = m_Menu->GetComponent<RectTransform>();
        bgRect->SetPivot({ 0.f,0.f });
        bgRect->SetAnchor({ 0.f,0.f });
        bgRect->SetPosition({ 50, 50 });

        //각버튼 키 목록
        std::vector<std::pair<std::wstring, std::string>> EmptyButtons = {
        { L"FishingSpot", "fishing" },
        { L"LumberMill", "lumbermill" },
        { L"Mine", "mine" },
        { L"House", "cabin" },
        // 필요하다면 더 추가…
        };
        std::vector<std::pair<std::wstring, std::string>> FilledButtons = {
        { L"CAT", "CAT" },
        // 필요하다면 더 추가…
        };

        // 공통 초기 오프셋
        float xOffset = 0.f;
        float yOffset = -150.f;
        float gap = 100.f;

        // 2-1) 빈 메뉴용 버튼 생성
        {
            float x = xOffset;
            float y = yOffset;
            for (auto& info : EmptyButtons) {
                auto* btn = CreateUIObject<Button>(info.first);
                // 튜토리얼용 보관
                //m_TutorialUIObjects.push_back(m_uiObjects.back().get());
                m_gridCreateButtons.push_back(btn);

                btn->SetTextureName(info.second);
                btn->SetText(L"");
                auto* rt = btn->GetComponent<RectTransform>();

                // [수정] RectTransform의 SetParent로만 부모-자식 관계를 설정합니다.
                rt->SetParent(bgRect);

                // [수정] 피벗을 중앙으로 설정하여 위치를 잡기 쉽게 합니다.
                rt->SetPivot({ 0.0f, 0.0f });
                rt->SetAnchor({ 0.0f, 0.0f });
                rt->SetSize({ 50, 50 });

                // 이제 이 Position은 버튼의 '중앙'을 부모의 (x, yOffset)으로 이동시킵니다.
                rt->SetPosition({ x, y });


                // LEGACY : 과거 코드
                /*
                //rt->SetPivot({ 0,0 });
                //rt->SetAnchor({ 0,0 });
                //rt->SetSize({ 50,50 });
                //rt->SetParent(bgRect);
                //rt->SetPosition({ x, yOffset });
                */

                m_Menu->AddChild(btn);
                btn->SetActive(false);
                m_menuButtons.push_back(btn);

                x += gap;
            }
        }

        // 2-2) 채운 메뉴용 버튼 생성
        {
            float x = xOffset;
            for (auto& info : FilledButtons) {
                auto* btn = CreateUIObject<Button>(info.first);
                //m_TutorialUIObjects.push_back(m_uiObjects.back().get());
                m_gridSettingButtons.push_back(btn);

                btn->SetTextureName(info.second);
                btn->SetText(L"");
                auto* rt = btn->GetComponent<RectTransform>();

                // [수정] RectTransform의 SetParent로만 부모-자식 관계를 설정합니다.
                rt->SetParent(bgRect);

                // [수정] 피벗을 중앙으로 설정하여 위치를 잡기 쉽게 합니다.
                rt->SetPivot({ 0.5f, 0.5f });
                rt->SetAnchor({ 0.0f, 0.0f });
                rt->SetSize({ 50, 50 });

                // 이제 이 Position은 버튼의 '중앙'을 부모의 (x, yOffset)으로 이동시킵니다.
                rt->SetPosition({ x, yOffset });

                // LEGACY : 과거 코드
                /*
                //rt->SetPivot({ 0,0 });
                //rt->SetAnchor({ 0,0 });
                //rt->SetSize({ 50,50 });
                //rt->SetParent(bgRect);
                //rt->SetPosition({ x, xOffset });
                */

                m_Menu->AddChild(btn);
                btn->SetActive(false);
                m_menuButtons.push_back(btn);

                x += gap;
            }
        }

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
        std::cout << "[DEBUG] m_emptyButtons.size(): " << m_gridCreateButtons.size() << std::endl;
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

    void TutorialScene::GameSpeedButtonUpdate()
    {
        // 0 배속일 떄
        if (GetTimeScale() == 0.0f)
        {
            m_stopButton->SetTextureName("ART_Pause01_ing");
            m_playButton->SetTextureName("ART_Play01");
            m_speedButton->SetTextureName("ART_Fast01");
        }

        else if (GetTimeScale() == 1.0f)
        {
            m_stopButton->SetTextureName("ART_Pause01");
            m_playButton->SetTextureName("ART_Play01_ing");
            m_speedButton->SetTextureName("ART_Fast01");
        }

        else
        {
            m_stopButton->SetTextureName("ART_Pause01");
            m_playButton->SetTextureName("ART_Play01");
            m_speedButton->SetTextureName("ART_Fast01_ing");
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // 튜토리얼 로직
    ////////////////////////////////////////////////////////////////////////////////

    // 튜토리얼 로직을 별도로 처리할 함수
    void TutorialScene::UpdateTutorial(float deltaTime)
    {
        // 튜토리얼이 완료되면 아무것도 하지 않음
        if (m_currentStep == TutorialStep::COMPLETE) return;

        // ImGui 창 위에 마우스가 있으면 튜토리얼 진행을 막음
        if (ImGui::GetIO().WantCaptureMouse) return;

        bool isClicked = InputManager::Instance().GetMouseState().leftClicked;

        // 현재 단계에 따라 로직을 분기
        switch (m_currentStep)
        {
        case TutorialStep::STEP_1_NOTICE_A:
        {
            // 마우스 클릭을 감지하면
            if (isClicked)
            {
                // 다음 단계로 상태를 변경
                m_currentStep = TutorialStep::STEP_1_NOTICE_B;
                // 다음 단계의 초기화 함수 호출
                EnterStep1_NoticeB();
            }
            break;
        }

        case TutorialStep::STEP_1_NOTICE_B:
        {
            // 여기에서 다음 단계로 넘어가는 조건을 체크합니다.
            // 예를 들어, 다시 한번 클릭하면 3단계로 넘어갈 수 있습니다.
            if (isClicked)
            {
                m_currentStep = TutorialStep::STEP_1_CLICKEMPTYGRID;
                EnterStep1_ClickEmptyGrid();
            }
            break;
        }

        case TutorialStep::STEP_1_CLICKEMPTYGRID:
            // 3단계 로직
            break;

            // ... 다른 단계들의 case 추가 ...

        default:
            break;
        }
    }

    void TutorialScene::EnterStep1_NoticeA()
    {
        if (m_narration)
            m_narration->SetText(L"\"의 새로운 집사로 취임한 것을 환영합니다.\"");
    }

    void TutorialScene::EnterStep1_NoticeB()
    {
        if (m_narration)
        {
            m_narration->SetText(
                L"\"당신은 건물을 지어 마을을 키우고 도시를 지켜내야 합니다.\"\n"
                L"먼저 빈 타일을 클릭하세요."
            );
        }
    }

    void TutorialScene::EnterStep1_ClickEmptyGrid()
    {
        if (m_fillter) m_fillter->SetActive(false);
    }

}
