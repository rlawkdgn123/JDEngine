#pragma once
/*!
* �޼���
*/

// �Լ� ����
LRESULT CALLBACK JDWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

class JDWndBase {
public:
	JDWndBase() = default;
	virtual ~JDWndBase() = default;

	bool Create(const wchar_t* className, const wchar_t* windowName, int width, int height); // ������ ����
	void Destroy();  // ������ ����

	void* GetHandle() const { return m_hWnd; } // 

protected:

	friend LRESULT CALLBACK JDWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam); // friend ���� (�ܺ� �Լ� ���� ���)

	virtual bool OnWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) { return false; } // ���� IMGUI �߰� �� ���캸��

	virtual void OnResize(int width, int height); // ������ ������ ������
	virtual void OnClose() abstract;

	HWND m_hWnd = nullptr;	// ������ �ڵ�
	int m_width = 0;		// ������ ���γ���
	int m_height = 0;		// ������ ���γ���

	JDWndBase(const JDWndBase&) = delete; // ���� ������ ����
	JDWndBase& operator=(const JDWndBase&) = delete; // ���� ���� ������ ����
	JDWndBase(JDWndBase&&) = delete; // �̵� ������ ����
	JDWndBase& operator=(JDWndBase&&) = delete; // �̵� ���� ������ ����
};
