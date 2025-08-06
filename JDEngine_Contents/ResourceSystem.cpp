#include "pch.h"
#include "ResourceSystem.h"

using namespace JDGlobal::Contents;

namespace JDGameSystem {
    void ResourceSystem::SetMaxPopulation(const int& value)
    {
        if (value < 0) {
            m_maxPopulation = 0;
            m_curPopulation = 0;  // 병력도 모두 파괴하는 처리
            std::cout << "최대 인구가 0으로 설정되어 병력이 모두 파괴되었습니다." << std::endl;
        }
        else if (value > MAX_GAME_POPULATION) {
            m_maxPopulation = MAX_GAME_POPULATION;
        }
        else {
            m_maxPopulation = value;
            //if (m_curPopulation > m_maxPopulation) {
            //    m_curPopulation = m_maxPopulation;  // 현재 인구가 최대 인구를 넘으면 조정 & 넘은 요소 파괴
            //}
        }
    }

    void ResourceSystem::AddMaxPopulation(const int& value)
    {
        if (m_maxPopulation + value < 0) {
            m_maxPopulation = 0;
            //m_curPopulation = 0; // 병력 모두 파괴 처리 추가 가능
            //std::cout << "최대 인구가 0으로 설정되어 병력이 모두 파괴되었습니다." << std::endl;
            std::cout << "최대 인구가 0으로 설정되었으나, 현재 인구가 초과되었습니다." << std::endl;
        }
        else if (m_maxPopulation + value > MAX_GAME_POPULATION) {
            m_maxPopulation = MAX_GAME_POPULATION;
            std::cout << "최대 인구 크기 도달" << std::endl;
        }
        else {
            m_maxPopulation += value;
        }

        // 현재 인구가 최대 인구를 넘으면 조정 & 넘은 요소 파괴
        /*if (m_curPopulation > m_maxPopulation) {
            m_curPopulation = m_maxPopulation;
        }*/
    }
    void ResourceSystem::SetCurPopulation(const int& value)
    {
        if (value < 0) {
            m_curPopulation = 0;  // 병력 모두 파괴
            std::cout << "현재 인구가 0으로 설정되어 병력이 모두 파괴되었습니다." << std::endl;
        }
        else if (value > m_maxPopulation) {
            std::cout << "최대 인구수 도달, 취소합니다." << std::endl;
            return;
            //m_curPopulation = m_maxPopulation;
        }
        else {
            m_curPopulation = value;
        }
    }

    void ResourceSystem::AddCurPopulation(const int& value)
    {
        if (m_maxPopulation + value < 0) {
            m_curPopulation = 0; // 병력 모두 파괴 처리 추가 가능
            std::cout << "최대 인구가 0으로 설정되어 병력이 모두 파괴되었습니다." << std::endl;
        }
        else if (m_maxPopulation + value > MAX_GAME_POPULATION) {
            std::cout << "최대 인구수 도달, 취소합니다." << std::endl;
            return;
           /* m_curPopulation = m_maxPopulation;
            std::cout << "최대 인구 크기 도달" << std::endl;*/
        }
        else {
            m_curPopulation += value;
        }

        // 현재 인구가 최대 인구를 넘으면 조정 & 넘은 요소 파괴
        if (m_curPopulation > m_maxPopulation) {
            m_curPopulation = m_maxPopulation;
        }
    }
    
}
