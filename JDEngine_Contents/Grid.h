#include "JDGlobal.h"
#include "ResourceSystem.h"
#include "SceneBase.h"

#pragma once
namespace JDGameObject {
    namespace Content {
        class House;    // 전방 선언
        class Building; // 전방 선언
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

        public:
            Grid() : GameObject(L"Grid") { m_cat = CatType::None; }
            Grid(const std::wstring& name) : GameObject(name) { m_cat = CatType::None; }
        public:

            void Awake() override;
            void Start() override;                              // 최초 1회만 호출
            void Update(float deltaTime) override;              // Update
            void LateUpdate(float deltaTime) override;          // Update 후 호출
            void FixedUpdate(float fixedDeltaTime) override;    // 물리 계산용
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

            // 주요 그리드 로직
            void ChangeCatType(CatType type);
            void UpdateSynergy();
            bool CreateTextureObj(SceneBase& scene);
            void DeleteTextureObj(SceneBase& scene);

            // Texture Obj 관리 함수
            void SetActiveCatTextureObj(bool isActive) { if (m_catTextureObj) m_catTextureObj->SetActive(isActive); }
            void SetActiveLevelTextureObj(bool isActive) { if (m_levelTextureObj) m_levelTextureObj->SetActive(isActive); }
            void SetActiveSynergyEffectTextureObj(bool isActive) { if (m_synergyEffectTextureObj) m_synergyEffectTextureObj->SetActive(isActive); }
            void SetActiveSelectEffectTextureObj(bool isActive) { if (m_selectEffectTextureObj) m_selectEffectTextureObj->SetActive(isActive); }
            void SetActiveExpendEffectTextureObj(bool isActive) { if (m_expendEffectTextureObj) m_expendEffectTextureObj->SetActive(isActive); }
            void ChangeCatTexture();
            void ChangeSynergyEffectTexture();

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
            bool m_expanded = false; // 현재 지형 확장 선택 가능 여부
            bool m_occupied = false; // 현재 확장 후 소유한 지형인지 여부
            bool m_hasBuilding = false; // 건물 소유 여부

            CatType m_cat;
            JDGameObject::GameObject* m_buildingRaw; // 소유중인 건물 참조 ptr
            JDGlobal::Contents::Resource m_synergyBonus;
            JDGameSystem::ResourceSystem* m_resourceSystem;

            // 상, 하, 좌, 우 인접 그리드 포인터 배열 (0:상,1:하,2:좌,3:우)
            Grid* others[4] = { nullptr, nullptr, nullptr, nullptr };

            /////
            GameObject* m_catTextureObj = nullptr;                  // 고양이 텍스쳐
            GameObject* m_levelTextureObj = nullptr;                  // 고양이 텍스쳐
            GameObject* m_synergyEffectTextureObj = nullptr;        // 인접효과 화살표 (임시 보류)
            GameObject* m_selectEffectTextureObj = nullptr;         // 그리드 선택 텍스쳐
            GameObject* m_expendEffectTextureObj = nullptr;         // 그리드 확장 가능 텍스쳐
        };
    }
}