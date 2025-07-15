#pragma once
// Core ������ ��� ���� ����Դϴ�.

namespace CoreGlobal {
	constexpr auto MAX_SCENES = 16;
	constexpr auto START_MAX_OBJECTS = 300;

	// Scenes
	enum class SceneList : int { // ������ �ִ� class ������ // C++11
		None = -1, // ��ȿ���� ���� �ʱ� ����
		SCENE_TITLE,
		SCENE_PLAY,
		SCENE_ENDING,
		// ... �� �߰�
		SCENE_MAX // �� ���� ī��Ʈ : �� ���� + 1�� �����̹Ƿ�, ������ ���� �� ī��Ʈ�� �ȴ�.
	};

	// Games
	//enum ShapeType {
	//	Unknown,
	//	Circle,
	//	Ractangle,
	//	Triangle,
	//	Line
	//};

}

