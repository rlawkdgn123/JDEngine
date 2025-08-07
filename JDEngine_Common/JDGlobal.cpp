#include"pch.h"
#include"framework.h"
#include"JDGlobal.h"

namespace JDMath = JDGlobal::Math;

namespace JDGlobal {
	namespace Base {
	}
	namespace Core {
	}
	namespace Contents {

		void Resource::ResourcePrint() const{
			std::cout << "m_food : " << m_food << std::endl;
			std::cout << "m_wood : " << m_food << std::endl;
			std::cout << "m_mineral : " << m_food << std::endl;
		}

		void BuildingStats::PrintStats() {
			// 헤더 출력
			std::cout << "자원,초기자원,"
				<< "업그레이드비용_1,업그레이드비용_2,업그레이드비용_3,"
				<< "자원획득_1,자원획득_2,자원획득_3,"
				<< "자원소모_1,자원소모_2,자원소모_3\n";

			// Food 행 출력
			std::cout << "Food : " << m_initResource.m_food << ",";
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_upgradeCost[i].m_food << ",";
			}
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_resourceGenPerSec[i].m_food << ",";
			}
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_resourceSubPerSec[i].m_food << (i == MAX_GAME_LEVEL - 1 ? "" : ",");
			}
			std::cout << "\n";

			// Wood 행 출력
			std::cout << "Wood : " << m_initResource.m_wood << ",";
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_upgradeCost[i].m_wood << ",";
			}
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_resourceGenPerSec[i].m_wood << ",";
			}
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_resourceSubPerSec[i].m_wood << (i == MAX_GAME_LEVEL - 1 ? "" : ",");
			}
			std::cout << "\n";

			// Mineral 행 출력
			std::cout << "Mineral : " << m_initResource.m_mineral << ",";
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_upgradeCost[i].m_mineral << ",";
			}
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_resourceGenPerSec[i].m_mineral << ",";
			}
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_resourceSubPerSec[i].m_mineral << (i == MAX_GAME_LEVEL - 1 ? "" : ",");
			}
			std::cout << "\n";
		}

		void HouseStats::PrintStats() {
			// 헤더 출력
			std::cout << "데이터,"
				<< "레벨_1,레벨_2,레벨_3,"
				<< "업그레이드비용_1,업그레이드비용_2,업그레이드비용_3,"
				<< "자원소모_1,자원소모_2,자원소모_3\n";

			// Population 행 출력 (레벨당 인구)
			std::cout << "Population : ";
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_initPopulation[i] << ",";
			}
			// 인구 행은 다른 데이터가 없으므로 공백으로 채웁니다.
			for (int i = 0; i < MAX_GAME_LEVEL * 2; ++i) {
				std::cout << ",";
			}
			std::cout << "\n";

			// Food 행 출력
			std::cout << "Food : ";
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << " ,";
			}
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_upgradeCost[i].m_food << ",";
			}
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_resourceSubPerSec[i].m_food << (i == MAX_GAME_LEVEL - 1 ? "" : ",");
			}
			std::cout << "\n";

			// Wood 행 출력
			std::cout << "Wood : ";
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << " ,";
			}
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_upgradeCost[i].m_wood << ",";
			}
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_resourceSubPerSec[i].m_wood << (i == MAX_GAME_LEVEL - 1 ? "" : ",");
			}
			std::cout << "\n";

			// Mineral 행 출력
			std::cout << "Mineral : ";
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << " ,";
			}
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_upgradeCost[i].m_mineral << ",";
			}
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_resourceSubPerSec[i].m_mineral << (i == MAX_GAME_LEVEL - 1 ? "" : ",");
			}
			std::cout << "\n";
		}

	}
}