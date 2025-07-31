#pragma once
#include "pch.h"
#include "framework.h"
namespace JDMath = JDGlobal::Math;

namespace JDGlobal {	
	namespace Base {

		// �ΰ��� �ĺ��� �±� (����� �±� ����)
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
			UI
		};

		// ������ ���� ����� Enum
		enum class SortingLayer : int {
			None,
			BackGround,
			Building,
			Cat,
		};
		
		struct RenderLayerInfo {
			SortingLayer sortingLayer;
			int orderInLayer = 0;

			// operator< �����ε�
			bool operator<(const RenderLayerInfo& other) const {
				if (sortingLayer != other.sortingLayer)
					return sortingLayer < other.sortingLayer;
				return orderInLayer < other.orderInLayer;
			}
		};

	}
	namespace Core {
		// Scenes
		enum class SceneType : int { // ������ �ִ� class ������ // C++11
			None = -1, // ��ȿ���� ���� �ʱ� ����
			SCENE_TEST,
			SCENE_TITLE,
			SCENE_PLAY,
			SCENE_ENDING,
			// ... �� �߰�
			SCENE_MAX // �� ���� ī��Ʈ : �� ���� + 1�� �����̹Ƿ�, ������ ���� �� ī��Ʈ�� �ȴ�.
		};

		constexpr unsigned int START_MAX_OBJECTS = 300;
		constexpr unsigned int MAX_SCENES = static_cast<int>(SceneType::SCENE_MAX); // ���� �ٲܼ���..??

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

		constexpr int OBJECT_NAME_LEN_MAX = 15; // ��ü �̸� �ִ�ġ

		enum class PivotPreset
		{
			TopLeft,
			TopRight,
			BottomLeft,
			BottomRight,
			Center
		};

		// ��ũ�� ������ ��Ŀ. RectTransform�� m_position�� �ش� ��Ŀ �������� +������ �ȴ�.
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
