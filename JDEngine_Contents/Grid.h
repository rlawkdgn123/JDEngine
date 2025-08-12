#include "JDGlobal.h"
#include "ResourceSystem.h"

#pragma once
namespace JDGameObject {
    namespace Content {
        class House;    // ���� ����
        class Building; // ���� ����
    }
}

namespace JDGameObject {
    namespace Content {
        class Grid : public JDGameObject::GameObject
        {
        public:
            using Direction = JDGlobal::Contents::Direction;
            using CatType = JDGlobal::Contents::CatType;
            using SynergyBonus = JDGlobal::Contents::CatSynergyBonus;
        public:
            Grid() : GameObject(L"Grid") { m_cat = CatType::None; }
            Grid(const std::wstring& name) : GameObject(name) { m_cat = CatType::None; }
        public:

            void Awake() override;
            void Start() override;                              // ���� 1ȸ�� ȣ��
            void Update(float deltaTime) override;              // Update
            void LateUpdate(float deltaTime) override;          // Update �� ȣ��
            void FixedUpdate(float fixedDeltaTime) override;    // ���� ����

            bool IsExpanded() const { return m_expanded; }
            void SetExpanded(bool expanded) { m_expanded = expanded; }

            bool IsOccupied() const { return m_occupied; }
            void SetOccupied(bool occupied) { m_occupied = occupied; }

            bool HasBuilding() const { return m_hasBuilding; }
            void SetHasBuilding(bool hasBuilding) { m_hasBuilding = hasBuilding; }

            void SetBuilding(JDGameObject::GameObject* building);
            GameObject* GetBuilding() const { return m_buildingRaw; }
            CatType GetCatType() const { return m_cat; }

            void ChangeCatType(CatType type);
            void UpdateSynergy();

            void SetOtherGrid(Direction dir, Grid* otherGrid) {
                switch (dir) {
                case Direction::North: others[0] = otherGrid; break;
                case Direction::South: others[1] = otherGrid; break;
                case Direction::West: others[2] = otherGrid; break;
                case Direction::East: others[3] = otherGrid; break;
                }
            }

            Grid* GetOtherGrid(Direction dir) const{
                switch (dir) {
                case Direction::North: return others[0]; break;
                case Direction::South: return others[1]; break;
                case Direction::West: return others[2]; break;
                case Direction::East: return others[3]; break;
                }
            }

        private:
            bool m_expanded = false; // ���� ���� Ȯ�� ���� ���� ����
            bool m_occupied = false; // ���� Ȯ�� �� ������ �������� ����
            bool m_hasBuilding = false; // �ǹ� ���� ����

            CatType m_cat;
            JDGameObject::GameObject* m_buildingRaw; // �������� �ǹ� ���� ptr
            JDGlobal::Contents::Resource m_synergyBonus;
            JDGameSystem::ResourceSystem* m_resourceSystem;

            // ��, ��, ��, �� ���� �׸��� ������ �迭 (0:��,1:��,2:��,3:��)
            Grid* others[4] = { nullptr, nullptr, nullptr, nullptr };
        };
    }
}