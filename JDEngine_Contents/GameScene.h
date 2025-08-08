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
        JDGameObject::GameObjectBase* enemy;  // ���� ��� ��
        float timer;                          // ���� Ÿ�̸� ����
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

        // ���� ���� �Լ�.
        GameObjectBase* CreateSoldierUnit( const JDGameSystem::UnitCounts& units, JDGlobal::Base::GameTag tag,
            JDGlobal::Contents::State state, const Vector2F& pos, const std::string& textureName); // ����.

        GameObjectBase* CreateStructure(const std::wstring& name, JDGlobal::Base::GameTag tag, 
            const Vector2F& pos, const std::string& textureName); // ������.

        GameObjectBase* CreateUIButton( const std::wstring& name, const Vector2F& pos,
            const std::string& textureName, const std::string& clickEventName, std::function<void()> callback); // ��ư.


        bool IsEnemySpawned(int day) const {
            return std::find(m_showedDays.begin(), m_showedDays.end(), day) != m_showedDays.end();
        }

        void AddEnemyDay(int day) {
            if (!IsEnemySpawned(day)) m_showedDays.push_back(day);
        }

        ////////////////////////////////////////////////////////////////////////////////
        // �� ���� �Լ�
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

        JDGameSystem::ArmySystem m_playerArmy; // ����.
        JDGameSystem::ArmySystem m_enemyArmy; // �ش� ��¥�� �����ϴ� ��.

        JDGameSystem::UnitCounts m_playerBattleArmy;
        JDGameSystem::UnitCounts m_enemyBattleArmy;
        int m_playerTotalPower = 0;
        int m_enemyTotalPower = 0;

        const float m_dayTime = 10.0f;
        float m_elapsedTime = 0.0f;
        const float m_battleTime = 5.0f;
        float m_btlElapsedTime = 0.0f;

        JDGameObject::GameObjectBase* m_targetEnemy = nullptr;    // ó�� ���õ� ��.
        JDGameObject::GameObjectBase* m_playerObject = nullptr;   // ó�� �Ʊ�.
        JDGameObject::GameObjectBase* m_barracksObject = nullptr; // �Ʊ� ����.
        JDGameObject::GameObjectBase* m_battleObject = nullptr;   // �ο�� �ִϸ��̼� ������ ������Ʈ.
        JDGameObject::GameObjectBase* m_wallObject = nullptr;     // �Ʊ� ����.

        std::vector<Attacker> m_attackers;       // ���� ������ ���� ���� ����
        int   m_wallHealth = 1000;            // ���� ü�� �ʱⰪ

        bool m_isBarracksSelected = false; // ���� ���� ����.



        ////////////////////////////////////////////////////////////////////////////////
        // �� ���� ����
        ////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr <BuildSystem> m_buildSystem;
        std::vector<UIObject*> m_TutorialUIObjects;

        // �׸���
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

        // [���] �ڿ�
        Button* m_resPop = nullptr;             // �α� UI
        Text* m_resPopText = nullptr;           // �α� �ؽ�Ʈ
        Text* m_resPopBonusText = nullptr;      // �α� ���ʽ� �ؽ�Ʈ

        Button* m_resFood = nullptr;            // ���� UI
        Text* m_resFoodText = nullptr;          // ���� �ؽ�Ʈ
        Text* m_resFoodBonusText = nullptr;     // ���� ���ʽ� �ؽ�Ʈ

        Button* m_resWood = nullptr;            // ���� UI
        Text* m_resWoodText = nullptr;          // ���� �ؽ�Ʈ
        Text* m_resWoodBonusText = nullptr;     // ���� ���ʽ� �ؽ�Ʈ

        Button* m_resMineral = nullptr;         // ���� UI
        Text* m_resMineralText = nullptr;       // ���� �ؽ�Ʈ
        Text* m_resMineralBonusText = nullptr;  // ���� ���ʽ� �ؽ�Ʈ

        ////////////////////////////////////////////////////////////////////////////////

        // [���] ���� ���̺�
        Image* m_monsterWaveBackground = nullptr;

        ////////////////////////////////////////////////////////////////////////////////

        // [���] ��¥ �г�
        Image* m_datePanel = nullptr;
        Text* m_yearText = nullptr;
        Text* m_monthText = nullptr;
        Text* m_dayText = nullptr;

        Button* m_stopButton = nullptr;         // �Ͻ����� ��ư
        Button* m_playButton = nullptr;         // ��� ��ư
        Button* m_speedButton = nullptr;        // ��� ��ư
        Button* m_optionButton = nullptr;       // �ɼ� ��ư

        ////////////////////////////////////////////////////////////////////////////////

        // �׸���� ��ȣ�ۿ� �� ���� Ȯ���ϱ� ���� �÷���
        bool isbuild = false;
        bool isupgrade = false;
        bool isexpedition = false;

        // [�ϴ�] �ǹ� ��ȣ�ۿ� UI
        Image* m_defaultUI = nullptr;           // 0. �⺻ UI
        Image* m_buildUI = nullptr;             // 1. �Ǽ� UI
        Image* m_upgradeUI = nullptr;           // 2. ���׷��̵� �� ����� ��ġ UI
        Image* m_expeditionUI = nullptr;        // 3. ¡�� �� ���� UI

        ////////////////////////////////////////////////////////////////////////////////

        // 0. �⺻ UI

        // 1. �Ǽ� UI ( �ְ���, ������, �����, ����, (������) )
        Button* m_buildHouse = nullptr;
        Button* m_buildFishingspot = nullptr;
        Button* m_buildRumbermill = nullptr;
        Button* m_buildMine = nullptr;
        // Button* m_buildLab = nullptr;

        // 1. �Ǽ� Info
        Text* m_buildTypeText = nullptr;

        Text* m_costInfoText = nullptr;
        Text* m_costText = nullptr;
        Image* m_costImage = nullptr;

        Text* m_effectInfoText = nullptr;
        Text* m_effectText = nullptr;
        Image* m_effctImage = nullptr;

        ////////////////////////////////////////////////////////////////////////////////

        // ����, �����̼�
        Image* m_fillter = nullptr;
        Text* m_narration = nullptr;

        ////////////////////////////////////////////////////////////////////////////////
    };
}