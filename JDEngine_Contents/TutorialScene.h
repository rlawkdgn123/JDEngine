#pragma once
#include "pch.h"
#include "framework.h"
#include "SceneBase.h"
#include "InputManager.h" 
#include "UIObject.h"
#include "UI_ImageComponent.h"
#include "UI_ButtonComponent.h"
#include "ColliderBase.h"
#include "CameraFader.h"
#include "ParticleSystem.h"
#include "BuildSystem.h"
#include "ArmySystem.h"
#include "BoxCollider.h"
#include "Player.h"
#include "D2DRenderer.h"
#include "Camera.h"
using namespace JDGameObject::Content;
class GameTimer;

enum class TutorialStep
{
    // 1. 건물 배치
    STEP_1_NOTICE_A,            // "새로운 집사로 취임한 것을 환영합니다."
    STEP_1_NOTICE_B,            // "당신은 건물을 지어 마을을 키우고..."
    STEP_1_CLICKEMPTYGRID,      // 그리드 클릭
    STEP_1_BUILD,               // 주거지 건설
    STEP_1_CLEAR,               // "축하합니다. 당신의 도시로..."

    // 2. 고양이 배치 및 식량 생산
    STEP_2_NOTICE_A,            // "고양이들을 먹여 살리려면 식량이.."
    STEP_2_CLICKEMPTYGRID,      // 그리드 클릭
    STEP_2_BUILD,               // 낚시터 건설
    STEP_2_CLICKBUILDING,       // 낚시터 클릭
    STEP_2_SETWORKCAT,          // 일할 고양이 배치 ( 나비냥만 고를 수 있음 )
    STEP_2_CLEAR,               // 

    // << 2 => 예외사항 >>
    // 20초간 건물을 짓지 않아 물고기 수가 0 <= 50이 되었을 때,
    // 게임 오버 방지를 위해 시간 정지 "[튜토리얼] 낚시터 건설 대기 중" 문장 출력

    // 3. 고양이 교체 및 목재 생산
    STEP_3_NOTICE_A,            // "도시를 키우려면 더 많은 목재가 필요..."
    STEP_3_CLICKEDEMPTYGRID,    // 그리드 클릭
    STEP_3_BUILD,               // 제재소 건설
    STEP_3_CLICKBUILDING_A,     // 제재소 클릭
    STEP_3_SETWORKCAT_A,        // 일할 고양이 배치
    STEP_3_NOTICE_B,            // "이 때 펠리스냥으로 교체하면 목재 10%를 얻을 수 ..."
    STEP_3_CLICKBUILDING_B,     // 제재소 클릭
    STEP_3_SETWORKCAT_B,        // 펠리스 고양이 배치
    STEP_3_CLEAR,               // 

    // 4. 건물 업그레이드
    STEP_4_NOTICE_A,            // "건물을 새로 짓는 대신 건물 레벨을..."
    STEP_4_NOTICE_B,            // "낚시터를 레벨업해서 식량을 보충하세요"
    STEP_4_CLICKBUILDING,       // 낚시터 클릭
    STEP_4_LEVELUP,             // 낚시터 레벨업
    STEP_4_CLEAR,               //
    // << 4 -> 예외사항 >> 
    // 철거 or 레벨 다운 클릭 불가 처리

    // 5. 시너지 보너스
    STEP_5_NOTICE_A,            // "광산을 짓고 광물을 잘 캐는 코네 고양이를 배치하세요."
    STEP_5_CLICKEDEMPTYGRID,    // 그리드 클릭
    STEP_5_NOTICE_B,            // "각 고양이는 인접한 고양이에 따라 효율이 달라집니다."
    STEP_5_SETWORKCAT,          // 코네 고양이 배치
    STEP_5_CLEAR,               //

    // 6. 징병
    STEP_6_NOTICE_A,            // "조심하세요! 자원을 노리고 적들이 다가오는 중입니다."
    STEP_6_NOTICE_B,            // "적 부대의 수와 위치는 상단 바에서 확인 가능합니다.
    STEP_6_NOTICE_C,            // 성 입구의 병영을 클릭하여 병사를 모집할 수 있습니다.
    STEP_6_CLEAR,               //

    // 7. 전투
    STEP_7_NOTICE_A,            // "병영을 클릭 후 적을 클릭하여 요격 가능합니다."
    STEP_7_NOTICE_B,            // "적과 충돌 후 전투력을 비교하여 승패를 판정합니다."
    STEP_7_NOTICE_C,            // "적과 싸워 얻은 전리품으로 새로운 타일을 얻을 수 있습니다."
    STEP_7_CLEAR,               //

    // << 7 -> 예외사항 >>
    // 적이 특정 위치에 나타났는데도 요격을 하지 않을 시, 제자리걸음 처리, 시스템 내레이션 출력
    // ex) [튜토리얼] 숙련냥이 요격 대기중

    // 8. 원정
    STEP_8_NOTICE_A,            // "자원을 소모하여 적의 부대로 원정을 보낼 수 있습니다."
    STEP_8_NOTICE_B,            // "원정 후에는 약간의 보상과 원정 포인트를 얻을 수 있습니다."
    STEP_8_NOTICE_C,            // "포인트를 채우고 모든 공격을 막아내면 게임에서 승리합니다."
    STEP_8_CLEAR,               //

    COMPLETE                    // 튜토리얼 완료
};

namespace JDScene {

    using Image = JDGameObject::Content::Image;
    using Text = JDGameObject::Content::Text;
    using Button = JDGameObject::Content::Button;

    class TutorialScene : public SceneBase
    {
    public:
        TutorialScene(SceneType type, std::string id) : SceneBase(type, id) {}

        ~TutorialScene() override {}

        void OnEnter() override;

        void OnLeave() override;

        void Update(float deltaTime) override;

        void FixedUpdate(float fixedDeltaTime) override;

        void LateUpdate(float deltaTime) override;

        void Render(float deltaTime) override;

        void ProcessDayTimer(float deltaTime);
        void ProcessBattle(float deltaTime);

        void SpawnWaveEnemy(const Vector2F& pos);
        void SpawnPlayerArmy(const Vector2F& pos);
        void SpawnBattleObject(const Vector2F& pos);

        void MoveEnemyObjects(float deltaTime);
        void MovePlayerObjects(float deltaTime);
        void MoveExpedition(float deltaTime);

        void AttackWall(float deltaTime);

        void SafeDestroy(GameObjectBase* obj);

        void ClickUpdate();
        void CreateParticlesAtMousePosition(POINT mousePos);

        // 공통 생성 함수.
        GameObjectBase* CreateSoldierUnit(const JDGameSystem::UnitCounts& units, JDGlobal::Base::GameTag tag,
            JDGlobal::Contents::State state, const Vector2F& pos, const std::string& textureName); // 병사.

        GameObjectBase* CreateStructure(const std::wstring& name, JDGlobal::Base::GameTag tag,
            const Vector2F& pos, const std::string& textureName); // 구조물.

        GameObjectBase* CreateUIButton(const std::wstring& name, const Vector2F& pos,
            const std::string& textureName, const std::string& clickEventName, std::function<void()> callback); // 버튼.

        // 원정대 생성.
        void CreateExpedition();

        bool IsEnemySpawned(int day) const {
            return std::find(m_showedDays.begin(), m_showedDays.end(), day) != m_showedDays.end();
        }

        void AddEnemyDay(int day) {
            if (!IsEnemySpawned(day)) m_showedDays.push_back(day);
        }

        // 1. 건설 UI
        // void InitBuildMenu();
        // void ShowBuildMenu();
        // void CloseBuildMenu();

        // 건설 UI
        //void ShowBuildInfo(JDGlobal::Contents::BuildingType buildType, std::string costText, std::string effectText);
        //void CloseBuildInfo();
        //void ChangeBuildInfo(JDGlobal::Contents::BuildingType buildType, std::string costText, std::string effectText);

        // 자원 UI 업데이트
        void UpdateResourceUI();

        ////////////////////////////////////////////////////////////////////////////////
        // 1. 건설 UI
        void InitGridCreateMenu();
        void ShowGridCreateMenu();
        void CloseGridCreateMenu();

        void ShowGridCreateInfo(JDGlobal::Contents::BuildingType buildType, std::string costText, std::string effectText);
        void CloseGridCreateInfo();

        void ChangeGridCreateInfo(JDGlobal::Contents::BuildingType buildType, std::string costText, std::string effectText);

        ////////////////////////////////////////////////////////////////////////////////
        // 2. 업그레이드 UI
        void InitGridSettingMenu();
        void ShowGridSettingMenu();
        // void ShowUpgradeMenu();
        void CloseGridSettingMenu();

        // 업그레이드 UI 
        void ShowCatInfo(JDGlobal::Contents::CatType catType, std::string costText, std::string effectText);
        void CloseCatInfo();
        void ChangeUpgradeInfo(JDGlobal::Contents::CatType catType, std::string costText, std::string effectText);

        ////////////////////////////////////////////////////////////////////////////////
        // 3. 징병 UI
        void InitAwayMenu();
        void ShowAwayMenu();
        void CloseAwayMenu();

        // 징병 UI
        void ShowAwayPopup();
        void CloseAwayPopup();

        // 게임 맵을 생성합니다.
        void CreateGameMap();

        // 배속 버튼 관리
        void GameSpeedButtonUpdate();

    private:
        FMOD::Channel* bgmChannel = nullptr;
        FMOD::Channel* sfxChannel = nullptr;

        std::unique_ptr<ParticleSystem> m_lightParticles;
        Vector2F                        m_emitterPos;

        CameraFader  m_fader;

        // 전투 관련 변수.
        std::vector<bool> m_isOpen;
        std::vector<int> m_showedDays;

        std::shared_ptr<Camera> m_camera;

        JDGameSystem::ArmySystem m_playerArmy; // 병영.
        JDGameSystem::ArmySystem m_enemyArmy; // 해당 날짜에 등장하는 적.

        JDGameSystem::UnitCounts m_playerBattleArmy;
        JDGameSystem::UnitCounts m_enemyBattleArmy;
        int m_playerTotalPower = 0;
        int m_enemyTotalPower = 0;

        const float m_dayTime = 10.0f;
        float m_elapsedTime = 0.0f;
        const float m_battleTime = 2.0f;
        float m_btlElapsedTime = 0.0f;


        JDGameObject::GameObjectBase* m_targetEnemy = nullptr;       // 처음 선택된 적.
        JDGameObject::GameObjectBase* m_playerObject = nullptr;      // 처음 아군.
        JDGameObject::GameObjectBase* m_barracksObject = nullptr;    // 아군 병영.
        JDGameObject::GameObjectBase* m_battleObject = nullptr;      // 싸우는 애니메이션 보여줄 오브젝트.
        //JDGameObject::GameObjectBase* m_wallObject = nullptr;      // 아군 성벽.
        JDGameObject::GameObjectBase* m_expeditionObject = nullptr;  // 원정대.

        Vector2F m_wallPos = { -300.0f, 75.0f }; // 성벽위치.

        int m_currentWaypointIndex = 0;
        std::array<Vector2F, 3> m_waypoints = { Vector2F{ 255.0f, -135.0f },
                                                Vector2F{ 755.0f, -115.0f },
                                                Vector2F{ 1010.0f, -175.0f } }; // 원정대 경로.

        std::vector<Attacker> m_attackers;    // 현재 성벽을 공격 중인 적들
        int   m_wallHealth = 1000;            // 성벽 체력 초기값

        bool m_isBarracksSelected = false; // 병영 선택 여부.

        // 맵 생성 변수
        ////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr <BuildSystem> m_buildSystem;


        // 그리드
        int m_totalCols = 4;
        int m_totalRows = 6;
        // std::vector<bool> m_isOpen;

        std::vector<Button*> m_gridCreateButtons;
        std::vector<Button*> m_gridSettingButtons;
        bool m_GirdClicked;

        Image* m_Menu = nullptr;
        std::vector<Button*> m_menuButtons;
        Button* m_selectedTool = nullptr;
        JDComponent::ColliderBase* m_selectedCollider = nullptr;

        ////////////////////////////////////////////////////////////////////////////////

        // [상단] 자원
        Button* m_buttonPop = nullptr;           // 인구 UI
        Text* m_curPopText = nullptr;           // 인구 보유량
        Text* m_maxPopText = nullptr;           // 인구 최대치

        Button* m_buttonFood = nullptr;         // 음식 UI
        Text* m_curFoodText = nullptr;          // 음식 보유량
        Text* m_resFoodText = nullptr;          // 음식 재생량

        Button* m_buttonWood = nullptr;         // 목재 UI
        Text* m_curWoodText = nullptr;          // 목재 보유량
        Text* m_resWoodText = nullptr;          // 목재 재생량

        Button* m_buttonMineral = nullptr;      // 광물 UI
        Text* m_curMineralText = nullptr;       // 광물 보유량
        Text* m_resMineralText = nullptr;       // 광물 재생량

        ////////////////////////////////////////////////////////////////////////////////

        // [상단] 몬스터 웨이브
        Image* m_monsterWaveBackground = nullptr;

        ////////////////////////////////////////////////////////////////////////////////

        // [상단] 날짜 패널
        Image* m_datePanel = nullptr;
        Text* m_yearText = nullptr;
        Text* m_monthText = nullptr;
        Text* m_dayText = nullptr;

        Button* m_stopButton = nullptr;         // 일시정지 버튼
        Button* m_playButton = nullptr;         // 재생 버튼
        Button* m_speedButton = nullptr;        // 배속 버튼
        Button* m_optionButton = nullptr;       // 옵션 버튼

        ////////////////////////////////////////////////////////////////////////////////

        // 원정 이미지
        Image* m_away = nullptr;
        Image* m_awayGauge = nullptr;

        ////////////////////////////////////////////////////////////////////////////////

        // 그리드와 상호작용 중 인지 확인하기 위한 플래그
        bool isGridBuild = false;
        bool isGridSetting = false;
        bool isAway = false;

        // [하단] 건물 상호작용 UI
        Image* m_defaultUI = nullptr;           // 0. 기본 UI
        Image* m_buildUI = nullptr;             // 1. 건설 UI
        Image* m_upgradeUI = nullptr;           // 2. 업그레이드 및 고양이 배치 UI
        Image* m_awayUI = nullptr;              // 3. 징병 및 원정 UI

        ////////////////////////////////////////////////////////////////////////////////

        // 0. 기본 UI

        // 1. 건설 UI ( 주거지, 낚시터, 제재소, 광산, (연구실) )
        Button* m_buildHouse = nullptr;
        Button* m_buildFishingspot = nullptr;
        Button* m_buildRumbermill = nullptr;
        Button* m_buildMine = nullptr;
        // Button* m_buildLab = nullptr;

        // 1. 건설 Info
        Text* m_buildTypeText = nullptr;

        Text* m_costInfoText = nullptr;
        Text* m_costText = nullptr;
        Image* m_costImage = nullptr;

        Text* m_effectInfoText = nullptr;
        Text* m_effectText = nullptr;
        Image* m_effctImage = nullptr;

        ////////////////////////////////////////////////////////////////////////////////

        // 2. 업그레이드 Info
        // 고양이 배치
        Text* m_catTypeText = nullptr;

        Text* m_catCostInfoText = nullptr;
        Text* m_catCostText = nullptr;
        Image* m_catCostImage = nullptr;

        Text* m_catEffectInfoText = nullptr;
        Text* m_catEffectText = nullptr;
        Image* m_catEffctImage = nullptr;

        // 고양이 선택 버튼
        Button* m_naviSetButton = nullptr;
        Button* m_felisSetButton = nullptr;
        Button* m_koneSetButton = nullptr;

        // 업그레이드 건물 정보
        Text* m_builtTypeText = nullptr;

        Text* m_upgradeCostInfoText = nullptr;
        Text* m_upgradeCostText = nullptr;
        Image* m_upgradeCostImage = nullptr;

        Text* m_upgradeEffectInfoText = nullptr;
        Text* m_upgradeEffectText = nullptr;
        Image* m_upgradeEffctImage = nullptr;

        Button* m_downgradeButton = nullptr;
        Button* m_upgradeButton = nullptr;

        ////////////////////////////////////////////////////////////////////////////////

        // 3. 징병 & 원정 Info

        /////
        // 견습 냥이
        Button* m_trainerCatButton = nullptr;
        Text* m_trainerCatName = nullptr;

        Text* m_trainerCatCostInfo = nullptr;
        Image* m_trainerCatCostImage01 = nullptr;
        Text* m_trainerCatCostText01 = nullptr;
        Image* m_trainerCatCostImage02 = nullptr;
        Text* m_trainerCatCostText02 = nullptr;

        Text* m_trainerCatRecruitInfo = nullptr;
        Text* m_trainerCatRecruitText = nullptr;

        Text* m_trainerCatPowerInfo = nullptr;
        Text* m_trainerCatPowerText = nullptr;

        /////
        // 숙련 냥이
        Button* m_expertCatButton = nullptr;
        Text* m_expertCatName = nullptr;

        Text* m_expertCatCostInfo = nullptr;
        Image* m_expertCatCostImage01 = nullptr;
        Text* m_expertCatCostText01 = nullptr;
        Image* m_expertCatCostImage02 = nullptr;
        Text* m_expertCatCostText02 = nullptr;

        Text* m_expertCatRecruitInfo = nullptr;
        Text* m_expertCatRecruitText = nullptr;

        Text* m_expertCatPowerInfo = nullptr;
        Text* m_expertCatPowerText = nullptr;

        /////
        // 원정 보내기
        Text* m_awayInfo = nullptr;

        Button* m_awayBeginner = nullptr;           // 초급
        Button* m_awayIntermediate = nullptr;       // 중급
        Button* m_awayAdvanced = nullptr;           // 상급

        /////
        // 원정 팝업
        Image* m_awayPopupUI = nullptr;
        Text* m_awayPopupInfo = nullptr;

        Text* m_awayCostInfo = nullptr;
        Image* m_awayCostImage01 = nullptr;
        Text* m_awayCostText01 = nullptr;
        Image* m_awayCostImage02 = nullptr;
        Text* m_awayCostText02 = nullptr;

        Text* m_awayAwardInfo = nullptr;
        Text* m_awayAwardText01 = nullptr;
        Text* m_awayAwardText02 = nullptr;

        Button* m_awayButton = nullptr;


        ////////////////////////////////////////////////////////////////////////////////

        // 필터, 나래이션
        Image* m_fillter = nullptr;
        Text* m_narration = nullptr;

        ////////////////////////////////////////////////////////////////////////////////

        // 현재 튜토리얼 진행 단계를 저장하는 변수
        TutorialStep m_currentStep;

        // 튜토리얼 로직을 별도로 처리할 함수
        void UpdateTutorial(float deltaTime);

        ////////////////////////////////////////////////////////////////////////////////

        // 각 튜토리얼 단계 시작 시 호출될 함수들

        // ==========================================================
        // 1. 건물 배치
        // ==========================================================
        void EnterStep1_NoticeA();          // "새로운 집사로 취임한 것을 환영합니다."
        void EnterStep1_NoticeB();          // "당신은 건물을 지어 마을을 키우고..."
        void EnterStep1_ClickEmptyGrid();   // 그리드 클릭 유도
        void EnterStep1_Build();            // 주거지 건설 유도
        void EnterStep1_Clear();            // "축하합니다. 당신의 도시로..."

        // ==========================================================
        // 2. 고양이 배치 및 식량 생산
        // ==========================================================
        void EnterStep2_NoticeA();          // "고양이들을 먹여 살리려면 식량이.."
        void EnterStep2_ClickEmptyGrid();   // 그리드 클릭 유도
        void EnterStep2_Build();            // 낚시터 건설 유도
        void EnterStep2_ClickBuilding();    // 낚시터 클릭 유도
        void EnterStep2_SetWorkCat();       // 나비냥 배치 유도
        void EnterStep2_Clear();            // 2단계 완료

        // ==========================================================
        // 3. 고양이 교체 및 목재 생산
        // ==========================================================
        void EnterStep3_NoticeA();              // "도시를 키우려면 더 많은 목재가 필요..."
        void EnterStep3_ClickEmptyGrid();       // 그리드 클릭 유도
        void EnterStep3_Build();                // 제재소 건설 유도
        void EnterStep3_ClickBuildingA();       // 제재소 클릭 유도 (첫 번째)
        void EnterStep3_SetWorkCatA();          // 일할 고양이 배치 유도 (첫 번째)
        void EnterStep3_NoticeB();              // "이 때 펠리스냥으로 교체하면..."
        void EnterStep3_ClickBuildingB();       // 제재소 클릭 유도 (두 번째)
        void EnterStep3_SetWorkCatB();          // 펠리스 고양이 배치 유도
        void EnterStep3_Clear();                // 3단계 완료

        // ==========================================================
        // 4. 건물 업그레이드
        // ==========================================================
        void EnterStep4_NoticeA();          // "건물을 새로 짓는 대신 건물 레벨을..."
        void EnterStep4_NoticeB();          // "낚시터를 레벨업해서 식량을 보충하세요"
        void EnterStep4_ClickBuilding();    // 낚시터 클릭 유도
        void EnterStep4_LevelUp();          // 낚시터 레벨업 유도
        void EnterStep4_Clear();            // 4단계 완료

        // ==========================================================
        // 5. 시너지 보너스
        // ==========================================================
        void EnterStep5_NoticeA();          // "광산을 짓고 광물을 잘 캐는 코네 고양이를..."
        void EnterStep5_ClickEmptyGrid();   // 그리드 클릭 유도
        void EnterStep5_NoticeB();          // "각 고양이는 인접한 고양이에 따라..."
        void EnterStep5_SetWorkCat();       // 코네 고양이 배치 유도
        void EnterStep5_Clear();            // 5단계 완료

        // ==========================================================
        // 6. 징병
        // ==========================================================
        void EnterStep6_NoticeA();          // "조심하세요! 자원을 노리고 적들이..."
        void EnterStep6_NoticeB();          // "적 부대의 수와 위치는 상단 바에서..."
        void EnterStep6_NoticeC();          // "성 입구의 병영을 클릭하여..."
        void EnterStep6_Clear();            // 6단계 완료

        // ==========================================================
        // 7. 전투
        // ==========================================================
        void EnterStep7_NoticeA();          // "병영을 클릭 후 적을 클릭하여..."
        void EnterStep7_NoticeB();          // "적과 충돌 후 전투력을 비교하여..."
        void EnterStep7_NoticeC();          // "적과 싸워 얻은 전리품으로..."
        void EnterStep7_Clear();            // 7단계 완료

        // ==========================================================
        // 8. 원정
        // ==========================================================
        void EnterStep8_NoticeA();          // "자원을 소모하여 적의 부대로..."
        void EnterStep8_NoticeB();          // "원정 후에는 약간의 보상과..."
        void EnterStep8_NoticeC();          // "포인트를 채우고 모든 공격을 막아내면..."
        void EnterStep8_Clear();            // 8단계 완료

        // ==========================================================
        // 튜토리얼 완료
        // ==========================================================
        void EnterComplete();               // 튜토리얼 완료 처리

        /////////////////////////////////////////////////////////////////////////////////////
        std::mt19937_64 rng{ std::random_device{}() };
        //파티클
        std::unique_ptr<ParticleSystem> m_mouseParticles;
        std::unique_ptr<ParticleSystem> m_leafParticles;
        std::unique_ptr<ParticleSystem> m_smokeParticles;
        std::unique_ptr<ParticleSystem> m_footdustParticles;
        std::unique_ptr<ParticleSystem> m_waveParticles;

        float m_torchSmokeAcc = 0.f;
        bool  m_emitTorchSmoke = false;
        float m_torchSmokePeriod = 0.2f;

        bool  m_emitleaf = false;
        bool  m_emitwave = false;
        //------------------------------------------------
        struct Actor {
            JDGameObject::GameObject* go = nullptr;
            JDComponent::TextureRenderer* tex = nullptr;
            JDComponent::AnimationRender* anim = nullptr;
            JDComponent::SFX* sfx = nullptr;
            JDComponent::BoxCollider* col = nullptr;

            // 상태 공통
            float timer = 0.f;        // 애니 끝나면 텍스쳐로 돌아갈 때 사용
            float lockTimer = 0.f;    // 락 해제까지 시간
            bool  changed = false;    // 애니->텍스쳐 복귀 대기 중
            bool  locked = false;    // 클릭 무시 락

            // 편의
            Vector2F emitOffset{ 0,0 }; // 파티클/효과 발사 위치 보정
        };

        static D2D1_SIZE_F GetAnimFirstFrameSize(const char* animKey) {
            auto* clip = AssetManager::Instance().GetAnimationRender(animKey);
            if (!clip || clip->frames.empty()) return { 0,0 };
            const auto& fr = clip->frames[0];

            float w = fr.srcRect.right - fr.srcRect.left;
            float h = fr.srcRect.bottom - fr.srcRect.top;
            return { w, h };

        }
        struct Torch {
            Actor actor;
            bool  emitSmoke = false;
            float period = 0.2f; // 토치별 스폰 주기
            float acc = 0.f;  // 토치별 누적 시간
        };

        void updateTorchSmoke(Torch& t, ParticleSystem* smokePS, float dt, float /*W*/, float /*H*/)
        {
            if (!smokePS) return;

            if (t.emitSmoke) {
                t.acc += dt;
                if (auto* tr = t.actor.go->GetComponent<JDComponent::Transform>()) {
                    Vector2F p = tr->GetPosition() + t.actor.emitOffset; // 예: {0, -126}
                    while (t.acc >= t.period) {
                        smokePS->SpawnSmokeParticle(p, /*count=*/1, /*baseScale=*/1.6f, /*maxLife=*/2.2f);
                        t.acc -= t.period;
                    }
                }
            }
            else {
                t.acc = 0.f;
            }
        }

        std::vector<Actor> frogs;
        std::vector<Actor> rats;
        std::vector<Torch> torches;
        float hx = JDGlobal::Window::WindowSize::Instance().GetHeight() / 2;
        float hy = JDGlobal::Window::WindowSize::Instance().GetWidth() / 2;

        //생성 헬퍼
        Actor makeActor(const wchar_t* name,
            const Vector2F& pos,
            const char* texKey,
            const char* animKey,
            const char* sfxKey = nullptr,
            Vector2F emitOffset = { 0,0 })
        {
            using namespace JDGameObject;
            using namespace JDComponent;

            auto* go = CreateGameObject<Player>(name);
            auto* tr = go->GetComponent<Transform>();
            tr->SetPosition(pos);
            //tr->SetPivot(Vector2F{ 0.5f, 0.5f });
            go->AddComponent<Editor_Clickable>();

            auto* tex = go->AddComponent<TextureRenderer>(texKey, RenderLayerInfo{ SortingLayer::BackGround, 1 });
            if (tex) tex->SetEnabled(true);

            auto* anim = go->AddComponent<AnimationRender>(animKey, 1, RenderLayerInfo{ SortingLayer::BackGround, 1 });
            if (anim) anim->SetEnabled(false);

            // 콜라이더는 텍스쳐 사이즈 기준
            auto bmp = static_cast<ID2D1Bitmap*>(AssetManager::Instance().GetTexture(texKey));
            auto* col = (bmp ? go->AddComponent<BoxCollider>(Vector2F{ bmp->GetSize().width / 2.f, bmp->GetSize().height / 2.f }) : nullptr);

            if (anim && tex) {
                D2D1_SIZE_F animFirst = GetAnimFirstFrameSize(animKey);
                if (animFirst.width > 0 && animFirst.height > 0) {
                    tex->SetSize(animFirst);                      // ← 핵심 한 줄
                    if (col) col->SetHalfSize({ animFirst.width * 0.5f, animFirst.height * 0.5f }); // 충돌도 맞추고 싶다면
                }
            }
            JDComponent::SFX* sfx = nullptr;
            if (sfxKey) {
                sfx = go->AddComponent<SFX>(sfxKey);
                if (sfx) {
                    sfx->SetEnabled(true);
                    sfx->SetLoop(false);
                    sfx->SetPlayOnMove(false);   // ← 요게 핵심
                    sfx->SetVolume(1.0f);        // 엔진에 있으면 안전하게
                }
            }

            Actor a; a.go = go; a.tex = tex; a.anim = anim; a.sfx = sfx; a.col = col; a.emitOffset = { emitOffset };
            return a;
        }

        //공통 동작 헬퍼
        inline bool isClicked(const Actor& a, const Vector2F& worldPos) {
            if (!a.col) return false;
            return a.col->IsMouseOver(worldPos);
        }

        inline void toTexture(Actor& a) {
            if (a.anim) a.anim->SetEnabled(false);
            if (a.tex)  a.tex->SetEnabled(true);
            a.changed = a.locked = false; a.timer = a.lockTimer = 0.f;
        }

        inline void playOnce(Actor& a, const char* clip, bool hideTexture = true) {
            if (hideTexture && a.tex) a.tex->SetEnabled(false);
            if (a.anim) {
                a.anim->SetClipName(clip);
                a.anim->SetEnabled(true);
                a.anim->PlayOnceFromStart();
            }
        }

        inline Torch makeTorch(const wchar_t* name,
            const Vector2F& pos,
            const char* texKey,
            const char* animKey,
            const char* sfxKey,
            Vector2F emitOffset,
            float periodSec = 0.2f)
        {
            Torch t;
            t.actor = makeActor(name, pos, texKey, animKey, sfxKey, emitOffset);
            t.period = periodSec;
            t.emitSmoke = false;
            t.acc = 0.f;
            if (t.actor.sfx) t.actor.sfx->SetLoop(true);
            return t;
        }

        void onClickFrog(Actor& f, ParticleSystem* wavePS, std::mt19937_64& rng, const Vector2F& clickPos) {
            if (f.locked) return;

            std::uniform_real_distribution<float> d01(0.f, 1.f);
            bool useAnim1 = (d01(rng) < 0.2f);
            const char* clip = useAnim1 ? "Frog1" : "Frog2";

            playOnce(f, clip);
            if (f.sfx) {
                f.sfx->Stop();
                f.sfx->SetClipName(useAnim1 ? "FrogPop" : "FrogSound");
                f.sfx->SetEnabled(true);
                f.sfx->SetPlayOnMove(false);
                f.sfx->SetVolume(1.0f);
                f.sfx->Play();

                std::cout << "[Frog] clip=" << (useAnim1 ? "FrogPop" : "FrogSound")
                    << " enabled=" << f.sfx->GetEnabled()
                    << " vol=" << f.sfx->GetVolume()
                    << " playing=" << f.sfx->IsPlaying() << "\n";
            }

            if (useAnim1) {
                std::uniform_real_distribution<float> dLock(3.f, 5.f);
                f.lockTimer = dLock(rng);
                f.locked = true;
            }
            else {
                // Frog2 길이 계산 후 복귀 타이머
                float dur = 1.f;
                if (auto* clipData = AssetManager::Instance().GetAnimationRender("Frog2")) {
                    dur = 0.f; for (auto& fr : clipData->frames) dur += fr.duration; if (dur <= 0.f) dur = 1.f;
                }
                f.timer = dur; f.changed = true;
            }

            ////////////////////
            // 자체적으로 좌표 계산을 막고 ClickUpdate에서 계산한 월드좌표를 인자로 직접 전달하도록 수정

            //// 물파동 ( 클릭 위치 )
            //if (wavePS) {
            //    // 개구리의 Transform 컴포넌트를 가져올 필요가 없습니다.
            //    // auto* tr = f.go->GetComponent<JDComponent::Transform>();

            //    // 바로 함수로 전달받은 '클릭 위치'를 사용합니다.
            //    wavePS->SpawnWaterWaveParticle({ clickPos.x, clickPos.y }, 0.35f, 1.6f, 0.8f, 64.f);
            //}

            // 물파동 ( 오브젝트 위치 )
            if (wavePS) {
                // 1. 클릭된 개구리(f)의 Transform 컴포넌트를 가져옵니다.
                auto* tr = f.go->GetComponent<JDComponent::Transform>();
                if (tr)
                {
                    // 2. Transform에서 개구리의 월드 좌표를 가져옵니다.
                    Vector2F spawnPos = tr->GetPosition();

                    // (선택사항) 필요하다면 오프셋을 더해줄 수 있습니다.
                    // spawnPos += f.emitOffset;

                    // 3. 클릭 위치(clickPos)가 아닌, '개구리의 위치(spawnPos)'에 파티클을 생성합니다.
                    wavePS->SpawnWaterWaveParticle({ spawnPos.x, spawnPos.y }, 0.35f, 1.6f, 0.8f, 64.f);
                }
            }
        }

        void onClickRat(Actor& r, ParticleSystem* leafPS, std::mt19937_64& rng, const Vector2F& clickPos) {
            if (r.locked) return;

            playOnce(r, "Rat");
            if (r.sfx) r.sfx->Play();

            std::uniform_real_distribution<float> dLock(2.f, 3.f);
            r.lockTimer = dLock(rng);
            r.locked = true;

            if (leafPS) {
                auto* tr = r.go->GetComponent<JDComponent::Transform>();
                if (tr)
                {
                    // 2. Transform에서 쥐의 월드 좌표를 가져옵니다.
                    Vector2F spawnPos = tr->GetPosition();

                    // (선택사항) 필요하다면 오프셋을 더해줄 수 있습니다.
                    // spawnPos += f.emitOffset;

                    // 3. 클릭 위치(clickPos)가 아닌, '개구리의 위치(spawnPos)'에 파티클을 생성합니다.
                    leafPS->SpawnLeavesParticle({ spawnPos.x, spawnPos.y }, 6, 1.0f, 4.0f);
                }
            }
        }

        void toggleTorch(Torch& t) {
            auto& a = t.actor;
            if (a.anim && a.anim->GetEnabled()) {
                toTexture(a);
                if (a.sfx) { a.sfx->Stop(); a.sfx->SetEnabled(false); }
                t.emitSmoke = false;
                t.acc = 0.f;
            }
            else {
                if (a.tex) a.tex->SetEnabled(false);
                if (a.anim) { a.anim->SetEnabled(true); a.anim->PlayOnceThenLoopRange(1, UINT_MAX); }
                if (a.sfx) { a.sfx->SetEnabled(true); a.sfx->SetLoop(true); if (!a.sfx->IsPlaying()) a.sfx->Play(); }
                t.emitSmoke = true;
            }
        }

        void updateActor(Actor& a, float dt) {
            if (a.changed && !a.locked) {
                a.timer -= dt;
                if (a.timer <= 0.f) toTexture(a);
            }
            if (a.locked) {
                a.lockTimer -= dt;
                if (a.lockTimer <= 0.f) toTexture(a);
            }
        }

        inline void emitTorchSmoke(Torch& t, ParticleSystem* smokePS, float dt) {
            if (!smokePS) return;
            if (!t.emitSmoke) { t.acc = 0.f; return; }

            t.acc += dt;
            if (auto* tr = t.actor.go->GetComponent<JDComponent::Transform>()) {
                auto p = tr->GetPosition() + t.actor.emitOffset; // 예: {0,-126}
                while (t.acc >= t.period) {
                    smokePS->SpawnSmokeParticle(p, 1, 1.6f, 2.2f); // 필요하면 여기서 count 조절
                    t.acc -= t.period;
                }
            }
        }
    };
}