#pragma once
#include "JDWndBase.h"

 
class GameTimer;
class SceneManager;
class D2DRenderer;
class InputManager;


namespace Window {
	class JDWndBase;
}

class EngineCore : public Window::JDWndBase {
public:
public:


	EngineCore() = default;
	virtual ~EngineCore() = default;

	bool Initialize();
	void Run();
	void Finalize();

	bool OnWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override; // 추후 IMGUI 추가 시 살펴보기
private:

	void UpdateTime();
	void UpdateLogic();

	void Render(); // 렌더러에 위임할 예정

	void OnResize(int width, int height) override;
	void OnClose() override;

	std::unique_ptr<GameTimer> m_EngineTimer = nullptr;
	//std::unique_ptr<SceneManager> m_SceneManager = nullptr;
	std::shared_ptr<D2DRenderer> m_Renderer = nullptr;
	//std::shared_ptr<InputManager> m_InputManager = nullptr;
	std::shared_ptr<ResourceManager> m_ResourceManager = nullptr;

	//GameTimer       m_GameTimer;
	//AssetManager    m_AssetManager;

	//string     m_pathInput;
	//wstring    m_folderPath;
	//wstring    m_selectedFile;

private:
	std::wstring    m_selectedAssetKey;

	//std::vector<SpriteAnimator> m_curSprites;

	std::vector<std::wstring> m_fileList;

	bool m_showFolderPanel = false;
	bool m_bChangedFile = false;
	float m_FixedTimeAccumulator = 0.0f;

	//std::shared_ptr<OnlyForTestScene> m_TestScenePtr = nullptr; //씬은 지금 분리해야함
	
};



