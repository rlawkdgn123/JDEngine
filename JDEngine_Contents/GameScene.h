#pragma once
#include "pch.h"
#include "framework.h"
#include "SceneBase.h"
#include "InputManager.h" 
#include "ArmySystem.h"

namespace JDScene {
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

        void SpawnWaveEnemy(const Vector2F& pos);
        void SpawnPlayerArmy(const Vector2F& pos);
        void SpawnBattleObject(const Vector2F& pos);

        void MoveEnemyObjects(float deltaTime);
        void MovePlayerObjects(float deltaTime);

        void ClickUpdate();

        void SafeDestroy(GameObjectBase* obj);

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

        JDGameSystem::ArmySystem player;

        const float m_dayTime = 10.0f;
        float m_elapsedTime = 0.0f;
        const float m_battleTime = 5.0f;
        float m_btlElapsedTime = 0.0f;

        JDGameObject::GameObjectBase* m_targetEnemy = nullptr;    // 처음 선택된 적.
        JDGameObject::GameObjectBase* m_playerObject = nullptr;   // 처음 아군.
        JDGameObject::GameObjectBase* m_barracksObject = nullptr; // 아군 병영.
        JDGameObject::GameObjectBase* m_battleObject = nullptr;   // 싸우는 애니메이션 보여줄 오브젝트.
        JDGameObject::GameObjectBase* m_wallObject = nullptr;     // 아군 성벽.

        bool m_isBarracksSelected = false; // 병영 선택 여부.
    };
}