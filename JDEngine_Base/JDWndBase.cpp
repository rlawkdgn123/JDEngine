#include "pch.h" // ���������� ���
#include "JDWndBase.h" // ������ ����

LRESULT CALLBACK JDWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// friend Ű����� JDWndBase ���� ����
	switch (msg) 
	{
	case WM_SIZE:	// â ������ ����
	{
		JDWndBase* pJDWnd = (JDWndBase*)GetWindowLongPtr(hwnd, GWLP_USERDATA); // hwnd �����쿡 ����� ����� ������(USERDATA) ���� ��������
		if (pJDWnd)pJDWnd->OnResize(LOWORD(lparam), HIWORD(lparam)); // Win32 �޼��� ��Ʈ �޾ƿ� ������ ������ ����
		break;
	}
	case WM_CLOSE:	// â �ݱ�
	{
		JDWndBase* pJDwnd = (JDWndBase*)GetWindowLongPtr(hwnd, GWLP_USERDATA); // hwnd �����쿡 ����� ����� ������(USERDATA) ���� ��������
		if (pJDwnd) pJDwnd->OnClose();
		PostQuitMessage(0); // ���ø����̼� ���� ��û
		break;
	}
	default:		// �⺻��
		return::DefWindowProc(hwnd, msg, wparam, lparam); // �޽����� ���� ó������ ���� ��, �����쿡 ����
	}
	return LRESULT();
}

bool JDWndBase::Create(const wchar_t* className, const wchar_t* windowName, int width, int height)
{
	WNDCLASSEX wc = {}; // Win32 API���� ������ Ŭ����(WNDCLASSEX)�� ���(�ʱ�ȭ)
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpszClassName = className;
	wc.lpfnWndProc = JDWndProc; // ������ ���ν���(�Լ�)�� ������ ���

	ATOM classId = 0;
	if (!GetClassInfoEx(HINSTANCE(), className, &wc))
	{
		classId = RegisterClassEx(&wc);

		if (0 == classId) return false;
	}

	// ���ν��� ������ ũ�� ����
	m_width = width;
	m_height = height;

	RECT rc = { 0, 0, width, height };

	// AdjustWindowRect()
	// Win32 API���� Ŭ���̾�Ʈ ���� ũ�⸦ ��������, ���� ������ ��ü ũ��(������, Ÿ��Ʋ�� ����)�� ����ϴ� �Լ�

	// AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false); // �⺻ ����. �Ʒ� �ڵ�� ��������/�ִ�ȭ�� �����ִ�. ��??
	AdjustWindowRect(&rc,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);

	// ������ ���� �� �ڵ� �Ҵ�
	m_hWnd = CreateWindowEx(NULL, MAKEINTATOM(classId), L"", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right - rc.left, rc.bottom - rc.top, HWND(), HMENU(), HINSTANCE(), NULL);

	if (NULL == m_hWnd) return false; // �ڵ��� ��������� ���� ����

	::SetWindowText((HWND)m_hWnd, windowName);

	SetWindowLongPtr((HWND)m_hWnd, GWLP_USERDATA, (LONG_PTR)this); // ������ �����쿡 ���� ������ ��ü �����͸� ����

	ShowWindow((HWND)m_hWnd, SW_SHOW); // �����츦 ȭ�鿡 ���̵��� ǥ��
	UpdateWindow((HWND)m_hWnd); // ������ Ŭ���̾�Ʈ ����

	return true;
}

void JDWndBase::Destroy() 
{
	if (NULL != m_hWnd)
	{
		DestroyWindow((HWND)m_hWnd); // ������ ����
		m_hWnd = NULL;
	}
}

void JDWndBase::OnResize(int width, int height)
{
	// ���������� ������ ������ ������
	m_width = width;
	m_height = height;
}
