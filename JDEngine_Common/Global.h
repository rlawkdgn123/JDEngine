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
			Enemy,
			NPC,
			UI
		};

		// 렌더링 순서 제어용 Enum
		enum class SortingLayer : int {
			None,
			BackGround,
			Architecture,
			Cat,
		};
		
		struct RenderLayerInfo { 
			SortingLayer sortingLayer;
			int orderInLayer = 0;

			// 렌더링 정렬 기준
			bool Compare(const RenderLayerInfo& a, const RenderLayerInfo& b) {
				if (a.sortingLayer != b.sortingLayer)
					return a.sortingLayer < b.sortingLayer; // enum값이 낮을수록 아래에 그림
				return a.orderInLayer < b.orderInLayer;     // 같은 그룹 내에서는 정렬 순서로
			}
		};

	}
	namespace Core {
		// Scenes
		enum class SceneType : int { // 범위가 있는 class 열거형 // C++11
			None = -1, // 유효하지 않은 초기 상태
			SCENE_TEST,
			SCENE_TITLE,
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
			TopRight,
			BottomLeft,
			BottomRight,
			Center
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
}
