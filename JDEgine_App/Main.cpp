#include "pch.h"
#include "Framework.h"

using namespace std;

/*
�� ���ӽ����̽� :

�� ���� ������ ���� �� .cpp ���� �ȿ����� ��ȿ

�ٸ� .cpp ���Ͽ� ���� �̸��� ���� ������ �־ �浹 �� ��

���� ������ ������ ���� (��Ŀ ���� ����)
*/
namespace
{
	std::unique_ptr<MainApp> g_pMainApp = nullptr;
}

int main(void) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // �޸� ��(����) �˻�

	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED); // CPU �����带 COM ��ü�� �����Ű�� ���� COM ���̺귯�� �ʱ�ȭ
	if (FAILED(hr))
		return -1; // -1 ���� ��ȣ ��ȯ �� ����

	//g_pMainApp = new Module::MainApp();
	g_pMainApp = make_unique<


	if (!g_pMainApp->Initialize())
	{
		std::cerr << "Failed to initialize sample code." << std::endl;
		return -1;
	}

	g_pMainApp->Run();

	g_pMainApp->Finalize();

	//delete g_pMainApp; // ����Ʈptr�� ����

	CoUninitialize();

	return 0;
}