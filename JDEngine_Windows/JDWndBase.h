﻿#pragma once
#include "pch.h"
// 인터페이스 축약어
#include "imgui.h"
#include "imgui_impl_win32.h"

// ImGui 메시지 처리 함수 선언
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace JDWindow {

	// 함수 선언
	LRESULT CALLBACK JDWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

    class WindowSizeProvider { //컴포넌트 등에 의존성 역전 원칙을 고수하면서 인자를 주입하기 위한 용도 (아직 미사용)
    public:
        virtual ~WindowSizeProvider() = default;
        virtual int GetWidth() const = 0;
        virtual int GetHeight() const = 0;
        virtual void* GetHandle() const = 0;
    };
    class JDWndBase : public WindowSizeProvider{
    public:
        JDWndBase() = default;
        virtual ~JDWndBase() = default;

        // 윈도우 생성
        bool Create(const wchar_t* className, const wchar_t* windowName, int width, int height);

        // 윈도우 제거
        void Destroy();

        // 윈도우 핸들 반환
        void* GetHandle() const { return m_hWnd; }

        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }

    protected:

        // 외부 함수에서 이 클래스의 private/protected 멤버 접근 허용
        friend LRESULT CALLBACK JDWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

        // 윈도우 사이즈 재조정
        virtual void OnResize(int width, int height);

        // 윈도우 종료 처리 - 순수 가상 함수 (추상화)
        virtual void OnClose() = 0;

        HWND m_hWnd = nullptr;  // 윈도우 핸들
        int m_width = 0;        // 윈도우 가로넓이
        int m_height = 0;       // 윈도우 세로넓이

        // 복사 및 이동 연산자 삭제
        JDWndBase(const JDWndBase&) = delete;
        JDWndBase& operator=(const JDWndBase&) = delete;
        JDWndBase(JDWndBase&&) = delete;
        JDWndBase& operator=(JDWndBase&&) = delete;
    };
}
