#pragma once
#include "ArmySystem.h"

namespace JDGameSystem {

    class CombatSystem {
    public:
        CombatSystem() = default;
        ~CombatSystem() = default;

        UnitCounts ResolveBattle() const; // ���� ���� �� ���� ���� �� ���. 
    };
}