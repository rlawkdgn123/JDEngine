#pragma once
/*!
* �޼���
*/

// �Լ� ����
LRESULT CALLBACK NzWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

class NzWndBase {
public:
	NzWndBase() = default;
	virtual ~NzWndBase() = default;

	bool Create(const wchar_t* className, const wchar_t* windowName, int width, int height); // ������ ����
	void Destroy();  // ������ ����

	void* GetHandle() const { return m_hWnd; } // 

protected:

	friend LRESULT CALLBACK NzWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam); // friend ���� (�ܺ� �Լ� ���� ���)

	virtual void OnResize(int width, int height); // ������ ������ ������
	virtual void OnClose() abstract;

	HWND m_hWnd = nullptr;	// ������ �ڵ�
	int m_width = 0;		// ������ ���γ���
	int m_height = 0;		// ������ ���γ���

	NzWndBase(const NzWndBase&) = delete; // ���� ������ ����
	NzWndBase& operator=(const NzWndBase&) = delete; // ���� ���� ������ ����
	NzWndBase(NzWndBase&&) = delete; // �̵� ������ ����
	NzWndBase& operator=(NzWndBase&&) = delete; // �̵� ���� ������ ����
};
