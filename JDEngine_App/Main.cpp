#include "pch.h"
#include "MainApp.h"

using namespace std;

int main(void) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // 메모리 릭(누수) 검사

	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED); // CPU 스레드를 COM 객체와 연결시키기 위한 COM 라이브러리 초기화
	if (FAILED(hr))
		return -1; // -1 실패 신호 반환 후 종료
	
	g_pMainApp = new MainApp();

	cout << "헤헤" << endl;
}