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
		constexpr float MAX_GAME_RESOURCE = 9999;
		constexpr int	MAX_GAME_POPULATION = 999;

		enum class CatType {
			Felis,
			Navi,
			Kone,
		};
		struct Resource {

			Resource() = default;
			Resource(float food, float wood, float mineral)
				: m_food(food), m_wood(wood), m_mineral(mineral) {
			}
			Resource(const Resource& resource) {
				m_food = resource.m_food;
				m_wood = resource.m_wood;
				m_mineral = resource.m_mineral;
			}

			float m_food;
			float m_wood;
			float m_mineral;

			Resource& operator=(const Resource& other) {
				m_food = other.m_food;
				m_wood = other.m_wood;
				m_mineral = other.m_mineral;
				return *this;
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
	}
	
}
