#pragma once

// 인터페이스 축약어

namespace Window {

	// 함수 선언
	LRESULT CALLBACK JDWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

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
}
