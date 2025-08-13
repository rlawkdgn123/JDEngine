#include "JDGlobal.h"
#include "ResourceSystem.h"
#include "SceneBase.h"

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
            using GameObject = JDGameObject::GameObject;
            using SceneBase = JDScene::SceneBase;
            using Resource = JDGlobal::Contents::Resource;
        public:
            Grid() : GameObject(L"Grid") { m_cat = CatType::None; }
            Grid(const std::wstring& name) : GameObject(name) { m_cat = CatType::None; }
        public:

            void Awake() override;
            void Start() override;                              // ���� 1ȸ�� ȣ��
            void Update(float deltaTime) override;              // Update
            void LateUpdate(float deltaTime) override;          // Update �� ȣ��
            void FixedUpdate(float fixedDeltaTime) override;    // ���� ����
            void OnDestroy() override { } 

            bool IsExpanded() const { return m_expanded; }
            void SetExpanded(bool expanded) { m_expanded = expanded; }

            bool IsOccupied() const { return m_occupied; }
            void SetOccupied(bool occupied) { m_occupied = occupied; }

            bool HasBuilding() const { return m_hasBuilding; }
            void SetHasBuilding(bool hasBuilding) { m_hasBuilding = hasBuilding; }

            void SetBuilding(JDGameObject::GameObject* building);
            GameObject* GetBuilding() const { return m_buildingRaw; }
            CatType GetCatType() const { return m_cat; }

            // �ֿ� �׸��� ����
            void ChangeCatType(CatType type);
            void UpdateSynergy();
            bool CreateTextureObj(SceneBase& scene);
            void DeleteTextureObj(SceneBase& scene);

            // Texture Obj ���� �Լ�
            void SetActiveCatTextureObj(bool isActive) { if (m_catTextureObj) m_catTextureObj->SetActive(isActive); }
            void SetActiveLevelTextureObj(bool isActive) { if (m_levelTextureObj) m_levelTextureObj->SetActive(isActive); }
            void SetActiveSynergyEffectTextureObj(bool isActive) { if (m_synergyEffectTextureObj) m_synergyEffectTextureObj->SetActive(isActive); }
            void SetActiveSelectEffectTextureObj(bool isActive) { if (m_selectEffectTextureObj) m_selectEffectTextureObj->SetActive(isActive); }
            void SetActiveExpendEffectTextureObj(bool isActive) { if (m_expendEffectTextureObj) m_expendEffectTextureObj->SetActive(isActive); }
            void ChangeCatTexture();
            void ChangeSynergyEffectTexture();

            void SetOtherGrid(Direction dir, Grid* otherGrid) {
                switch (dir) {
                case Direction::North: others[static_cast<int>(Direction::North)] = otherGrid; break;
                case Direction::South: others[static_cast<int>(Direction::South)] = otherGrid; break;
                case Direction::West: others[static_cast<int>(Direction::West)] = otherGrid; break;
                case Direction::East: others[static_cast<int>(Direction::East)] = otherGrid; break;
                }
            }

            Grid* GetOtherGrid(Direction dir) const{
                switch (dir) {
                case Direction::North: return others[static_cast<int>(Direction::North)]; break;;
                case Direction::South: return others[static_cast<int>(Direction::South)]; break;;
                case Direction::West: return others[static_cast<int>(Direction::West)]; break;
                case Direction::East: return others[static_cast<int>(Direction::East)]; break;
                }
            }
            Grid* GetOtherGrid(int dir) const {
                switch (dir) {
                case static_cast<int>(Direction::North): return others[static_cast<int>(Direction::North)]; break;
                case static_cast<int>(Direction::South): return others[static_cast<int>(Direction::South)]; break;
                case static_cast<int>(Direction::West): return others[static_cast<int>(Direction::West)]; break;
                case static_cast<int>(Direction::East): return others[static_cast<int>(Direction::East)]; break;
                }
            }

            Resource GetSynergyBonus() const { return m_synergyBonus; }
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

            /////
            GameObject* m_catTextureObj = nullptr;                  // ����� �ؽ���
            GameObject* m_levelTextureObj = nullptr;                  // ����� �ؽ���
            GameObject* m_synergyEffectTextureObj = nullptr;        // ����ȿ�� ȭ��ǥ (�ӽ� ����)
            GameObject* m_selectEffectTextureObj = nullptr;         // �׸��� ���� �ؽ���
            GameObject* m_expendEffectTextureObj = nullptr;         // �׸��� Ȯ�� ���� �ؽ���
        };
    }
}