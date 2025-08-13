#pragma once
#include "JDGlobal.h"

// ������ �������ô�. �ϴ� �ð� �����ϱ� �����ϰ�.
namespace JDGameSystem {
	using Resource = JDGlobal::Contents::Resource;
	using ExpeditionInfo = JDGlobal::Contents::ExpeditionInfo;

	enum class ExpeditionGrade { // ���� ���.
		Beginner,     // �ʱ�.
		Intermediate, // �߱�.
		Higher,       // ���.
		Count
	};

	constexpr int GradeCount = static_cast<int>(ExpeditionGrade::Count);

	//struct ExpeditionInfo { // ���� ����. 
	//	ExpeditionInfo() = default;
	//	ExpeditionInfo(Resource cost, int point, float successRate, Resource reward)
	//		: m_cost(cost), m_point(point), m_successRate(successRate), m_reward(reward) {}

	//	Resource m_cost;           // �ʿ� �ڿ�.
	//	int m_point;               // ���� ����Ʈ.
	//	float m_successRate;       // �߰� ���� Ȯ��.
	//	Resource m_reward;         // �߰� ����. 
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
		void InitGradesInfo(); // ���� ���� �ʱ�ȭ.
		ExpeditionInfo GetExpeditionInfo(ExpeditionGrade grade) const; // ���� ���� ��������.
		bool SendExpedition(ExpeditionGrade grade); // ���� ���� �� �ִ��� �˻��ϰ� �ڿ� �Ҹ�.
		bool RollBonusReward(float successRate) const; // �߰� ���� ���� �� �ִ���.
		void RollBonusType(); // ���� ���� ���� ����.
		void ResolveExpedition(); // ���� ���.
		bool ReachedTheGoal() const { return m_expeditionPoints >= m_goalPoints; } // ���� ��ǥ�� �� ä������.

		int GetExpeditionPoints() const { return m_expeditionPoints; } // ���� ���� ����Ʈ ��������.
		int GetGoalPoints() const { return m_goalPoints; } // ��ǥ ���� ����Ʈ ��������.
		int GetRandomResourceIndex() const { return m_randomResourceIndex; } // ���� ���� ���� ��������.


	private:
		int m_expeditionPoints = 0; // ���� ���� ����Ʈ.
		const int m_goalPoints = 200; // ��ǥ ���� ����Ʈ.

		bool m_expeditionActive = true; // ���� ���� �� �ִ���. ���� �����밡 �ִٸ� false. ���� ������ ����.
		ExpeditionGrade m_nowExpeditionGrade = ExpeditionGrade::Beginner; // ���� �������� ����.

		std::array<ExpeditionInfo, GradeCount> m_expeditionGrades; // ���� ���� �����.

		int m_randomResourceIndex = 0; // ���� ���ʽ� ���� ����. 0: ����, 2: ����, 3: ����.
	};
}