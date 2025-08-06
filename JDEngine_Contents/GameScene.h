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

        void SpawnWaveEnemy();
        void SpawnPlayerArmy();

        void MoveEnemyObjects(float deltaTime);
        void MovePlayerObjects(float deltaTime);

        void ClickUpdate();

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

        const float m_dayTime = 10.f;
        float m_elapsedTime = 0.0f;

        JDGameObject::GameObject* m_playerObject = nullptr;   // 화면에 존재하는 아군 유닛.
        JDGameObject::GameObject* m_targetEnemy = nullptr;    // 현재 선택된 적 유닛.
        JDGameObject::GameObject* m_barracksObject = nullptr; // 아군 병영.

        bool m_isBarracksSelected = false; // 병영 선택 여부.
    };
}