#pragma once
#include "pch.h"
#include "framework.h"
#include "SceneBase.h"
#include "InputManager.h" 
#include "ArmySystem.h"

namespace JDScene {
    struct Attacker {
        JDGameObject::GameObjectBase* enemy;  // ���� ��� ��
        float timer;                          // ���� Ÿ�̸� ����
    };


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
    private:
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
    };
}