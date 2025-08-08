#pragma once
#include "ArmySystem.h"

// 원정을 보내봅시다. 일단 시간 없으니까 간단하게.
namespace JDGameSystem {

	class ExpeditionSystem {
	public:
		void SetArmyReference(ArmySystem* army) { m_armyRef = army; }
		bool AddExpeditionUnit(); // 원정대원 모집.
		bool TrySendExpedition() const; // 원정 성공 여부.
		void ExpeditionReward(); // 원정 보상.

		bool CanSendExpedition() const { return m_expeditionCount > 0; } // 원정을 보낼 수 있는지.

	private:
		int m_expeditionCount = 0; // 모집된 원정대원 수.
		int m_expeditionPoints = 0; // 누적 원정 포인트.
		const int m_goalPoints = 100; // 목표 원정 포인트.

		ArmySystem* m_armyRef = nullptr;
	};
}