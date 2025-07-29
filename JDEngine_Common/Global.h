#pragma once
#include "pch.h"
#include "framework.h"
namespace JDMath = JDGlobal::Math;

namespace JDGlobal {	

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
