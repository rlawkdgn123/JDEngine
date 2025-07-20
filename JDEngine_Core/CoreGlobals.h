#pragma once
// Core 전반의 상수 모음 헤더입니다.

namespace CoreGlobal {

	

	// Scenes
	enum class SceneType : int { // 범위가 있는 class 열거형 // C++11
		None = -1, // 유효하지 않은 초기 상태
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
}

