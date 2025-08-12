#pragma once
#include "ArmySystem.h"

struct WaveData {
    JDGameSystem::UnitCounts enemyUnits;
    int day;
};

struct GameDate { // m_currDay�� ���� �� ��¥�� �ٲ� �����ϱ� ��.
    int year;
    int month;
    int day;
};

constexpr int MAX_DAY = 35640; // 99�� 12�� 30��

class WaveManager {
public:
    static WaveManager& Instance() {
        static WaveManager instance;
        return instance;
    }

private:
    WaveManager() = default;
    ~WaveManager() = default;

    WaveManager(const WaveManager&) = delete;
    WaveManager& operator=(const WaveManager&) = delete;

public:
    void LoadStageWaves();
    const WaveData* GetWaveForToday() const;

    void AdvanceDay() { if (m_currDay < MAX_DAY) ++m_currDay; } // 99�� 12�� 30���� ���� �ʵ���.

    int GetCurrDay() const { return m_currDay; }
    int GetCurrStage() const { return m_currStage; }
    const WaveData* GetWave(int day) const;
    GameDate GetConvertedDate() const;
    int GetWaveEnemyPower() const;

    void SetCurrDay(int day) { m_currDay = day; }
    void SetCurrStage(int stage) { m_currStage = stage;  m_currDay = 1; }

    void Reset();


private:
    std::vector<WaveData> m_waves;
    int m_currDay = 1; // ���� ��¥.
    int m_currStage = 1; // ���� ��������.
};

