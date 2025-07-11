#pragma once
/*!
* 메세지
*/

// 함수 선언
LRESULT CALLBACK NzWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

class NzWndBase {
public:
	NzWndBase() = default;
	virtual ~NzWndBase() = default;

	bool Create(const wchar_t* className, const wchar_t* windowName, int width, int height); // 윈도우 생성
	void Destroy();  // 윈도우 제거

	void* GetHandle() const { return m_hWnd; } // 

protected:

	friend LRESULT CALLBACK NzWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam); // friend 선언 (외부 함수 접근 허용)

	virtual void OnResize(int width, int height); // 윈도우 사이즈 재조정
	virtual void OnClose() abstract;

	HWND m_hWnd = nullptr;	// 윈도우 핸들
	int m_width = 0;		// 윈도우 가로넓이
	int m_height = 0;		// 윈도우 세로넓이

	NzWndBase(const NzWndBase&) = delete; // 복사 생성자 제거
	NzWndBase& operator=(const NzWndBase&) = delete; // 복사 대입 연산자 제거
	NzWndBase(NzWndBase&&) = delete; // 이동 생성자 제거
	NzWndBase& operator=(NzWndBase&&) = delete; // 이동 대입 연산자 제거
};
