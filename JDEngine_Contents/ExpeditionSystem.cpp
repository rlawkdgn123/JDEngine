#include "pch.h"
#include "ExpeditionSystem.h"
#include "ResourceSystem.h"
#include <random>

namespace JDGameSystem {
	bool ExpeditionSystem::AddExpeditionUnit()
	{
		if (!m_armyRef) { 
			std::cout << "[ExpeditionSystem] No ArmySystem." << std::endl;
			return false; 
		}

		if (m_armyRef->DecreaseUnitCount()) {
			++m_expeditionCount;
			return true;
		}

		std::cout << "[ExpeditionSystem] No Army." << std::endl;
        return false;
	}

	bool ExpeditionSystem::TrySendExpedition() const
	{
		if (m_expeditionCount == 0) {
			std::cout << "[ExpeditionSystem] No Expedition." << std::endl;
			return false;
		}

		return true;
	}

	void ExpeditionSystem::ExpeditionReward()
	{
	}
}