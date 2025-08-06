#include "pch.h"
#include "framework.h"
#include "FishingSpot.h"

using namespace JDComponent;

namespace JDGameObject {
    namespace Content {
        void FishingSpot::Awake()
        {
            __super::Awake();
            DataTableManager::Instance().ParseFishingSpotTable(stats);
            //AddComponent<TextureRenderer>(); µøπŒ¿Ã ºˆ¡§
            stats.PrintStats();
        }
        void FishingSpot::Start()
        {
        }
        void FishingSpot::Update(float deltaTime)
        {
        }
        void FishingSpot::LateUpdate(float deltaTime)
        {
        }
        void FishingSpot::FixedUpdate(float fixedDeltaTime)
        {
        }
        void FishingSpot::OnDestroy()
        {
        }
    }
}