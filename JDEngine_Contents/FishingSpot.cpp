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
            DataTableManager::Instance().GetFishingSpotTable(m_stats);
            m_stats.PrintStats();
        }
        void FishingSpot::Start()
        {
            __super::Start();
        }
        void FishingSpot::Update(float deltaTime)
        {
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