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

        // ���� �������̽����� private ��� �Լ�/���� ����
        // �ʿ��ϸ� protected ���� ���� �Լ��� ���� ����

        // ��Ÿ ������Ʈ, ������ ���� �Լ��� ���� ���� �Լ��� ����
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