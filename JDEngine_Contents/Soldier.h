#pragma once
#include "ArmySystem.h"

namespace JDGameObject {
    namespace Content {
        class Soldier : public JDGameObject::GameObject
        {
        public:
            Soldier() : GameObject(L"Soldier") {}
            Soldier(const std::wstring& name) : GameObject(name) {}
            void Awake() override;
            void Start() override;                              // ���� 1ȸ�� ȣ��
            void Update(float deltaTime) override;              // Update
            void LateUpdate(float deltaTime) override;          // Update �� ȣ��
            void FixedUpdate(float fixedDeltaTime) override;    // ���� ����

            void SetUnitCounts(const JDGameSystem::UnitCounts& counts) { m_unitCounts = counts; }
            void SetPower(int power) { m_totalPower = power; }

            const JDGameSystem::UnitCounts& GetUnitCounts() const { return m_unitCounts; }
            int GetPower() const { return m_totalPower; }

        private:
            JDGameSystem::UnitCounts m_unitCounts;
            int m_totalPower = 0;
        };
    }
}
