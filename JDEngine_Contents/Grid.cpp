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
            if (!m_buildingRaw || !m_hasBuilding) { ChangeCatType(CatType::None); return; } // �ǹ��� ������ ����

            // �ó��� ó���� �� �� ����̸� ������Ʈ
            if (auto building = dynamic_cast<Building*>(m_buildingRaw)) {
                if (m_cat == building->GetCatType()) return;  // ���� ����̶� ������
                else ChangeCatType(building->GetCatType());
            }
        }

        void Grid::LateUpdate(float deltaTime) {}
        void Grid::FixedUpdate(float fixedDeltaTime) {}
        void Grid::SetBuilding(JDGameObject::GameObject* building)
        {
            m_buildingRaw = dynamic_cast<JDGameObject::GameObject*>(building);
        }

        // ����̸� �ٲٱ�
        void Grid::ChangeCatType(CatType type) {

            UpdateSynergy(); // ���� �� ���� ����� �ó��� ������Ʈ

            if (m_cat == type) return;

            CatType preType = m_cat; // ���� ����� ����

            m_resourceSystem->AddSynergyBonus(-m_synergyBonus); // ���� �ó��� ����

            m_cat = type; // �� ����� �Ҵ�
            UpdateSynergy(); // �� ����̿� ���� ���ʽ� ���Ҵ�

        }

        // ����� ���ʽ��� �ֺ� Ÿ�� ����� ������ ���ؼ� ������ֱ�
        void Grid::UpdateSynergy()
        {
            // �����¿� ��ȸ�ϸ鼭 ����� Ÿ�Կ� ���� ���ʽ� / ���Ƽ ����
            Resource newBonus{ 0, 0, 0 };
            SynergyBonus synergyData;

            for (Grid* other : others) {
                if (!other || !other->HasBuilding()) continue;  // nullptr �� �ǹ� ���� üũ

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