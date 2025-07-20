#pragma once
// Core ������ ��� ���� ����Դϴ�.

namespace CoreGlobal {

	

	// Scenes
	enum class SceneType : int { // ������ �ִ� class ������ // C++11
		None = -1, // ��ȿ���� ���� �ʱ� ����
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
}

