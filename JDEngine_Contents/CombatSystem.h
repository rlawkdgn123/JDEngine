#pragma once
#include "ArmySystem.h"

namespace JDGameSystem {

    class CombatSystem {
    public:
        CombatSystem() = default;
        ~CombatSystem() = default;

        UnitCounts ResolveBattle() const; // 전투 진행 및 잃은 병사 수 계산. 
    };
}