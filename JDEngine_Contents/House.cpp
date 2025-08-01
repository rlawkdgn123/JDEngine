#include "pch.h"
#include "ResourceSystem.h"
#include "House.h"

using ResourceSystem = JDGameSystem::ResourceSystem;
namespace JDGameObject {
    namespace Content {
        
        void House::Awake()
        {
            m_resourceSystem = &ResourceSystem::Instance();
        }
        void House::Start()
        {
            m_resourceSystem->AddMaxPopulation(10);
        }
    }
}
