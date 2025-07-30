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
			Enemy,
			NPC,
			UI
		};

		// ������ ���� ����� Enum
		enum class SortingLayer : int {
			None,
			BackGround,
			Architecture,
			Cat,
		};
		
		struct RenderLayerInfo { 
			SortingLayer sortingLayer;
			int orderInLayer = 0;

			// ������ ���� ����
			bool Compare(const RenderLayerInfo& a, const RenderLayerInfo& b) {
				if (a.sortingLayer != b.sortingLayer)
					return a.sortingLayer < b.sortingLayer; // enum���� �������� �Ʒ��� �׸�
				return a.orderInLayer < b.orderInLayer;     // ���� �׷� �������� ���� ������
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
}
