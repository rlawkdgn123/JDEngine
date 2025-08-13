#include "pch.h"
#include "Framework.h"
#include "EngineCore.h"

using namespace std;

/*
빈 네임스페이스 :

이 전역 변수는 오직 이 .cpp 파일 안에서만 유효

다른 .cpp 파일에 같은 이름의 전역 변수가 있어도 충돌 안 함

전역 스코프 오염을 막음 (링커 오류 방지)
*/
namespace
{
	std::unique_ptr<EngineCore> g_pEngineCore = nullptr;
	
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	//std::wcout이 시스템의 현재 로케일(locale) 설정을 따르도록 설정하는 코드
	std::wcout.imbue(std::locale("")); // 시스템 로케일 자동 적용

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // 메모리 릭(누수) 검사

	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED); // CPU 스레드를 COM 객체와 연결시키기 위한 COM 라이브러리 초기화
	if (FAILED(hr))
		return -1; // -1 실패 신호 반환 후 종료

	//g_pEngineCore = new  EngineCore();
	g_pEngineCore = make_unique<EngineCore>();

	if (!g_pEngineCore->Initialize())
	{
		std::cerr << "Failed to initialize sample code." << std::endl;
		return -1;
	}

	g_pEngineCore->Run();

	g_pEngineCore->Finalize();

	//delete g_pEngineCore; // 스마트ptr이 제거

	CoUninitialize();

	return 0;
}