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

            if (m_secondTimer >= 1.f) // 1초 지났으면
            {
                m_secondTimer -= 1.f; // 남은 시간 보존

                //초당 작업 입력
                cout << "FishingSpot - 1초가 지났읍니다..." << endl;

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