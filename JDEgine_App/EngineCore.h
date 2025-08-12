#pragma once
#include "JDWndBase.h"

class GameTimer;
class SceneManager;
class D2DRenderer;
class InputManager;


namespace JDWindow {
	class JDWndBase;
}

class EngineCore : public JDWindow::JDWndBase {
public:
public:

	EngineCore() = default;
	virtual ~EngineCore() = default;

	bool Initialize();
	void Run();
	void Finalize();

private:

	void UpdateTime();
	void UpdateLogic();

	void Render(); // 렌더러에 위임할 예정
	void RenderImGui();	// 렌더 마지막 순서에 ImGui가 그려지는 방식.

	void OnResize(int width, int height) override;
	void OnClose() override;

	void LoadResources();

	std::unique_ptr<GameTimer> m_EngineTimer = nullptr;
	//std::unique_ptr<SceneManager> m_SceneManager = nullptr;
	//std::shared_ptr<D2DRenderer> m_Renderer = nullptr;
	//std::shared_ptr<InputManager> m_InputManager = nullptr;
	//std::shared_ptr<AssetManager> m_AssetManager = nullptr;
	std::shared_ptr<Camera> cam = std::make_shared<Camera>();

	//GameTimer       m_GameTimer;
	//AssetManager    m_AssetManager;

	//string     m_pathInput;
	//wstring    m_folderPath;
	//wstring    m_selectedFile;

private:

	D2D1_POINT_2F  m_cameraPosition = { 0.0f, 0.0f };      // 기본 카메라 위치
	float m_cameraRotationDeg = 0.0f;               // 기본 회전 각도 (도 단위)
	float m_cameraZoom = 1.0f;                      // 기본 줌 배율 (1.0 = 100%)
  bool flag = true;

	CameraFader  m_fader;

	std::wstring    m_selectedAssetKey;

	//std::vector<SpriteAnimator> m_curSprites;

	std::vector<std::wstring> m_fileList;

	bool m_showFolderPanel = false;
	bool m_bChangedFile = false;
	float m_FixedTimeAccumulator = 0.0f;

	//std::shared_ptr<OnlyForTestScene> m_TestScenePtr = nullptr; //씬은 지금 분리해야함
	
	// ImGui 활성/비활성
	bool m_showImGui = false;
};



