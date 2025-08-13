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
#include "WaveManager.h"

using namespace JDGameObject::Content;
class GameTimer;

namespace JDScene {
    struct Attacker {
        JDGameObject::GameObjectBase* enemy;  // 공격 대상 적
        float timer;                          // 공격 타이머 누적
    };

    using Image = JDGameObject::Content::Image;
    using Text = JDGameObject::Content::Text;
    using Button = JDGameObject::Content::Button;

    class GameScene : public SceneBase
    {
    public:
        GameScene(SceneType type, std::string id) : SceneBase(type, id) {}

        ~GameScene() override {}

        void OnEnter() override;

        void OnLeave() override;

        void Update(float deltaTime) override;

        void FixedUpdate(float fixedDeltaTime) override;

        void LateUpdate(float deltaTime) override;

        void Render(float deltaTime) override;

        void ProcessDayTimer(float deltaTime);
        void ProcessBattle(float deltaTime);
        void BattleReward();

        void SpawnWaveEnemy(const Vector2F& pos);
        void SpawnPlayerArmy(const Vector2F& pos);
        void SpawnBattleObject(const Vector2F& pos);

        void MoveEnemyObjects(float deltaTime);
        void MovePlayerObjects(float deltaTime);
        void MoveExpedition(float deltaTime);

        void AttackWall(float deltaTime);

        void SafeDestroy(GameObjectBase* obj);

        void ClickUpdate();

        // 공통 생성 함수.
        GameObjectBase* CreateSoldierUnit(const JDGameSystem::UnitCounts& units, JDGlobal::Base::GameTag tag,
            JDGlobal::Contents::State state, const Vector2F& pos, const std::string& textureName); // 병사.

        GameObjectBase* CreateStructure(const std::wstring& name, JDGlobal::Base::GameTag tag,
            const Vector2F& pos, const std::string& textureName); // 구조물.

        //GameObjectBase* CreateUIButton(const std::wstring& name, const Vector2F& pos,
        //    const std::string& textureName, const std::string& clickEventName, std::function<void()> callback); // 버튼.

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

        // 날짜 UI
        void DateUIUpdate();

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

        const float m_dayTime = 1.0f;
        float m_elapsedTime = 0.0f;
        const float m_battleTime = 2.0f;
        float m_btlElapsedTime = 0.0f;

        JDGameObject::GameObjectBase* m_targetEnemy = nullptr;       // 처음 선택된 적.
        JDGameObject::GameObjectBase* m_playerObject = nullptr;      // 처음 아군.
        JDGameObject::GameObjectBase* m_barracksObject = nullptr;    // 아군 병영.
        JDGameObject::GameObjectBase* m_battleObject = nullptr;      // 싸우는 애니메이션 보여줄 오브젝트.
        //JDGameObject::GameObjectBase* m_wallObject = nullptr;      // 아군 성벽.
        JDGameObject::GameObjectBase* m_expeditionObject = nullptr;  // 원정대.

        Vector2F m_wallPos = { -90.0f, 75.0f }; // 성벽위치.

        int m_currentWaypointIndex = 0;
        std::array<Vector2F, 3> m_waypoints = { Vector2F{ 255.0f, -135.0f },
                                                Vector2F{ 755.0f, -115.0f },
                                                Vector2F{ 1010.0f, -175.0f } }; // 원정대 경로.

        std::vector<Attacker> m_attackers;    // 현재 성벽을 공격 중인 적들
        int m_wallHealth = 999;              // 성벽 현재 체력

        bool m_isBarracksSelected = false; // 병영 선택 여부.

        GameDate m_date; // 현재 날짜.

        // 웨이브 표시
        struct NextWaveIndicator { // 웨이브 아이콘 표시
            Image* waveIcon = nullptr;
            Text*  powerText = nullptr;
            int    stepsMoved = 0;   // 이동한 횟수.
            int    srcDay = 0;       // 생성 시점 날짜.
            int    lastMovedDay = 0; // 마지막으로 위치 반영한 날짜.
        };


        void SpawnNextWaveIndicator(int wavePower);
        void AdvanceNextWaveIndicators(); // 현재 날짜 기준으로 nextMoveDay를 지난 것만 이동
        std::vector<NextWaveIndicator> m_nextWaveIndicators;

        //엔딩 관련 함수.
        bool CheckEnding(); // 기본 엔딩 조건.
        bool ResolveGameEnding(); // 엔딩 분기 처리.

        ///////////////////////////////////////////////////////////////////////////////
        // 전투 아군 적군 관리용. 추가 부분.
       
        std::vector<JDGameObject::GameObjectBase*> m_players;
        std::vector<JDGameObject::GameObjectBase*> m_enemies;

        void RegisterUnit(JDGlobal::Base::GameTag tag, JDGameObject::GameObjectBase* obj);
        void UnregisterUnit(JDGameObject::GameObjectBase* obj);

        struct Attachment {
            JDGameObject::GameObjectBase* follower{ nullptr };
            JDGameObject::GameObjectBase* host{ nullptr };
            Vector2F offset{ 0,0 };
        };
        std::vector<Attachment> m_attachments;

        JDGameObject::GameObjectBase* AttachObject(JDGameObject::GameObjectBase* follower,
            JDGameObject::GameObjectBase* host,
            Vector2F offset);
        void UpdateAttachments(float dt);
        void DetachByHost(JDGameObject::GameObjectBase* host);

        JDGameObject::GameObjectBase* CreateHeadMarker(JDGameObject::GameObjectBase* host);
        ///////////////////////////////////////////////////////////////////////////////

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

        // 원정 포인트 UI
        void UpdateAwayPointUI();
        Image* m_away = nullptr;
        Text* m_awayCurValue = nullptr;
        Text* m_awayDivText = nullptr;          // "/" 이렇게 구분하는 텍스트
        Text* m_awayMaxValue = nullptr;

        // 병영 Info UI
        void CreateBarrackUI();
        void UpdateBarrackUI();
        void UpdateAttackPowerText();
        void ShowClickedBarrackObj();
        void SetBarracksSelected(bool on);
        GameObject* m_barrackUI = nullptr;
        GameObject* m_barrackCurText = nullptr;
        GameObject* m_barrackDivText = nullptr;
        GameObject* m_barrackMaxText = nullptr;
        GameObject* m_attackPowerText = nullptr;

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
        Image* m_awayCostImage03 = nullptr;
        Text* m_awayCostText03 = nullptr;

        Text* m_awayAwardInfo = nullptr;
        Text* m_awayAwardText01 = nullptr;
        Text* m_awayAwardText02 = nullptr;

        Button* m_awayButton = nullptr;

        ////////////////////////////////////////////////////////////////////////////////
        // 2025.08.12 옵션 추가 (게임 씬)
        ////////////////////////////////////////////////////////////////////////////////

        void CreateOptionUI();
        void ShowOptionUI();
        void CloseOptionUI();

        // 옵션창

        bool isOpenOption = false;

        Image* m_optionUI = nullptr;
        Image* m_optionVolume = nullptr;
        Image* m_optionControl = nullptr;
        Image* m_optionCredit = nullptr;

        // 옵션 닫기 버튼
        Button* m_closeOption = nullptr;
        Button* m_backToTitle = nullptr;
        Button* m_quitGame = nullptr;

        // 옵션 선택 실제 버튼
        Button* m_selectVolume = nullptr;
        Button* m_selectControl = nullptr;
        Button* m_selectCredit = nullptr;

        // 옵션 선택 더미 텍스트
        Text* m_selectVolumeDummyText = nullptr;
        Text* m_selectControlDummyText = nullptr;
        Text* m_selectCreditDummyText = nullptr;

        // 볼륨 선택 슬라이더
        Slider* m_masterSlider = nullptr;
        Slider* m_bgmSlider = nullptr;
        Slider* m_sfxSlider = nullptr;

        // 배속 키 텍스트
        Text* m_stopKeyText = nullptr;
        Text* m_playKeyText = nullptr;
        Text* m_speedKeyText = nullptr;

        float m_prevGameSpeed = 0.0f;

        ////////////////////////////////////////////////////////////////////////////////

        // UI 개발용 필터
        void CreateFillter();
        Image* m_fillter = nullptr;
    };
}