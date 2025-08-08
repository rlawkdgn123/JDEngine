#include "pch.h"
#include "ArmySystem.h"
#include "ResourceSystem.h"

namespace JDGameSystem {

	void ArmySystem::InitUnitTypes() // 병력 정보 초기화. (타입, 자원, 전투력)
	{
		m_unitTypes[static_cast<int>(UnitType::Novice)] =
			UnitTypeData(UnitType::Novice, Resource(200.f, 50.f, 0.f), 10);
		m_unitTypes[static_cast<int>(UnitType::Expert)] =
			UnitTypeData(UnitType::Expert, Resource(100.f, 0.f, 50.f), 20);
	}

	/*void ArmySystem::SetUnitCounts(const UnitCounts& army)
	{
		const int nowCounts = army.Total(); // 새 병사 수.
		const int prevCounts = m_unitCounts.Total(); // 이전 병사 수.
		const int diffCounts = nowCounts - prevCounts; // 차이 계산. 

		auto& rs = ResourceSystem::Instance();

		const int curPopulation = rs.GetCurPopulation();
		rs.SetCurPopulation(curPopulation + diffCounts); // 현재 인수 수에 반영.

		m_unitCounts = army;
	}*/

	bool ArmySystem::RecruitUnits(UnitType type) // 병력 모집.
	{
		auto& rs = ResourceSystem::Instance();

		// 자원 정보 가져오기.
		const int curPopulation = rs.GetCurPopulation();
		const Resource haveResource = rs.GetTotalResource();

		//// 인구 수 보다 더 많은 병력을 모집할 수 없음.
		//if (curPopulation <= m_unitCounts.Total()) {
		//	std::cout << "[ArmySystem] lack of population" << std::endl;
		//	return false;
		//}

		// 인구 수가 1보다 작으면 병력을 모집할 수 없음.
		if (curPopulation < 1) {
			std::cout << "[ArmySystem] lack of population" << std::endl;
			return false;
		}

		// 모집하는 병종.
		const int idx = static_cast<int>(type);
		if (idx < 0 || idx >= UnitTypeCount) {
			std::cout << "[ArmySystem] invalid unit type" << std::endl;
			return false;
		}

		const UnitTypeData& unitType = m_unitTypes[idx];

		// 필요 자원.
		const Resource needResource = unitType.GetRecruitCost();

		// 자원이 충분한지 검사.
		if (!(haveResource.m_food >= needResource.m_food &&
			haveResource.m_wood >= needResource.m_wood &&
			haveResource.m_mineral >= needResource.m_mineral)) {
			std::cout << "[ArmySystem] lack of resources" << std::endl;
			return false;
		}

		// 자원 감소.
		rs.SetTotalResource(haveResource - needResource);
		
		// 현재 인구 감소.
		rs.SetCurPopulation(curPopulation - 1);

		// 병력 수 증가.
		++m_unitCounts[type];

		return true;
	}

	bool ArmySystem::DecreaseUnitCount() // 병력 원정 보내기용.
	{
		if (m_unitCounts[UnitType::Novice] > 0) { // 견습냥이가 있다면 견습냥이에서 한 마리를 줄이고, 
			m_unitCounts[UnitType::Novice] -= 1;
			return true;
		}
		else if (m_unitCounts[UnitType::Expert] > 0) { // 견습냥이가 없고 숙련냥이가 있다면 숙련냥이 한 마리를 줄임.
			m_unitCounts[UnitType::Expert] -= 1;
			return true;
		}

		return false;
	}

	int ArmySystem::CalculateTotalPower() const // 총 전투력 계산.
	{
		int totalPower = 0;

		for (int i = 0; i < UnitTypeCount; ++i)
		{
			int unitCount = m_unitCounts.counts[i];
			int unitPower = m_unitTypes[i].GetPower();
			totalPower += unitCount * unitPower;
		}

		return totalPower;
	}
}