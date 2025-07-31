#include "pch.h"
#include "JDWndBase.h" // 윈도우 관련
#include "../JDEngine_Core/InputManager.h"

namespace JDWindow { // 이 네임스페이스를 추가하여 JDWndBase::Create 및 OnResize 정의를 포함합니다.
	LRESULT CALLBACK JDWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		// ImGui 메시지 처리를 맨 먼저!
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
			return true;

		JDWndBase* jdWnd = (JDWndBase*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

		// InputManager에 메시지 넘기기 (키보드, 마우스 메시지 등)
		if (jdWnd && InputManager::Instance().OnHandleMessage({ hwnd, msg, wparam, lparam }))
		{
			// 메시지를 처리했다면 추가 처리하지 말고 바로 리턴
			return 0;
		}

		// friend 키워드로 JDWndBase 접근 가능
		switch (msg)
		{
		case WM_SIZE:	// 창 사이즈 조정
		{
			JDWndBase* pJDWnd = (JDWndBase*)GetWindowLongPtr(hwnd, GWLP_USERDATA); // hwnd 윈도우에 저장된 사용자 데이터(USERDATA) 영역 가져오기
			if (pJDWnd)pJDWnd->OnResize(LOWORD(lparam), HIWORD(lparam)); // Win32 메세지 비트 받아와 윈도우 사이즈 조정
			break;
		}
		case WM_CLOSE:	// 창 닫기
		{
			JDWndBase* pJDwnd = (JDWndBase*)GetWindowLongPtr(hwnd, GWLP_USERDATA); // hwnd 윈도우에 저장된 사용자 데이터(USERDATA) 영역 가져오기
			if (pJDwnd) pJDwnd->OnClose();
			PostQuitMessage(0); // 어플리케이션 종료 요청
			break;
		}
		default:		// 기본값
			return::DefWindowProc(hwnd, msg, wparam, lparam); // 메시지를 직접 처리하지 않을 때, 윈도우에 위임
		}
		return LRESULT();
	}

	bool JDWndBase::Create(const wchar_t* className, const wchar_t* windowName, int width, int height)
	{
		WNDCLASSEX wc = {}; // Win32 API에서 윈도우 클래스(WNDCLASSEX)를 등록(초기화)
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.lpszClassName = className;
		wc.lpfnWndProc = JDWndProc; // 윈도우 프로시저(함수)의 포인터 등록

		ATOM classId = 0;
		if (!GetClassInfoEx(HINSTANCE(), className, &wc))
		{
			classId = RegisterClassEx(&wc);

			if (0 == classId) return false;
		}

		// 프로시저 윈도우 크기 설정
		m_width = width;
		m_height = height;

		RECT rc = { 0, 0, width, height };

		// AdjustWindowRect()
		// Win32 API에서 클라이언트 영역 크기를 기준으로, 실제 윈도우 전체 크기(프레임, 타이틀바 포함)를 계산하는 함수

		AdjustWindowRect(&rc,
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);

		// 윈도우 생성 및 핸들 할당
		m_hWnd = CreateWindowEx(NULL, MAKEINTATOM(classId), L"", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			rc.right - rc.left, rc.bottom - rc.top, HWND(), HMENU(), HINSTANCE(), NULL);

		if (NULL == m_hWnd) return false; // 핸들이 비어있으면 생성 실패

		::SetWindowText((HWND)m_hWnd, windowName);

		SetWindowLongPtr((HWND)m_hWnd, GWLP_USERDATA, (LONG_PTR)this); // 생성한 윈도우에 내가 설정한 객체 포인터를 연결

		ShowWindow((HWND)m_hWnd, SW_SHOW); // 윈도우를 화면에 보이도록 표시
		UpdateWindow((HWND)m_hWnd); // 윈도우 클라이언트 갱신

		return true;
	}

	void JDWndBase::Destroy()
	{
		if (NULL != m_hWnd)
		{
			DestroyWindow((HWND)m_hWnd); // 윈도우 제거
			m_hWnd = NULL;
		}
	}

	void JDWndBase::OnResize(int width, int height)
	{
		// 지정값으로 윈도우 사이즈 재조정
		m_width = width;
		m_height = height;
	}

} // Window 네임스페이스 끝