#pragma once
#include "ArmySystem.h"

namespace JDGameSystem {

    class CombatSystem {
    public:
        CombatSystem() = default;
        ~CombatSystem() = default;

        void ResolveBattle(ArmySystem& player, const ArmySystem& enemy);
    };
}