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
namespace JDScene {
    struct Attacker {
        JDGameObject::GameObjectBase* enemy;  // 공격 대상 적
        float timer;                          // 공격 타이머 누적
    };


    class GameScene : public SceneBase
    {
        using Image = JDGameObject::Content::Image;
        using Text = JDGameObject::Content::Text;
        using Button = JDGameObject::Content::Button;

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

        void SpawnWaveEnemy(const Vector2F& pos);
        void SpawnPlayerArmy(const Vector2F& pos);
        void SpawnBattleObject(const Vector2F& pos);

        void MoveEnemyObjects(float deltaTime);
        void MovePlayerObjects(float deltaTime);

        void AttackWall(float deltaTime);

        void ClickUpdate();

        void SafeDestroy(GameObjectBase* obj);

        // 공통 생성 함수.
        GameObjectBase* CreateSoldierUnit( const JDGameSystem::UnitCounts& units, JDGlobal::Base::GameTag tag,
            JDGlobal::Contents::State state, const Vector2F& pos, const std::string& textureName); // 병사.

        GameObjectBase* CreateStructure(const std::wstring& name, JDGlobal::Base::GameTag tag, 
            const Vector2F& pos, const std::string& textureName); // 구조물.

        GameObjectBase* CreateUIButton( const std::wstring& name, const Vector2F& pos,
            const std::string& textureName, const std::string& clickEventName, std::function<void()> callback); // 버튼.


        bool IsEnemySpawned(int day) const {
            return std::find(m_showedDays.begin(), m_showedDays.end(), day) != m_showedDays.end();
        }

        void AddEnemyDay(int day) {
            if (!IsEnemySpawned(day)) m_showedDays.push_back(day);
        }

        ////////////////////////////////////////////////////////////////////////////////
        // 맵 생성 함수
        ////////////////////////////////////////////////////////////////////////////////
        void CreateMap();

        void InitBuildMenu();
        void ShowBuildMenu();
        void CloseBuildMenu();

        void ShowBuildInfo(JDGlobal::Contents::BuildingType buildType, std::string costText, std::string effectText);
        void CloseBuildInfo();

        void ChangeBuildInfo(JDGlobal::Contents::BuildingType buildType, std::string costText, std::string effectText);

        void ShowFilledMenu();

    private:
        FMOD::Channel* bgmChannel = nullptr;
        FMOD::Channel* sfxChannel = nullptr;

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
        const float m_battleTime = 5.0f;
        float m_btlElapsedTime = 0.0f;

        JDGameObject::GameObjectBase* m_targetEnemy = nullptr;    // 처음 선택된 적.
        JDGameObject::GameObjectBase* m_playerObject = nullptr;   // 처음 아군.
        JDGameObject::GameObjectBase* m_barracksObject = nullptr; // 아군 병영.
        JDGameObject::GameObjectBase* m_battleObject = nullptr;   // 싸우는 애니메이션 보여줄 오브젝트.
        JDGameObject::GameObjectBase* m_wallObject = nullptr;     // 아군 성벽.

        std::vector<Attacker> m_attackers;       // 현재 성벽을 공격 중인 적들
        int   m_wallHealth = 1000;            // 성벽 체력 초기값

        bool m_isBarracksSelected = false; // 병영 선택 여부.



        ////////////////////////////////////////////////////////////////////////////////
        // 맵 생성 변수
        ////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr <BuildSystem> m_buildSystem;
        std::vector<UIObject*> m_TutorialUIObjects;

        // 그리드
        int m_totalCols = 4;
        int m_totalRows = 6;
        // std::vector<bool> m_isOpen;

        std::vector<Button*> m_emptyButtons;
        std::vector<Button*> m_filledButtons;

        Image* m_Menu = nullptr;
        std::vector<Button*> m_menuButtons;
        Button* m_selectedTool = nullptr;
        JDComponent::ColliderBase* m_selectedCollider = nullptr;

        ////////////////////////////////////////////////////////////////////////////////

        // [상단] 자원
        Button* m_resPop = nullptr;             // 인구 UI
        Text* m_resPopText = nullptr;           // 인구 텍스트
        Text* m_resPopBonusText = nullptr;      // 인구 보너스 텍스트

        Button* m_resFood = nullptr;            // 음식 UI
        Text* m_resFoodText = nullptr;          // 음식 텍스트
        Text* m_resFoodBonusText = nullptr;     // 음식 보너스 텍스트

        Button* m_resWood = nullptr;            // 목재 UI
        Text* m_resWoodText = nullptr;          // 목재 텍스트
        Text* m_resWoodBonusText = nullptr;     // 목재 보너스 텍스트

        Button* m_resMineral = nullptr;         // 광물 UI
        Text* m_resMineralText = nullptr;       // 광물 텍스트
        Text* m_resMineralBonusText = nullptr;  // 광물 보너스 텍스트

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

        // 그리드와 상호작용 중 인지 확인하기 위한 플래그
        bool isGridBuild = false;
        bool isGridSetting = false;
        bool isAway = false;

        // [하단] 건물 상호작용 UI
        Image* m_defaultUI = nullptr;           // 0. 기본 UI
        Image* m_buildUI = nullptr;             // 1. 건설 UI
        Image* m_upgradeUI = nullptr;           // 2. 업그레이드 및 고양이 배치 UI
        Image* m_awayUI = nullptr;        // 3. 징병 및 원정 UI

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

        // 필터, 나래이션
        Image* m_fillter = nullptr;
        Text* m_narration = nullptr;

        ////////////////////////////////////////////////////////////////////////////////
    };
}