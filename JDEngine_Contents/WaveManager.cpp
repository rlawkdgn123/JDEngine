#include "pch.h"
#include "WaveManager.h"
#include "DataTableManager.h"

void WaveManager::LoadStageWaves()
{
    m_wavesTable = DataTableManager::Instance().GetWaveTableInfo();
    m_waves.clear();
    m_currDay = 1;

    switch (m_currStage) {
    case 1: {
        for (int i = 0; i < JDGlobal::Contents::MAX_GAME_WAVE_COUNT; ++i)
        {
            // 병사 수
            JDGameSystem::UnitCounts units{
                { m_wavesTable.m_novice[i], m_wavesTable.m_expert[i] }
            };

            // 날짜
            int day = m_wavesTable.m_day[i];

            // 웨이브 데이터 추가
            m_waves.push_back({ units, day });
        }

        break;
    }
    default: {
        std::cout << "[WaveManager] No stage." << std::endl;
        break;
    }
    }
}

const WaveData* WaveManager::GetWaveForToday() const
{
    for (const auto& wave : m_waves) {
        if (wave.day == m_currDay)
            return &wave;
    }
    return nullptr;
}

const WaveData* WaveManager::GetWave(int day) const
{
    for (const auto& wave : m_waves) {
        if (wave.day == day)
            return &wave;
    }
    //std::cout << "[WaveManager] No wave." << std::endl;
    return nullptr;
}

GameDate WaveManager::GetConvertedDate() const
{
    const int daysPerMonth = 30; 
    const int monthsPerYear = 12;
    const int daysPerYear = daysPerMonth * monthsPerYear;  // 360??

    int totalDays = m_currDay - 1;

    GameDate date;
    date.year = totalDays / daysPerYear + 1; 
    date.month = (totalDays % daysPerYear) / daysPerMonth + 1; 
    date.day = (totalDays % daysPerMonth) + 1;  // 1??1??1?쇰????쒖옉.

    return date;
}

int WaveManager::GetWaveEnemyPower() const
{
    const int targetDay = m_currDay + 70;
    if (targetDay > MAX_DAY) return -1; 

    const WaveData* wave = GetWave(targetDay);

    if (!wave) return -1;
    
    JDGameSystem::ArmySystem enemy;
    enemy.OverrideUnitCounts(wave->enemyUnits);

    return enemy.CalculateTotalPower();
}

int WaveManager::GetRemainingWaves() const
{
    int count = 0;
    for (const auto& wave : m_waves) {
        if (wave.day >= m_currDay)
            ++count;
    }
    return count;
}

void WaveManager::Reset()
{
	m_currDay = 1;
	m_waves.clear();
}
