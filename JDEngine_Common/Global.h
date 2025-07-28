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
			XStretch,
			YStretch
		};

		struct Anchor
		{
			AnchorPreset m_anchor = AnchorPreset::TopLeft;
			JDMath::Vector2F min;
			JDMath::Vector2F max;


			JDMath::Vector2F GetAnchorOffset(const D2D1_SIZE_F& parentSize) const
			{
				switch (m_anchor)
				{
				case AnchorPreset::TopLeft:
					return { 0.f, 0.f };
				case AnchorPreset::TopCenter:
					return { parentSize.width * 0.5f, 0.f };
				case AnchorPreset::TopRight:
					return { parentSize.width, 0.f };
				case AnchorPreset::CenterLeft:
					return { 0.f, parentSize.height * 0.5f };
				case AnchorPreset::Center:
					return { parentSize.width * 0.5f, parentSize.height * 0.5f };
				case AnchorPreset::CenterRight:
					return { parentSize.width, parentSize.height * 0.5f };
				case AnchorPreset::BottomLeft:
					return { 0.f, parentSize.height };
				case AnchorPreset::BottomCenter:
					return { parentSize.width * 0.5f, parentSize.height };
				case AnchorPreset::BottomRight:
					return { parentSize.width, parentSize.height };
				case AnchorPreset::XStretch:
					return { parentSize.width * 0.5f, 0.f };
				case AnchorPreset::YStretch:
					return { 0.f, parentSize.height * 0.5f };
				default:
					return { 0.f, 0.f };
				}
			}
		};
	}
}
