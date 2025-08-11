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
            DataTableManager::Instance().GetMineTable(m_stats);
            m_stats.PrintStats();
        }
        void Mine::Start()
        {
            __super::Start();
        }
        void Mine::Update(float deltaTime)
        {
            __super::Update(deltaTime);
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