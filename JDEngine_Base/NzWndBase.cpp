#include "pch.h" // ���������� ���
#include "NzWndBase.h" // ������ ����

LRESULT CALLBACK NzWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// friend Ű����� NzWndBase ���� ����
	switch (msg) {

	}
	return LRESULT();
}

bool NzWndBase::Create(const wchar_t* className, const wchar_t* windowName, int width, int height)
{
	return false;
}
