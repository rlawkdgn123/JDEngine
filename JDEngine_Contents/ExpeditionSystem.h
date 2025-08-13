#pragma once
#include "JDGlobal.h"

// 원정을 보내봅시다. 일단 시간 없으니까 간단하게.
namespace JDGameSystem {
	using Resource = JDGlobal::Contents::Resource;
	using ExpeditionInfo = JDGlobal::Contents::ExpeditionInfo;

	enum class ExpeditionGrade { // 원정 등급.
		Beginner,     // 초급.
		Intermediate, // 중급.
		Higher,       // 상급.
		Count
	};

	constexpr int GradeCount = static_cast<int>(ExpeditionGrade::Count);

	//struct ExpeditionInfo { // 원정 정보. 
	//	ExpeditionInfo() = default;
	//	ExpeditionInfo(Resource cost, int point, float successRate, Resource reward)
	//		: m_cost(cost), m_point(point), m_successRate(successRate), m_reward(reward) {}

	//	Resource m_cost;           // 필요 자원.
	//	int m_point;               // 원정 포인트.
	//	float m_successRate;       // 추가 보상 확률.
	//	Resource m_reward;         // 추가 보상. 
	//};

	class ExpeditionSystem {
	public:
		static ExpeditionSystem& Instance() {
			static ExpeditionSystem instance;
			return instance;
		}

	private:
		ExpeditionSystem() { InitGradesInfo(); }
		~ExpeditionSystem() = default;

		ExpeditionSystem(const ExpeditionSystem&) = delete;
		ExpeditionSystem& operator=(const ExpeditionSystem&) = delete;

	public:
		void InitGradesInfo(); // 원정 정보 초기화.
		ExpeditionInfo GetExpeditionInfo(ExpeditionGrade grade) const; // 원정 정보 가져오기.
		bool SendExpedition(ExpeditionGrade grade); // 원정 보낼 수 있는지 검사하고 자원 소모.
		bool RollBonusReward(float successRate) const; // 추가 보상 얻을 수 있는지.
		void RollBonusType(); // 랜덤 보상 종류 결정.
		void ResolveExpedition(); // 원정 결과.
		bool ReachedTheGoal() const { return m_expeditionPoints >= m_goalPoints; } // 원정 목표를 다 채웠는지.

		int GetExpeditionPoints() const { return m_expeditionPoints; } // 누적 원정 포인트 가져오기.
		int GetGoalPoints() const { return m_goalPoints; } // 목표 원정 포인트 가져오기.
		int GetRandomResourceIndex() const { return m_randomResourceIndex; } // 랜덤 보상 종류 가져오기.


	private:
		int m_expeditionPoints = 0; // 누적 원정 포인트.
		const int m_goalPoints = 200; // 목표 원정 포인트.

		bool m_expeditionActive = true; // 원정 보낼 수 있는지. 만약 원정대가 있다면 false. 원정 보내지 못함.
		ExpeditionGrade m_nowExpeditionGrade = ExpeditionGrade::Beginner; // 현재 진행중인 원정.

		std::array<ExpeditionInfo, GradeCount> m_expeditionGrades; // 원정 정보 저장용.

		int m_randomResourceIndex = 0; // 랜덤 보너스 보상 종류. 0: 음식, 2: 목재, 3: 광물.
	};
}