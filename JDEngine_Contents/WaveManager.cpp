#include "pch.h"
#include "WaveManager.h"

void WaveManager::LoadStageWaves()
{
    m_waves.clear();
    m_currDay = 1;

    switch (m_currStage) {
    case 1: { // 아직 스테이지 1밖에 없음.
        m_waves.push_back({ JDGameSystem::UnitCounts{ {3, 0} },  84 });  
        m_waves.push_back({ JDGameSystem::UnitCounts{ {5, 1} },  94 });  
        m_waves.push_back({ JDGameSystem::UnitCounts{ {7, 2} }, 104 });  
        m_waves.push_back({ JDGameSystem::UnitCounts{ {3, 0} }, 114 });  
        m_waves.push_back({ JDGameSystem::UnitCounts{ {5, 1} }, 124 }); 
        m_waves.push_back({ JDGameSystem::UnitCounts{ {7, 2} }, 134 });
        m_waves.push_back({ JDGameSystem::UnitCounts{ {3, 0} }, 144 }); 
        m_waves.push_back({ JDGameSystem::UnitCounts{ {5, 1} }, 154 });
        m_waves.push_back({ JDGameSystem::UnitCounts{ {7, 2} }, 164 });
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

void WaveManager::Reset()
{
	m_currDay = 1;
	m_waves.clear();
}
