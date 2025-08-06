#pragma once
#include "pch.h"
#include "framework.h"
#include <iostream>
#include <cmath>            // std::round
#include <stdexcept>        // std::invalid_argument
namespace JDMath = JDGlobal::Math;

namespace JDGlobal {	
	namespace Base {

		// 인게임 식별용 태그 (현재는 태그 임의)
		enum class GameTag
		{
			None,
			Player,
			PlayerMove,
			PlayerCat,
			PlayerBuilding,
			Enemy,
			EnemyMove,
			EnemyCat,
			EnemyBuilding,
			Barracks,
			NPC,
			UI,
			Mover
		};

		// 렌더링 순서 제어용 Enum
		enum class SortingLayer : int {
			None,
			BackGround,
			Building,
			Cat,
		};
		
		struct RenderLayerInfo {
			SortingLayer sortingLayer;
			int orderInLayer = 0;

			// operator< 오버로드
			bool operator<(const RenderLayerInfo& other) const {
				if (sortingLayer != other.sortingLayer)
					return sortingLayer < other.sortingLayer;
				return orderInLayer < other.orderInLayer;
			}
		};

	}
	namespace Core {
		// Scenes
		enum class SceneType : int { // 범위가 있는 class 열거형 // C++11
			None = -1, // 유효하지 않은 초기 상태
			SCENE_TEST,
			SCENE_TITLE,
			SCENE_SELECTNATION,
			SCENE_TUTORIAL,
			SCENE_PLAY,
			SCENE_ENDING,
			// ... 씬 추가
			SCENE_MAX // 씬 개수 카운트 : 씬 개수 + 1의 개수이므로, 가져다 쓰면 곧 카운트가 된다.
		};

		constexpr unsigned int START_MAX_OBJECTS = 300;
		constexpr unsigned int MAX_SCENES = static_cast<int>(SceneType::SCENE_MAX); // 추후 바꿀수도..??

		// Games
		//enum ShapeType {
		//	Unknown,
		//	Circle,
		//	Ractangle,
		//	Triangle,
		//	Line
		//};
		// GameObjects
		using MessageID = uint32_t;

		constexpr int OBJECT_NAME_LEN_MAX = 15; // 객체 이름 최대치

		enum class PivotPreset
		{
			TopLeft,
			TopCenter,
			TopRight,
			CenterLeft,
			Center,
			CenterRight,
			BottomLeft,
			BottomCenter,
			BottomRight,
		};

		// 스크린 기준의 앵커. RectTransform의 m_position은 해당 앵커 기준으로 +보정이 된다.
		enum class AnchorPreset {
			TopLeft,
			TopCenter,
			TopRight,
			CenterLeft,
			Center,
			CenterRight,
			BottomLeft,
			BottomCenter,
			BottomRight,
			StretchHorizontal,
			StretchVertical,
			StretchAll
		};
	}
	namespace Contents {
		constexpr int MAX_GAME_RESOURCE = 9999;  // 자원 최대치
		constexpr int MAX_GAME_POPULATION = 999; // 인구 최대치
		constexpr int MAX_GAME_LEVEL = 3;	 // 업그레이드 최대치
		constexpr int MAX_GAME_RESOURCE_BONUS = 500;	 //보너스 최대치
		constexpr int RESOURCE_COUNT = 3;
		struct Resource {

			Resource() = default;
			Resource(int food, int wood, int mineral)
				: m_food(food), m_wood(wood), m_mineral(mineral) {
			}
			Resource(const Resource& resource) {
				m_food = resource.m_food;
				m_wood = resource.m_wood;
				m_mineral = resource.m_mineral;
			}

			int m_food;
			int m_wood;
			int m_mineral;

			Resource& operator=(const Resource& other) {
				m_food = other.m_food;
				m_wood = other.m_wood;
				m_mineral = other.m_mineral;
				return *this;
			}

			Resource operator-() const {
				return Resource{
					-m_food,
					-m_wood,
					-m_mineral
				};
			}

			Resource operator+(const Resource& other) const {
				return Resource{
					m_food + other.m_food,
					m_wood + other.m_wood,
					m_mineral + other.m_mineral
				};

			}

			Resource operator-(const Resource& other) const {
				return Resource{
					m_food - other.m_food,
					m_wood - other.m_wood,
					m_mineral - other.m_mineral
				};
			}

			Resource& operator+=(const Resource& other) {
				m_food += other.m_food;
				m_wood += other.m_wood;
				m_mineral += other.m_mineral;
				return *this;
			}

			Resource& operator*=(const Resource& other) {
				m_food *= other.m_food;
				m_wood *= other.m_wood;
				m_mineral *= other.m_mineral;
				return *this;
			}

			Resource operator*(const Resource& other) const {
				return Resource{
					m_food * other.m_food,
					m_wood * other.m_wood,
					m_mineral * other.m_mineral
				};
			}
		};

		enum class CatType : int {
			Felis = 0,
			Navi,
			Kone,
			CatTypeMAX // 고양이 종족 수
		};

		enum class BuildingType : int {
			FishingSpot,
			LumberMill,
			Mine
		};

		struct WorkerStats {
			int m_populationCost;            // 인구 비용
			Resource m_resourceBonus[3];     // 자원 보너스 (고양이 타입 * 3)
			Resource m_synergyBonus[MAX_GAME_LEVEL];      // 시너지 보너스 (고양이 타입 * 3)
			Resource m_resourceSubPerSec[MAX_GAME_LEVEL];    // 초당 자원 유지비용 (소모)
		};

		struct BuildingStats {
			Resource m_initResource;							// 건물 설치 시 일회성 획득 자원
			Resource m_upgradeCost[MAX_GAME_LEVEL];             // 업그레이드 비용 * 3레벨 (1레벨은 빌드 비용)
			Resource m_resourceGenPerSec[MAX_GAME_LEVEL];       // 초당 자원 획득량
			Resource m_resourceSubPerSec[MAX_GAME_LEVEL];       // 초당 자원 유지비용 (소모)

			void PrintStats() {
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
		};

		struct HouseStats {
			int m_initPopulation[MAX_GAME_LEVEL];              // 건물 설치 시 일회성 획득 자원 (int 주의, 배열인 이유 : 레벨 당 인구)
			Resource m_upgradeCost[MAX_GAME_LEVEL];          // 업그레이드 비용 * 3레벨 (1레벨은 빌드 비용)
			Resource m_resourceSubPerSec[MAX_GAME_LEVEL];    // 초당 자원 유지비용 (소모)
			void PrintStats() {
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
		};
	}
}
