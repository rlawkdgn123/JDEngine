#include "pch.h"
#include "framework.h"
#include "Mine.h"
using namespace JDComponent;
using namespace std;


namespace JDGameObject {
    namespace Content {
        void Mine::Awake()
        {
            __super::Awake();
            DataTableManager::Instance().GetMineTable(stats);
            stats.PrintStats();
        }
        void Mine::Start()
        {
            __super::Start();
        }
        void Mine::Update(float deltaTime)
        {
            __super::Update(deltaTime);
            m_secondTimer += deltaTime;

            if (m_secondTimer >= 1.f) // 1�� ��������
            {
                m_secondTimer -= 1.f; // ���� �ð� ����

                //�ʴ� �۾� �Է�
                cout << "FishingSpot - 1�ʰ� �������ϴ�..." << endl;

                //m_resourceSystem->AddResourcePerSec(stats.m_resourceGenPerSec[m_nowLevel] * cats.GetAllCats());
                m_resourceSystem->AddResourcePerSec(Resource{ 1,1,1 });
                m_resourceSystem->GetTotalResourcePerSec().ResourcePrint();


            }
        }
        void Mine::LateUpdate(float deltaTime)
        {
            __super::LateUpdate(deltaTime);
        }
        void Mine::FixedUpdate(float fixedDeltaTime)
        {
            __super::FixedUpdate(fixedDeltaTime);
        }
        void Mine::OnDestroy()
        {
            __super::OnDestroy();
        }
    }
}