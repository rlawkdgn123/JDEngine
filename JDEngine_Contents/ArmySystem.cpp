#include "pch.h"
#include "ArmySystem.h"
#include "ResourceSystem.h"

namespace JDGameSystem {

	void ArmySystem::InitSordierTypes() // ���� ���� �ʱ�ȭ. (Ÿ��, �ڿ�, ������)
	{
		m_SordierTypes[static_cast<int>(SordierType::Novice)] =
			SordierTypeData(SordierType::Novice, Resource(200.f, 50.f, 0.f), 10);
		m_SordierTypes[static_cast<int>(SordierType::Expert)] =
			SordierTypeData(SordierType::Expert, Resource(100.f, 0.f, 50.f), 20);
	}

	/*void ArmySystem::SetUnitCounts(const UnitCounts& army)
	{
		const int nowCounts = army.Total(); // �� ���� ��.
		const int prevCounts = m_unitCounts.Total(); // ���� ���� ��.
		const int diffCounts = nowCounts - prevCounts; // ���� ���. 

		auto& rs = ResourceSystem::Instance();

		const int curPopulation = rs.GetCurPopulation();
		rs.SetCurPopulation(curPopulation + diffCounts); // ���� �μ� ���� �ݿ�.

		m_unitCounts = army;
	}*/

	void ArmySystem::BringbackPopulation(int num)
	{
		auto& rs = ResourceSystem::Instance();
		const int curPopulation = rs.GetCurPopulation();

		// ���� �α��� �ǵ�����.
		rs.SetCurPopulation(curPopulation + num);
	}

	bool ArmySystem::RecruitUnits(SordierType type) // ���� ����.
	{
		auto& rs = ResourceSystem::Instance();

		// �ڿ� ���� ��������.
		const int curPopulation = rs.GetCurPopulation();
		const Resource haveResource = rs.GetTotalResource();

		//// �α� �� ���� �� ���� ������ ������ �� ����.
		//if (curPopulation <= m_unitCounts.Total()) {
		//	std::cout << "[ArmySystem] lack of population" << std::endl;
		//	return false;
		//}

		// �α� ���� 1���� ������ ������ ������ �� ����.
		if (curPopulation < 1) {
			std::cout << "[ArmySystem] lack of population" << std::endl;
			return false;
		}

		// �����ϴ� ����.
		const int idx = static_cast<int>(type);
		if (idx < 0 || idx >= 2) {
			std::cout << "[ArmySystem] invalid unit type" << std::endl;
			return false;
		}

		const SordierTypeData& SordierType = m_SordierTypes[idx];

		// �ʿ� �ڿ�.
		const Resource needResource = SordierType.GetRecruitCost();

		// �ڿ��� ������� �˻�.
		if (!(haveResource.m_food >= needResource.m_food &&
			haveResource.m_wood >= needResource.m_wood &&
			haveResource.m_mineral >= needResource.m_mineral)) {
			std::cout << "[ArmySystem] lack of resources" << std::endl;
			return false;
		}

		// �ڿ� ����.
		rs.SetTotalResource(haveResource - needResource);
		
		// ���� �α� ����.
		rs.SetCurPopulation(curPopulation - 1);

		// ���� �� ����.
		++m_unitCounts[type];

		return true;
	}

	int ArmySystem::CalculateTotalPower() const // �� ������ ���.
	{
		int totalPower = 0;

		for (int i = 0; i < 2; ++i)
		{
			int unitCount = m_unitCounts.counts[i];
			int unitPower = m_SordierTypes[i].GetPower();
			totalPower += unitCount * unitPower;
		}

		return totalPower;
	}
}