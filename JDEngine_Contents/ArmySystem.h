#pragma once
#include "JDGlobal.h"

namespace JDGameSystem {
    using Resource = JDGlobal::Contents::Resource;
    using UnitType = JDGlobal::Contents::UnitType;
    using UnitTypeData = JDGlobal::Contents::UnitTypeData;

    constexpr int UnitTypeCount = static_cast<int>(UnitType::Count);

    struct UnitCounts { // 병사 수 정보.
        std::array<int, UnitTypeCount> counts{};

        int& operator[](UnitType type) {
            return counts[static_cast<int>(type)];
        }

        const int& operator[](UnitType type) const {
            return counts[static_cast<int>(type)];
        }

        UnitCounts& operator+=(const UnitCounts& other) {
            for (int i = 0; i < UnitTypeCount; ++i) {
                counts[i] += other.counts[i];
            }
            return *this;
        }

        int Total() const {
            int sum = 0;
            for (int n : counts) sum += n;
            return sum;
        }
    };

    class ArmySystem {
    public:
        ArmySystem() { InitUnitTypes(); }
        ~ArmySystem() = default;

        void InitUnitTypes();

        const UnitCounts& GetUnitCounts() const { return m_unitCounts; }
        int GetTotalUnits() const { return m_unitCounts.Total(); }
        const std::array<UnitTypeData, UnitTypeCount>& GetUnitTypes() const { return m_unitTypes; }

        // void SetUnitCounts(const UnitCounts& army); // 인구 수 변화 있음. 일단 필요 없어 보여서 주석처리.
        void OverrideUnitCounts(const UnitCounts& army) { m_unitCounts = army; } // 인구 수 변화 없음.
        static void BringbackPopulation(int num); // 현재 인구 수 되돌리기.

        bool RecruitUnits(UnitType type);

        int CalculateTotalPower() const;

    private:
        std::array<UnitTypeData, UnitTypeCount> m_unitTypes; // 병종 정보 저장용.
        UnitCounts m_unitCounts;
    };
}


