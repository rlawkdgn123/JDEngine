#pragma once
#include "JDGlobal.h"

namespace JDGameSystem {
    using Resource = JDGlobal::Contents::Resource;
    using SordierType = JDGlobal::Contents::SordierType;
    using SordierTypeData = JDGlobal::Contents::SordierTypeData;

    constexpr int SordierTypeCount = static_cast<int>(SordierType::Count);

    struct UnitCounts { // 병사 수 정보.
        std::array<int, SordierTypeCount> counts{};

        int& operator[](SordierType type) {
            return counts[static_cast<int>(type)];
        }

        const int& operator[](SordierType type) const {
            return counts[static_cast<int>(type)];
        }

        UnitCounts& operator+=(const UnitCounts& other) {
            for (int i = 0; i < SordierTypeCount; ++i) {
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
        ArmySystem() { InitSordierTypes(); }
        ~ArmySystem() = default;

        void InitSordierTypes();

        const UnitCounts& GetUnitCounts() const { return m_unitCounts; }
        int GetTotalUnits() const { return m_unitCounts.Total(); }
        const std::array<SordierTypeData, 2>& GetSordierTypes() const { return m_SordierTypes; }

        // void SetUnitCounts(const UnitCounts& army); // 인구 수 변화 있음. 일단 필요 없어 보여서 주석처리.
        void OverrideUnitCounts(const UnitCounts& army) { m_unitCounts = army; } // 인구 수 변화 없음.
        static void BringbackPopulation(int num); // 현재 인구 수 되돌리기.

        bool RecruitUnits(SordierType type);

        int CalculateTotalPower() const;

    private:
        std::array<SordierTypeData, 2> m_SordierTypes; // 병종 정보 저장용.
        UnitCounts m_unitCounts;
    };
}


