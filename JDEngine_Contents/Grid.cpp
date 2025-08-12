#include "pch.h"
#include "framework.h"
#include "House.h"
#include "Building.h"
#include "Grid.h"
#include "ResourceSystem.h"

using namespace std;

namespace JDGameObject {
    namespace Content {

        void Grid::Awake()
        {
            __super::Awake();
            m_resourceSystem = &ResourceSystem::Instance();
        }

        void Grid::Start() {
            __super::Start();
        }

        void Grid::Update(float deltaTime) {
            if (!m_buildingRaw || !m_hasBuilding) { ChangeCatType(CatType::None); return; } // 건물이 없으면 리턴

            // 시너지 처리를 한 후 고양이를 업데이트
            if (auto building = dynamic_cast<Building*>(m_buildingRaw)) {
                if (m_cat == building->GetCatType()) return;  // 이전 고양이랑 같으면
                else ChangeCatType(building->GetCatType());
            }
        }

        void Grid::LateUpdate(float deltaTime) {}
        void Grid::FixedUpdate(float fixedDeltaTime) {}
        void Grid::SetBuilding(JDGameObject::GameObject* building)
        {
            m_buildingRaw = dynamic_cast<JDGameObject::GameObject*>(building);
        }

        // 고양이를 바꾸기
        void Grid::ChangeCatType(CatType type) {

            UpdateSynergy(); // 로직 전 기존 고양이 시너지 업데이트

            if (m_cat == type) return;

            CatType preType = m_cat; // 기존 고양이 변수

            m_resourceSystem->AddSynergyBonus(-m_synergyBonus); // 기존 시너지 제거

            m_cat = type; // 새 고양이 할당
            UpdateSynergy(); // 새 고양이에 대한 보너스 재할당

        }

        // 고양이 보너스를 주변 타일 고양이 개수와 합해서 계산해주기
        void Grid::UpdateSynergy()
        {
            // 상하좌우 순회하면서 고양이 타입에 대한 보너스 / 페널티 누적
            Resource newBonus{ 0, 0, 0 };
            SynergyBonus synergyData;

            for (Grid* other : others) {
                if (!other || !other->HasBuilding()) continue;  // nullptr 및 건물 유무 체크

                auto building = dynamic_cast<Building*>(other->GetBuilding());
                if (!building) continue;
                    
                auto catType = building->GetCatType();

                switch (m_cat) {
                case CatType::Felis:
                    if (catType == CatType::Kone) newBonus += synergyData.FelisBonus;
                    else if (catType == CatType::Navi) newBonus += synergyData.FelisPenalty;
                    break;

                case CatType::Navi:
                    if (catType == CatType::Felis) newBonus += synergyData.NaviBonus;
                    else if (catType == CatType::Kone) newBonus += synergyData.NaviPenalty;
                    break;

                case CatType::Kone:
                    if (catType == CatType::Navi) newBonus += synergyData.KoneBonus;
                    else if (catType == CatType::Felis) newBonus += synergyData.KonePenalty;
                    break;

                default:
                    break;
                }
            }
            m_synergyBonus = (newBonus - m_synergyBonus);
            m_resourceSystem->AddSynergyBonus(m_synergyBonus);
        }
    }
}