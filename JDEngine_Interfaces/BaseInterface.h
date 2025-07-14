#pragma once
#include "pch.h"

class IJDWndBase {
public:
	IJDWndBase() = default;
	virtual ~IJDWndBase() = default;

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

	IJDWndBase(const IJDWndBase&) = delete; // ���� ������ ����
	IJDWndBase& operator=(const IJDWndBase&) = delete; // ���� ���� ������ ����
	IJDWndBase(IJDWndBase&&) = delete; // �̵� ������ ����
	IJDWndBase& operator=(IJDWndBase&&) = delete; // �̵� ���� ������ ����
};
