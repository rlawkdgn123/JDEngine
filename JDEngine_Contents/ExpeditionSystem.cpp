#include "pch.h"
#include "ExpeditionSystem.h"
#include "ResourceSystem.h"
#include <random>

namespace JDGameSystem {
	void ExpeditionSystem::InitGradesInfo() { // 원정 정보 초기화.
		m_expeditionGrades[static_cast<int>(ExpeditionGrade::Beginner)] =
			ExpeditionInfo(Resource(50.f, 50.f, 50.f), 10, 10.f, Resource(55.f, 55.f, 55.f));
		m_expeditionGrades[static_cast<int>(ExpeditionGrade::Intermediate)] =
			ExpeditionInfo(Resource(100.f, 100.f, 100.f), 25, 15.f, Resource(150.f, 150.f, 150.f));
		m_expeditionGrades[static_cast<int>(ExpeditionGrade::Higher)] =
			ExpeditionInfo(Resource(150.f, 150.f, 150.f), 40, 20.f, Resource(250.f, 250.f, 250.f));
	}

	ExpeditionInfo ExpeditionSystem::GetExpeditionInfo(ExpeditionGrade grade) const { // 원정 정보 가져오기.
		const int idx = static_cast<int>(grade);
		if (idx < 0 || idx >= GradeCount) {
			std::cout << "[ExpeditionSystem] invalid." << std::endl;
			return ExpeditionInfo(Resource(999.f, 999.f, 999.f), 0, 0.f, Resource(0.f, 0.f, 0.f)); // 오류값.
		}

		return m_expeditionGrades[idx];
	}

	bool ExpeditionSystem::SendExpedition(ExpeditionGrade grade){ // 원정 보낼 수 있는지 검사하고 자원 소모.
		if (ReachedTheGoal()) { // 원정 포인트를 다 모았으면 원정 못감.
			std::cout << "[ExpeditionSystem] 원정 포인트를 다 모았으면 원정 못감." << std::endl;
			return false;
		}
		
		if (!m_expeditionActive) { // 화면에 원정대가 있으면 원정 보내지 못함. 
			std::cout << "[ExpeditionSystem] 화면에 원정대가 있으면 원정 보내지 못함." << std::endl;
			return false;
		}

		ExpeditionInfo expeditionInfo = GetExpeditionInfo(grade);
		if (expeditionInfo.m_successRate == 0) { // 만약 오류값이 왔으면 실패.
			std::cout << "[ExpeditionSystem] 오류값이 왔음." << std::endl;
			return false; 
		} 

		m_nowExpeditionGrade = grade; // 현재 진행중인 원정.
		
		auto& rs = ResourceSystem::Instance();

		const Resource haveResource = rs.GetTotalResource(); // 현재 자원.
		const Resource needResource = expeditionInfo.m_cost; // 필요 자원.

		// 자원이 충분한지 검사.
		if (!(haveResource.m_food >= needResource.m_food &&
			haveResource.m_wood >= needResource.m_wood &&
			haveResource.m_mineral >= needResource.m_mineral)) {
			std::cout << "[ExpeditionSystem] lack of resources" << std::endl;
			return false;
		}

		m_expeditionActive = false; // 원정 못가게 막기.

		// 자원 감소.
		rs.SetTotalResource(haveResource - needResource);

		return true;
	}

	bool ExpeditionSystem::RollBonusReward(float successRate) const {
		if (successRate <= 0.0f) {
			std::cout << "[ExpeditionSystem] 성공 확률이 0임." << std::endl;
			return false; 
		}
		
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dist(0.0f, 100.0f);
		float randValue = dist(gen);

		return randValue < successRate;
	}

	void ExpeditionSystem::RollBonusType() {
		// 자원 중 하나만 선택해서 보상.
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dist(0, 2);
		m_randomResourceIndex = dist(gen);
		std::cout << "[ExpeditionSystem] 랜덤보상 종류 : " << m_randomResourceIndex << std::endl;
	}

	void ExpeditionSystem::ResolveExpedition() {
		ExpeditionInfo expeditionInfo = GetExpeditionInfo(m_nowExpeditionGrade);
		if (expeditionInfo.m_successRate == 0) { // 오류값.
			std::cout << "[ExpeditionSystem] 오류값이 왔음22." << std::endl;
			return;
		}

		m_expeditionPoints = std::min(m_goalPoints, m_expeditionPoints + expeditionInfo.m_point); // 원정 포인트 더하기.
		m_expeditionActive = true; // 다시 원정 갈 수 있게.

		RollBonusType(); // 랜덤 보상 다시 정해주기.

		if (!RollBonusReward(expeditionInfo.m_successRate)) { // 확률을 뚫지 못하면 끝.
			std::cout << "[ExpeditionSystem] 확률을 뚫지 못함." << std::endl;
			return;
		}

		Resource reward = {0, 0, 0};
		switch (m_randomResourceIndex) { // 랜덤 자원 하나만 더해줄거임.
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