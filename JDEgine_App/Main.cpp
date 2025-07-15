#include "pch.h"
#include "Framework.h"
#include "EngineCore.h"

using namespace std;

/*
�� ���ӽ����̽� :

�� ���� ������ ���� �� .cpp ���� �ȿ����� ��ȿ

�ٸ� .cpp ���Ͽ� ���� �̸��� ���� ������ �־ �浹 �� ��

���� ������ ������ ���� (��Ŀ ���� ����)
*/
namespace
{
	std::unique_ptr<EngineCore> g_pEngineCore = nullptr;
	
}

int main(void) {

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // �޸� ��(����) �˻�

	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED); // CPU �����带 COM ��ü�� �����Ű�� ���� COM ���̺귯�� �ʱ�ȭ
	if (FAILED(hr))
		return -1; // -1 ���� ��ȣ ��ȯ �� ����

	//g_pEngineCore = new Module::EngineCore();
	g_pEngineCore = make_unique<EngineCore>();

	if (!g_pEngineCore->Initialize())
	{
		std::cerr << "Failed to initialize sample code." << std::endl;
		return -1;
	}

	g_pEngineCore->Run();

	g_pEngineCore->Finalize();

	//delete g_pEngineCore; // ����Ʈptr�� ����

	CoUninitialize();

	return 0;
}