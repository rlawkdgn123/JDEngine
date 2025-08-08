#include "pch.h"
#include "framework.h"
#include "FishingSpot.h"

using namespace JDComponent;
using namespace std;

namespace JDGameObject {
    namespace Content {
        void FishingSpot::Awake()
        {
            __super::Awake();
            DataTableManager::Instance().GetFishingSpotTable(stats);

            stats.PrintStats();
        }
        void FishingSpot::Start()
        {
            __super::Start();
            Resource bonus = JDUtil::GetCatBonus(m_cat);
            m_resourceSystem->AddResourcePerSec(stats.m_resourceGenPerSec[m_nowLevel]);
            //m_resourceSystem->
        }
        void FishingSpot::Update(float deltaTime)
        {
            __super::Update(deltaTime);
            m_secondTimer += deltaTime;

            if (m_secondTimer >= 1.f) // 1�� ��������
            {
                m_secondTimer -= 1.f; // ���� �ð� ����

                //�ʴ� �۾� �Է�
                cout << "FishingSpot - 1�ʰ� �������ϴ�..." << endl;

                m_resourceSystem->GetTotalResourcePerSec().ResourcePrint();
            }
        }
        void FishingSpot::LateUpdate(float deltaTime)
        {
            __super::LateUpdate(deltaTime);
        }
        void FishingSpot::FixedUpdate(float fixedDeltaTime)
        {
            __super::FixedUpdate(fixedDeltaTime);
        }
        void FishingSpot::OnDestroy()
        {
            __super::OnDestroy();
        }
    }
}