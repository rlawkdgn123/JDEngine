#pragma once
#include "JDGlobal.h"

namespace JDGameSystem {
    using Resource = JDGlobal::Contents::Resource;

    enum class UnitType : int // ���� ����.
    {
        Novice = 0, // �߽�����.
        Expert, // ���ó���
        Count
    };

    constexpr int UnitTypeCount = static_cast<int>(UnitType::Count);

    class UnitTypeData { // ���� ���� �� ������. Ÿ��, �ʿ� �ڿ�, ������ ����.
    public:
        UnitTypeData(UnitType type = UnitType::Novice, Resource cost = {}, int power = 0)
            : m_unitType(type), m_recruitCost(cost), m_power(power) {
        }

        UnitType GetUnitType() const { return m_unitType; }
        Resource GetRecruitCost() const { return m_recruitCost; }
        int GetPower() const { return m_power; }

    private:
        UnitType m_unitType;
        Resource m_recruitCost;
        int m_power;
    };

    struct UnitCounts { // ���� �� ����.
        std::array<int, UnitTypeCount> counts{};

        int& operator[](UnitType type) {
            return counts[static_cast<int>(type)];
        }

        const int& operator[](UnitType type) const {
            return counts[static_cast<int>(type)];
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

        void SetUnitCounts(const UnitCounts& army); // �α� �� ��ȭ ����.
        void OverrideUnitCounts(const UnitCounts& army) { m_unitCounts = army; } // �α� �� ��ȭ ����.

        bool RecruitUnits(UnitType type);
        int CalculateTotalPower() const;

    private:
        std::array<UnitTypeData, UnitTypeCount> m_unitTypes;
        UnitCounts m_unitCounts;
    };
}


