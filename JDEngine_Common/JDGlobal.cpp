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
			// 늘어난 레벨에 맞춰 헤더를 동적으로 생성
			std::cout << "자원,초기자원,";
			std::cout << "업그레이드비용";
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << "_" << (i + 1);
				if (i < MAX_GAME_LEVEL - 1) std::cout << ",";
			}
			std::cout << ",자원획득";
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << "_" << (i + 1);
				if (i < MAX_GAME_LEVEL - 1) std::cout << ",";
			}
			std::cout << ",자원소모";
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << "_" << (i + 1);
				if (i < MAX_GAME_LEVEL - 1) std::cout << ",";
			}
			std::cout << "\n";

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
			// 늘어난 레벨에 맞춰 헤더를 동적으로 생성
			std::cout << "데이터,";
			std::cout << "레벨";
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << "_" << (i + 1);
				if (i < MAX_GAME_LEVEL - 1) std::cout << ",";
			}
			std::cout << ",업그레이드비용";
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << "_" << (i + 1);
				if (i < MAX_GAME_LEVEL - 1) std::cout << ",";
			}
			std::cout << ",자원소모";
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << "_" << (i + 1);
				if (i < MAX_GAME_LEVEL - 1) std::cout << ",";
			}
			std::cout << "\n";

			// Population 행 출력 (레벨당 인구)
			std::cout << "Population : ";
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_initPopulation[i] << ",";
			}
			// 인구 행은 다른 데이터가 없으므로 빈 공간으로 채웁니다.
			for (int i = 0; i < MAX_GAME_LEVEL * 2; ++i) {
				std::cout << " ,";
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