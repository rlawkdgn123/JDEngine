#pragma once
/*!
* 메세지
*/

// 함수 선언
LRESULT CALLBACK JDWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

class JDWndBase {
public:
	JDWndBase() = default;
	virtual ~JDWndBase() = default;

	bool Create(const wchar_t* className, const wchar_t* windowName, int width, int height); // 윈도우 생성
	void Destroy();  // 윈도우 제거

	void* GetHandle() const { return m_hWnd; } // 

protected:

	friend LRESULT CALLBACK JDWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam); // friend 선언 (외부 함수 접근 허용)

	virtual bool OnWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) { return false; } // 추후 IMGUI 추가 시 살펴보기

	virtual void OnResize(int width, int height); // 윈도우 사이즈 재조정
	virtual void OnClose() abstract;

	HWND m_hWnd = nullptr;	// 윈도우 핸들
	int m_width = 0;		// 윈도우 가로넓이
	int m_height = 0;		// 윈도우 세로넓이

	JDWndBase(const JDWndBase&) = delete; // 복사 생성자 제거
	JDWndBase& operator=(const JDWndBase&) = delete; // 복사 대입 연산자 제거
	JDWndBase(JDWndBase&&) = delete; // 이동 생성자 제거
	JDWndBase& operator=(JDWndBase&&) = delete; // 이동 대입 연산자 제거
};
