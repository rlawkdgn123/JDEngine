#include "pch.h"
#include "MainApp.h"

using namespace std;

int main(void) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // �޸� ��(����) �˻�

	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED); // CPU �����带 COM ��ü�� �����Ű�� ���� COM ���̺귯�� �ʱ�ȭ
	if (FAILED(hr))
		return -1; // -1 ���� ��ȣ ��ȯ �� ����
	
	g_pMainApp = new MainApp();

	cout << "����" << endl;
}