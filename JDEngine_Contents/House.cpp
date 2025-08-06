#include "pch.h"
#include "framework.h"
#include "ResourceSystem.h"
#include "House.h"
using namespace JDComponent;

using ResourceSystem = JDGameSystem::ResourceSystem;
namespace JDGameObject {
    namespace Content {
        
        void House::Awake()
        {
            m_resourceSystem = &ResourceSystem::Instance();
            DataTableManager::Instance().ParseHouseTable(stats);
            stats.PrintStats();
        }
        void House::Start()
        {
            m_resourceSystem->AddMaxPopulation(10);
        }
        void House::Update(float deltaTime)
        {
        }
        void House::LateUpdate(float deltaTime)
        {
        }
        void House::FixedUpdate(float fixedDeltaTime)
        {
        }
        void House::OnDestroy()
        {
        }
    }
}
