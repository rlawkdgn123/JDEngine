#include "pch.h" // 프리컴파일 헤더
#include "NzWndBase.h" // 윈도우 관련

LRESULT CALLBACK NzWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// friend 키워드로 NzWndBase 접근 가능
	switch (msg) {

	}
	return LRESULT();
}

bool NzWndBase::Create(const wchar_t* className, const wchar_t* windowName, int width, int height)
{
	return false;
}
