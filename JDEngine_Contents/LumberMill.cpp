#include "pch.h"
#include "framework.h"
#include "LumberMill.h"

using namespace JDComponent;
using namespace std;

namespace JDGameObject {
    namespace Content {
        void LumberMill::Awake()
        {
            __super::Awake();
            DataTableManager::Instance().GetLumberMillTable(stats);
            stats.PrintStats();
        }
        void LumberMill::Start()
        {
            __super::Start();
        }
        void LumberMill::Update(float deltaTime)
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
        void LumberMill::LateUpdate(float deltaTime)
        {
            __super::LateUpdate(deltaTime);
        }
        void LumberMill::FixedUpdate(float fixedDeltaTime)
        {
            __super::FixedUpdate(fixedDeltaTime);
        }
        void LumberMill::OnDestroy()
        {
            __super::OnDestroy();
        }
    }
}