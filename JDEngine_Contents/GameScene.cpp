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

        // 자원 세팅.
        auto& rs = ResourceSystem::Instance();
        rs.SetTotalResource(Resource(10000, 10000, 10000));
        rs.SetMaxPopulation(500);
        rs.SetCurPopulation(100);

        // 징병 버튼.
        //auto noviceBtn = CreateUIObject<Button>(L"noviceBtn");
        //noviceBtn->SetTextureName("GAME_START_B");
        //noviceBtn->SetSizeToOriginal();
        //noviceBtn->SetPosition({ 100.0f, 50.0f });
        //noviceBtn->AddOnClick("Recruit Novice", [this]() {
        //    player.RecruitUnits(JDGameSystem::UnitType::Novice);
        //    std::cout << "Player Power: " << player.CalculateTotalPower() << std::endl;
        //    });

        //auto expertBtn = CreateUIObject<Button>(L"expertBtn");
        //expertBtn->SetTextureName("GAME_START_B");
        //expertBtn->SetSizeToOriginal();
        //expertBtn->SetPosition({ 100.0f, 150.0f });
        //expertBtn->AddOnClick("Recruit Expert", [this]() {
        //    player.RecruitUnits(JDGameSystem::UnitType::Expert);
        //    std::cout << "Player Power: " << player.CalculateTotalPower() << std::endl;
        //    });

        { // 병영.
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
        }
        

        { // 성벽.
            auto* wallObj = CreateGameObject<Player>(L"wallObj");

            wallObj->SetTag(JDGlobal::Base::GameTag::Wall);
            wallObj->AddComponent<Editor_Clickable>();
            wallObj->AddComponent<JDComponent::TextureRenderer>("Test", RenderLayerInfo{ SortingLayer::BackGround, 1 });
            auto bitmap = static_cast<ID2D1Bitmap*> (AssetManager::Instance().GetTexture("Test"));
            auto size = bitmap->GetSize();
            wallObj->AddComponent<JDComponent::BoxCollider>(Vector2F{ size.width / 2.0f, size.height / 2.0f });
            wallObj->GetComponent<JDComponent::BoxCollider>()->SetOpen(true);
            wallObj->GetComponent<Transform>()->SetPosition({ -800.0f, 100.0f });

            m_wallObject = wallObj;
        }
        

        //======================================================
    }

    void GameScene::OnLeave() {
        //cout << "[GameScene] OnLeave()\n";
    }

    void GameScene::Update(float deltaTime) {
        SceneBase::Update(deltaTime);

        // 시간 증가 타이머.
        m_elapsedTime += deltaTime;
        if (m_battleObject) m_btlElapsedTime += deltaTime;

        if (m_elapsedTime >= m_dayTime) {
            m_elapsedTime = 0.0f;
            WaveManager::Instance().AdvanceDay();

            const int currDay = WaveManager::Instance().GetCurrDay();
            const int warningDay = currDay + 2;

            std::cout << "[GameScene] 날짜 증가: " << currDay << std::endl;

            // 웨이브가 있는 경우, 아직 화면에 나타나지 않았으면 생성.
            if (WaveManager::Instance().GetWave(warningDay) && !IsEnemySpawned(warningDay)) {
                float windowWidth = JDGlobal::Window::WindowSize::Instance().GetWidth();
                SpawnWaveEnemy({(windowWidth + 100.0F) / 2.0f, 100.0f });
                AddEnemyDay(warningDay);
                std::cout << "[GameScene] 적 스폰됨." << std::endl;
            }
        }

        if (m_btlElapsedTime >= m_battleTime) {
            m_btlElapsedTime = 0.0f;
            SafeDestroy(m_battleObject);
            std::cout << "[GameScene] 전투 끝." << std::endl;
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

    void GameScene::SpawnWaveEnemy(const Vector2F& pos) {

        auto* enemyObj = CreateGameObject<Player>(L"enemyObj");

        enemyObj->SetTag(JDGlobal::Base::GameTag::Enemy);
        enemyObj->SetState(JDGlobal::Contents::State::Move);
        enemyObj->AddComponent<Editor_Clickable>();
        enemyObj->AddComponent<JDComponent::TextureRenderer>("Test", RenderLayerInfo{ SortingLayer::BackGround, 1 });

        auto bitmap = static_cast<ID2D1Bitmap*> (AssetManager::Instance().GetTexture("Test"));
        auto size = bitmap->GetSize();

        enemyObj->AddComponent<JDComponent::BoxCollider>(Vector2F{ size.width / 2.0f, size.height / 2.0f });
        enemyObj->GetComponent<JDComponent::BoxCollider>()->SetOpen(true);
        float windowWidth = JDGlobal::Window::WindowSize::Instance().GetWidth();
        //enemyObj->GetComponent<Transform>()->SetPosition({ (windowWidth + size.width) / 2.0f, 100.0f });
        enemyObj->GetComponent<Transform>()->SetPosition(pos);
        enemyObj->AddComponent<JDComponent::SFX>("Step");
    }

    void GameScene::SpawnPlayerArmy(const Vector2F& pos) {
        auto* playerObj = CreateGameObject<Player>(L"playerObj");
        playerObj->SetTag(JDGlobal::Base::GameTag::Player);
        playerObj->SetState(JDGlobal::Contents::State::Move);
        playerObj->AddComponent<JDComponent::TextureRenderer>("Test", RenderLayerInfo{ SortingLayer::BackGround, 1 });

        auto bitmap = static_cast<ID2D1Bitmap*> (AssetManager::Instance().GetTexture("Test"));
        auto size = bitmap->GetSize();

        playerObj->AddComponent<JDComponent::BoxCollider>(Vector2F{ size.width / 2.0f, size.height / 2.0f });
        playerObj->GetComponent<JDComponent::BoxCollider>()->SetOpen(true);
        //playerObj->GetComponent<Transform>()->SetPosition({ -400.0f, 100.0f });
        playerObj->GetComponent<Transform>()->SetPosition(pos);
        playerObj->AddComponent<JDComponent::SFX>("Step");

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
                    SafeDestroy(objPtr.get());
                    m_btlElapsedTime = 0.0f;
                    continue;
                }
            }

            // 성벽이랑 충돌하면 멈춤.
            if (m_wallObject) {
                auto* wallTm = m_wallObject->GetComponent<Transform>();
                if (!wallTm)  continue;
                Vector2F diffPos = wallTm->GetPosition() - transform->GetPosition();

                if (diffPos.Length() <= 10.0f) {
                    objPtr->SetState(JDGlobal::Contents::State::Idle);
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

                    SafeDestroy(m_playerObject);
                    SafeDestroy(m_targetEnemy);

                    SpawnBattleObject(mid);
                    return;
                }
            }

            // 전투 오브젝트가 없고, 일반 아군이면 후퇴 상태로 전환
            if (!m_battleObject && objPtr.get() != m_playerObject) {
                if (!m_barracksObject) continue;
                auto* barracksTm = m_barracksObject->GetComponent<Transform>();
                if (!barracksTm) continue;
                Vector2F diffPos = barracksTm->GetPosition() - transform->GetPosition();

                if (diffPos.Length() <= 10.0f) {
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
                        else if (m_isBarracksSelected && !m_battleObject && (isEnemyTag && !m_targetEnemy)) {
                            m_targetEnemy = gameObj;
                            SpawnPlayerArmy({ -400.0f, 100.0f });
                            m_isBarracksSelected = false;
                            std::cout << "[GameScene] 병영 클릭 -> 적 클릭." << std::endl;
                        }
                        else if (m_isBarracksSelected && tag == JDGlobal::Base::GameTag::BattleAnim) {
                            SpawnPlayerArmy({ -400.0f, 100.0f });
                            m_isBarracksSelected = false;
                            std::cout << "[GameScene] 병영 클릭 -> 전투중 클릭." << std::endl;
                        }
                        else {
                            m_isBarracksSelected = false; 
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

        if (m_playerObject == obj) m_playerObject = nullptr;
        if (m_targetEnemy == obj) m_targetEnemy = nullptr;
        if (m_battleObject == obj) m_battleObject = nullptr;
        if (GetSelectedObject() == obj) SetSelectedObject(nullptr);

        SceneBase::DestroyObject(obj);
    }
}
