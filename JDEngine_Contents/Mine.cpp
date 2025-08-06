#include "pch.h"
#include "framework.h"
#include "Mine.h"
using namespace JDComponent;
namespace JDGameObject {
    namespace Content {
        void Mine::Awake()
        {
            __super::Awake();
            DataTableManager::Instance().ParseMineTable(stats);
            stats.PrintStats();
        }
        void Mine::Start()
        {
        }
        void Mine::Update(float deltaTime)
        {
        }
        void Mine::LateUpdate(float deltaTime)
        {
        }
        void Mine::FixedUpdate(float fixedDeltaTime)
        {
        }
        void Mine::OnDestroy()
        {
        }
    }
}