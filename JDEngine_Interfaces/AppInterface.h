#pragma once
#include "pch.h"
#include "BaseInterface.h"
#include "../JDEngine_Windows/JDWndBase.h"
namespace Interface {
    class MainApp : public Window::JDWndBase {
    public:
        virtual ~MainApp() = default;

        virtual bool Initialize() = 0;
        virtual void Run() = 0;
        virtual void Finalize() = 0;

        virtual bool OnWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override = 0;

        virtual void OnResize(int width, int height) override = 0;
        virtual void OnClose() override = 0;

        // 보통 인터페이스에는 private 멤버 함수/변수 없음
        // 필요하면 protected 순수 가상 함수로 선언 가능

        // 기타 업데이트, 렌더링 관련 함수도 순수 가상 함수로 선언
        virtual void UpdateTime() = 0;
        virtual void UpdateInput() = 0;
        virtual void UpdateLogic() = 0;

        virtual void Render() = 0;
        virtual void RenderImGUI() = 0;

        virtual void LoadAssets() = 0;

        virtual void BrowseForFolder() = 0;
        virtual void UpdateFileList() = 0;
    };
}