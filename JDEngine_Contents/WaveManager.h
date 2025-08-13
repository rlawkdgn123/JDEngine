#pragma once
#include "ArmySystem.h"
#include "JDGlobal.h"

struct WaveData {
    JDGameSystem::UnitCounts enemyUnits;
    int day;
};

struct GameDate { // m_currDay를 게임 속 날짜로 바꿔 전달하기 용.
    int year;
    int month;
    int day;
};

constexpr int MAX_DAY = 35640; // 99년 12월 30일

class WaveManager {
public:
    using WaveStats = JDGlobal::Contents::WaveStats;
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

    void AdvanceDay() { if (m_currDay < MAX_DAY) ++m_currDay; } // 99년 12월 30일을 넘지 않도록.

    int GetCurrDay() const { return m_currDay; }
    int GetCurrStage() const { return m_currStage; }
    const WaveData* GetWave(int day) const;
    GameDate GetConvertedDate() const;
    int GetWaveEnemyPower() const;
    int GetTotalWaves() const { return static_cast<int>(m_waves.size()); } // 총 웨이브 수.
    int GetRemainingWaves() const; // 남은 웨이브 수.

    void SetCurrDay(int day) { m_currDay = day; }
    void SetCurrStage(int stage) { m_currStage = stage;  m_currDay = 1; }

    void Reset();


private:
    std::vector<WaveData> m_waves;
    WaveStats m_wavesTable;
    int m_currDay = 1; // 현재 날짜.
    int m_currStage = 1; // 현재 스테이지.

};

