#include "pch.h"
#include "framework.h"
using ResourceSystem = JDGameSystem::ResourceSystem;
namespace JDGameObject {
    namespace Content {
        void Building::Awake()
        {
            m_resourceSystem = &ResourceSystem::Instance();
        }

        void Building::Start()
        {
            m_resourceSystem->AddTotalResource(m_buildCost);
        }
    }
}