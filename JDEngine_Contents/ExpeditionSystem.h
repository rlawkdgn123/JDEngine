#pragma once
#include "ArmySystem.h"

// ������ �������ô�. �ϴ� �ð� �����ϱ� �����ϰ�.
namespace JDGameSystem {

	class ExpeditionSystem {
	public:
		void SetArmyReference(ArmySystem* army) { m_armyRef = army; }
		bool AddExpeditionUnit(); // ������� ����.
		bool TrySendExpedition() const; // ���� ���� ����.
		void ExpeditionReward(); // ���� ����.

		bool CanSendExpedition() const { return m_expeditionCount > 0; } // ������ ���� �� �ִ���.

	private:
		int m_expeditionCount = 0; // ������ ������� ��.
		int m_expeditionPoints = 0; // ���� ���� ����Ʈ.
		const int m_goalPoints = 100; // ��ǥ ���� ����Ʈ.

		ArmySystem* m_armyRef = nullptr;
	};
}