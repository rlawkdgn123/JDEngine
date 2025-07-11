#pragma once
#include "JDWndBase.h"

//#include "D2DTransform.h"
#include "GameTimer.h"

class MainApp : public JDWndBase{
public :
	MainApp() = default;
	virtual ~MainApp() = default;

	bool Initalize();
	void Run();
	void Finalize();

private:

	void UpdateTime();

	void OnResize(int width, int height) override;
	void OnClose() override;
	std::unique_ptr<GameTimer> m_TimerPtr = nullptr;

	//std::shared_ptr<OnlyForTestScene> m_TestScenePtr = nullptr; //씬은 지금 분리해야함
};

