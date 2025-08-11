#include "pch.h"
#include "WaveManager.h"

void WaveManager::LoadStageWaves()
{
    m_waves.clear();
    m_currDay = 1;

    switch (m_currStage) {
    case 1: { // 아직 스테이지 1밖에 없음.
        m_waves.push_back({ JDGameSystem::UnitCounts{ {3, 0} }, 3 }); // 3일차.
        m_waves.push_back({ JDGameSystem::UnitCounts{ {5, 1} }, 5 }); // 5일차.
        m_waves.push_back({ JDGameSystem::UnitCounts{ {7, 2} }, 7 }); // 7일차.
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

std::vector<int> WaveManager::GetRemainingDays() const // 모든 웨이브 남을 날짜들. 
{
    std::vector<int> result;
    result.reserve(m_waves.size());

    for (const auto& wave : m_waves) {
        int remDay = wave.day - m_currDay;
        if (remDay < 0 || remDay > 7) remDay = -1;
        result.push_back(remDay);
    }

    return result;
}

void WaveManager::Reset()
{
	m_currDay = 1;
	m_waves.clear();
}
