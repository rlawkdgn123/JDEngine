#pragma once
#include "ArmySystem.h"

namespace JDGameSystem {

    class CombatSystem {
    public:
        CombatSystem() = default;
        ~CombatSystem() = default;

        void ResolveBattle(ArmySystem& player, const ArmySystem& enemy);
        bool ResolveBattle(const UnitCounts& playerUnits, int playerPower,
                           const UnitCounts& enemyUnits, int enemyPower,
                           UnitCounts& outPlayerResult, UnitCounts& outEnemyResult);
    };
}