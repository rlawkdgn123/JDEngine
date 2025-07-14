#pragma once
#include "JDWndBase.h"

//#include "D2DTransform.h"
#include "GameTimer.h"

//class MainApp : public JDWndBase{
public :
	MainApp() = default;
	virtual ~MainApp() = default;

	bool Initalize();
	void Run();
	void Finalize();

	bool OnWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override; // 추후 IMGUI 추가 시 살펴보기

private:

	void UpdateTime();
	void UpdateInput();
	void UpdateLogic();

	void Render();
	void RenderImGUI();

	void LoadAssets();

	void OnResize(int width, int height) override;
	void OnClose() override;
	
	void BrowseForFolder();
	void UpdateFileList();

	std::unique_ptr<GameTimer> m_TimerPtr = nullptr;

	GameTimer       m_GameTimer;
	//AssetManager    m_AssetManager;

	std::string     m_pathInput;
	std::wstring    m_folderPath;
	std::wstring    m_selectedFile;

	std::wstring    m_selectedAssetKey;

	//std::vector<SpriteAnimator> m_curSprites;

	std::vector<std::wstring> m_fileList;

	bool m_showFolderPanel = false;
	bool m_bChangedFile = false;

	//std::shared_ptr<OnlyForTestScene> m_TestScenePtr = nullptr; //씬은 지금 분리해야함
};

