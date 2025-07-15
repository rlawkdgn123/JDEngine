#pragma once
// Core 전반의 상수 모음 헤더입니다.

namespace CoreGlobal {
	constexpr auto MAX_SCENES = 16;
	constexpr auto START_MAX_OBJECTS = 300;

	// Scenes
	enum class SceneList : int { // 범위가 있는 class 열거형 // C++11
		None = -1, // 유효하지 않은 초기 상태
		SCENE_TITLE,
		SCENE_PLAY,
		SCENE_ENDING,
		// ... 씬 추가
		SCENE_MAX // 씬 개수 카운트 : 씬 개수 + 1의 개수이므로, 가져다 쓰면 곧 카운트가 된다.
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

