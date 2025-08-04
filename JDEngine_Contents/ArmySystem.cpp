#include "pch.h"
#include "ArmySystem.h"
#include "ResourceSystem.h"

namespace JDGameSystem {

	void ArmySystem::InitUnitTypes() {
		m_unitTypes[static_cast<int>(UnitType::Temp0)] =
			UnitTypeData(UnitType::Temp0, Resource(200.f, 50.f, 0.f), 100);
		m_unitTypes[static_cast<int>(UnitType::Temp1)] =
			UnitTypeData(UnitType::Temp1, Resource(100.f, 0.f, 50.f), 200);
	}

	bool ArmySystem::RecruitUnits(UnitType type)
	{
		auto& rs = ResourceSystem::Instance();
		
		// �ڿ� ���� ��������
		const int curPopulation = rs.GetCurPopulation();
		const Resource haveResource = rs.GetTotalResource();

		// �α� �� ���� �� ���� ������ ������ �� ����
		if (curPopulation <= m_unitCounts.Total()) {
			std::cout << "[ArmySystem] lack of population" << std::endl;
			return false;
		}

		// �����ϴ� ����
		const int idx = static_cast<int>(type);
		if (idx < 0 || idx >= static_cast<int>(UnitType::Count)) {
			std::cout << "[ArmySystem] invalid unit type" << std::endl;
			return false;
		}

		const UnitTypeData& unitType = m_unitTypes[idx];

		// �ʿ� �ڿ�
		const Resource needResource = unitType.GetRecruitCost();

		// �ڿ��� ������� �˻�
		if (!(haveResource.m_food >= needResource.m_food &&
			haveResource.m_wood >= needResource.m_wood &&
			haveResource.m_mineral >= needResource.m_mineral)) {
			std::cout << "[ArmySystem] lack of resources" << std::endl;
			return false;
		}

		// �ڿ� ����
		rs.SetTotalResource(haveResource - needResource);

		// ���� �� ����
		++m_unitCounts[type];

		return true;
	}
	int ArmySystem::CalculateTotalPower() const
	{
		int totalPower = 0;

		for (int i = 0; i < UnitTypeCount; ++i)
		{
			int unitCount = m_unitCounts.counts[i];
			int unitPower = m_unitTypes[i].GetPower();
			totalPower += unitCount * unitPower;
		}

		return totalPower;
		
		return 0;
	}
}