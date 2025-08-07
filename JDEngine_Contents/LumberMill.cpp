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