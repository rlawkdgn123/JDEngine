#include "pch.h"
#include "WaveManager.h"

void WaveManager::LoadStageWaves()
{
    m_waves.clear();
    m_currDay = 1;

    switch (m_currStage) {
    case 1: { // 아직 스테이지 1밖에 없음.
        //m_waves.push_back({ JDGameSystem::UnitCounts{ {1, 0} }, 300 });  
        //m_waves.push_back({ JDGameSystem::UnitCounts{ {0, 1} }, 500 });  
        //m_waves.push_back({ JDGameSystem::UnitCounts{ {3, 0} }, 800 });
        //m_waves.push_back({ JDGameSystem::UnitCounts{ {0, 3} }, 1000 });
        //m_waves.push_back({ JDGameSystem::UnitCounts{ {8, 0} }, 1300 });
        //m_waves.push_back({ JDGameSystem::UnitCounts{ {0, 5} }, 1500 });
        //m_waves.push_back({ JDGameSystem::UnitCounts{ {13, 0} }, 1800 });
        //m_waves.push_back({ JDGameSystem::UnitCounts{ {0, 7} }, 2000 });
        //m_waves.push_back({ JDGameSystem::UnitCounts{ {16, 0} }, 2300 });
        //m_waves.push_back({ JDGameSystem::UnitCounts{ {0, 9} }, 2500 });
        //m_waves.push_back({ JDGameSystem::UnitCounts{ {20, 0} }, 2800 });
        //m_waves.push_back({ JDGameSystem::UnitCounts{ {0, 11} }, 3000 });
        m_waves.push_back({ JDGameSystem::UnitCounts{ {10, 0} }, 84 });
        m_waves.push_back({ JDGameSystem::UnitCounts{ {10, 0} }, 94 });
        m_waves.push_back({ JDGameSystem::UnitCounts{ {10, 0} }, 104 });
        m_waves.push_back({ JDGameSystem::UnitCounts{ {10, 0} }, 114 });
        m_waves.push_back({ JDGameSystem::UnitCounts{ {10, 0} }, 124 });

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
    const int daysPerYear = daysPerMonth * monthsPerYear;  // 360일.

    int totalDays = m_currDay - 1;

    GameDate date;
    date.year = totalDays / daysPerYear + 1; 
    date.month = (totalDays % daysPerYear) / daysPerMonth + 1; 
    date.day = (totalDays % daysPerMonth) + 1;  // 1년 1월 1일부터 시작.

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
