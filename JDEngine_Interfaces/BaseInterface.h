#pragma once
#include "pch.h"

namespace Interface {

    /// <summary>
    /// JDWndBase : 윈도우 프로시저 생성용 인터페이스
    /// </summary>
    class JDWndBase {
    public:
        JDWndBase() = default;
        virtual ~JDWndBase() = default;

        // 윈도우 생성
        bool Create(const wchar_t* className, const wchar_t* windowName, int width, int height);

        // 윈도우 제거
        void Destroy();

        // 윈도우 핸들 반환
        void* GetHandle() const { return m_hWnd; }

    protected:
        // 외부 함수에서 이 클래스의 private/protected 멤버 접근 허용
        friend LRESULT CALLBACK JDWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

        // 윈도우 프로시저 (기본 구현: false 반환)
        virtual bool OnWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) { return false; }

        // 윈도우 사이즈 재조정 (기본 구현이 있을 경우 구현)
        virtual void OnResize(int width, int height) { /* 기본 구현 가능 */ }

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


    /// <summary>
    /// Timer.h
    /// </summary>
    class GameTimer {
    public:
        virtual ~GameTimer() = default;

        // 현재까지 흐른 전체 시간(초 단위).
        virtual float TotalTime() const = 0;

        // 한 프레임이 처리되는 데 걸리는 시간.
        virtual float DeltaTime() const = 0;

        // 위와 동일하지만 밀리초 단위로 반환 (추가된 메서드).
        virtual float DeltaTimeMS() const = 0;

        // 타이머 리셋 (기준 시점 초기화)
        virtual void Reset() = 0;

        // 타이머 시작
        virtual void Start() = 0;

        // 타이머 일시 정지
        virtual void Stop() = 0;

        // 프레임마다 호출되어 DeltaTime을 계산
        virtual void Tick() = 0;
    };
}
