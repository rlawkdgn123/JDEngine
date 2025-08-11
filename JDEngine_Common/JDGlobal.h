#pragma once
#include "pch.h"
#include "framework.h"

namespace JDMath = JDGlobal::Math;
namespace JDGlobal {	
	namespace Base {

		// 인게임 식별용 태그 (현재는 태그 임의)
		enum class GameTag
		{
			None,
			Player,
			PlayerCat,
			PlayerBuilding,
			Enemy,
			EnemyCat,
			EnemyBuilding,
			Barracks,
			BattleAnim,
			Wall,
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
		constexpr int MAX_GAME_RESOURCE = 10000;  // 자원 최대치
		constexpr int MAX_GAME_POPULATION = 1000; // 인구 최대치
		constexpr int MAX_GAME_LEVEL = 10;	 // 업그레이드 최대치
		constexpr int MAX_GAME_RESOURCE_BONUS = 500;	 //보너스 최대치	
		constexpr int MAX_GAME_GRID_COL = 4;
		constexpr int MAX_GAME_GRID_ROW = 6;
		constexpr int MAX_GAME_GRID_MAT = MAX_GAME_GRID_COL * MAX_GAME_GRID_ROW;
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

			void ResourcePrint() const;
			void Clear() {
				m_food = 0;
				m_wood = 0;
				m_mineral = 0;
			}
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

			Resource operator*(const int& num) const {
				return Resource{
					m_food * num,
					m_wood * num,
					m_mineral * num
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
			None = -1,
			Felis,
			Navi,
			Kone,
			CatTypeMAX // 고양이 종족 수
		};

		enum class BuildingType : int {
			House,
			FishingSpot,
			LumberMill,
			Mine,
			Lab
		};
		
		

		struct Cats {
			int Felis;
			int Navi;
			int Kone;
			inline int GetAllCats() {return (Felis + Navi + Kone);}
		};

		struct CatResourceBonus {
			CatResourceBonus()
				: FelisBonus( 10,0,0 ),
				NaviBonus( 0, 10, 0),
				KoneBonus(0, 0, 10) {}

			Resource FelisBonus;
			Resource NaviBonus;
			Resource KoneBonus;
		};

		enum class State { // 상태.
			Idle,
			Move,
			Back
		};

		enum class UnitType : int // 병사 종류.
		{
			Novice = 0, // 견습냥이.
			Expert, // 숙련냥이
			Count
		};

		class UnitTypeData { // 병사 종류 별 데이터. 타입, 필요 자원, 전투력 정보.
		public:
			UnitTypeData(UnitType type = UnitType::Novice, Resource cost = {}, int power = 0)
				: m_unitType(type), m_recruitCost(cost), m_power(power) {
			}

			UnitType GetUnitType() const { return m_unitType; }
			Resource GetRecruitCost() const { return m_recruitCost; }
			int GetPower() const { return m_power; }

		private:
			UnitType m_unitType;
			Resource m_recruitCost;
			int m_power;
		};
		
		// 사용안함
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
			void PrintStats();
		};

		struct HouseStats {
			int m_initPopulation[MAX_GAME_LEVEL];              // 건물 설치 시 일회성 획득 자원 (int 주의, 배열인 이유 : 레벨 당 인구)
			Resource m_upgradeCost[MAX_GAME_LEVEL];          // 업그레이드 비용 * 3레벨 (1레벨은 빌드 비용)
			Resource m_resourceSubPerSec[MAX_GAME_LEVEL];    // 초당 자원 유지비용 (소모)
			void PrintStats();
		};

		struct StartResources {
			int m_food;
			int m_wood;
			int m_mineral;
			int m_population;
			void PrintResources();
		};

	}
}
