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
		
		// 자원 정보 가져오기
		const int curPopulation = rs.GetCurPopulation();
		const Resource haveResource = rs.GetTotalResource();

		// 인구 수 보다 더 많은 병력을 모집할 수 없음
		if (curPopulation <= m_unitCounts.Total()) {
			std::cout << "[ArmySystem] lack of population" << std::endl;
			return false;
		}

		// 모집하는 병종
		const int idx = static_cast<int>(type);
		if (idx < 0 || idx >= static_cast<int>(UnitType::Count)) {
			std::cout << "[ArmySystem] invalid unit type" << std::endl;
			return false;
		}

		const UnitTypeData& unitType = m_unitTypes[idx];

		// 필요 자원
		const Resource needResource = unitType.GetRecruitCost();

		// 자원이 충분한지 검사
		if (!(haveResource.m_food >= needResource.m_food &&
			haveResource.m_wood >= needResource.m_wood &&
			haveResource.m_mineral >= needResource.m_mineral)) {
			std::cout << "[ArmySystem] lack of resources" << std::endl;
			return false;
		}

		// 자원 감소
		rs.SetTotalResource(haveResource - needResource);

		// 병력 수 증가
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