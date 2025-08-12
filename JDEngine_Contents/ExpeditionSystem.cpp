#include "pch.h"
#include "ExpeditionSystem.h"
#include "ResourceSystem.h"
#include <random>

namespace JDGameSystem {
	void ExpeditionSystem::InitGradesInfo() { // ���� ���� �ʱ�ȭ.
		m_expeditionGrades[static_cast<int>(ExpeditionGrade::Beginner)] =
			ExpeditionInfo(Resource(50.f, 50.f, 50.f), 10, 10.f, Resource(55.f, 55.f, 55.f));
		m_expeditionGrades[static_cast<int>(ExpeditionGrade::Intermediate)] =
			ExpeditionInfo(Resource(100.f, 100.f, 100.f), 25, 15.f, Resource(150.f, 150.f, 150.f));
		m_expeditionGrades[static_cast<int>(ExpeditionGrade::Higher)] =
			ExpeditionInfo(Resource(150.f, 150.f, 150.f), 40, 20.f, Resource(250.f, 250.f, 250.f));
	}

	ExpeditionInfo ExpeditionSystem::GetExpeditionInfo(ExpeditionGrade grade) const { // ���� ���� ��������.
		const int idx = static_cast<int>(grade);
		if (idx < 0 || idx >= GradeCount) {
			std::cout << "[ExpeditionSystem] invalid." << std::endl;
			return ExpeditionInfo(Resource(999.f, 999.f, 999.f), 0, 0.f, Resource(0.f, 0.f, 0.f)); // ������.
		}

		return m_expeditionGrades[idx];
	}

	bool ExpeditionSystem::SendExpedition(ExpeditionGrade grade){ // ���� ���� �� �ִ��� �˻��ϰ� �ڿ� �Ҹ�.
		if (ReachedTheGoal()) { // ���� ����Ʈ�� �� ������� ���� ����.
			std::cout << "[ExpeditionSystem] ���� ����Ʈ�� �� ������� ���� ����." << std::endl;
			return false;
		}
		
		if (!m_expeditionActive) { // ȭ�鿡 �����밡 ������ ���� ������ ����. 
			std::cout << "[ExpeditionSystem] ȭ�鿡 �����밡 ������ ���� ������ ����." << std::endl;
			return false;
		}

		ExpeditionInfo expeditionInfo = GetExpeditionInfo(grade);
		if (expeditionInfo.m_successRate == 0) { // ���� �������� ������ ����.
			std::cout << "[ExpeditionSystem] �������� ����." << std::endl;
			return false; 
		} 

		m_nowExpeditionGrade = grade; // ���� �������� ����.
		
		auto& rs = ResourceSystem::Instance();

		const Resource haveResource = rs.GetTotalResource(); // ���� �ڿ�.
		const Resource needResource = expeditionInfo.m_cost; // �ʿ� �ڿ�.

		// �ڿ��� ������� �˻�.
		if (!(haveResource.m_food >= needResource.m_food &&
			haveResource.m_wood >= needResource.m_wood &&
			haveResource.m_mineral >= needResource.m_mineral)) {
			std::cout << "[ExpeditionSystem] lack of resources" << std::endl;
			return false;
		}

		m_expeditionActive = false; // ���� ������ ����.

		// �ڿ� ����.
		rs.SetTotalResource(haveResource - needResource);

		return true;
	}

	bool ExpeditionSystem::RollBonusReward(float successRate) const {
		if (successRate <= 0.0f) {
			std::cout << "[ExpeditionSystem] ���� Ȯ���� 0��." << std::endl;
			return false; 
		}
		
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dist(0.0f, 100.0f);
		float randValue = dist(gen);

		return randValue < successRate;
	}

	void ExpeditionSystem::RollBonusType() {
		// �ڿ� �� �ϳ��� �����ؼ� ����.
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dist(0, 2);
		m_randomResourceIndex = dist(gen);
		std::cout << "[ExpeditionSystem] �������� ���� : " << m_randomResourceIndex << std::endl;
	}

	void ExpeditionSystem::ResolveExpedition() {
		ExpeditionInfo expeditionInfo = GetExpeditionInfo(m_nowExpeditionGrade);
		if (expeditionInfo.m_successRate == 0) { // ������.
			std::cout << "[ExpeditionSystem] �������� ����22." << std::endl;
			return;
		}

		m_expeditionPoints = std::min(m_goalPoints, m_expeditionPoints + expeditionInfo.m_point); // ���� ����Ʈ ���ϱ�.
		m_expeditionActive = true; // �ٽ� ���� �� �� �ְ�.

		RollBonusType(); // ���� ���� �ٽ� �����ֱ�.

		if (!RollBonusReward(expeditionInfo.m_successRate)) { // Ȯ���� ���� ���ϸ� ��.
			std::cout << "[ExpeditionSystem] Ȯ���� ���� ����." << std::endl;
			return;
		}

		Resource reward = {0, 0, 0};
		switch (m_randomResourceIndex) { // ���� �ڿ� �ϳ��� �����ٰ���.
		case 0:
			reward.m_food = expeditionInfo.m_reward.m_food;
			break;
		case 1:
			reward.m_wood = expeditionInfo.m_reward.m_wood;
			break;
		case 2:
			reward.m_mineral = expeditionInfo.m_reward.m_mineral;
			break;
		}

		auto& rs = ResourceSystem::Instance();
		rs.AddTotalResource(reward);
	}
}