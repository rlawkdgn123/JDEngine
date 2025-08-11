#pragma once
#include "ArmySystem.h"

struct WaveData {
    JDGameSystem::UnitCounts enemyUnits;
    int day;
};

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

    void AdvanceDay() { ++m_currDay; }

    int GetCurrDay() const { return m_currDay; }
    int GetCurrStage() const { return m_currStage; }
    const WaveData* GetWave(int day) const;
    std::vector<int> GetRemainingDays() const;

    void SetCurrDay(int day) { m_currDay = day; }
    void SetCurrStage(int stage) { m_currStage = stage;  m_currDay = 1; }

    void Reset();


private:
    std::vector<WaveData> m_waves;
    int m_currDay = 1; // 현재 날짜.
    int m_currStage = 1; // 현재 스테이지.
};

