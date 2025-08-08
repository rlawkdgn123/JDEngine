#include "pch.h"
#include "framework.h"
using namespace JDComponent;
using ResourceSystem = JDGameSystem::ResourceSystem;
namespace JDGameObject {
    namespace Content {
        void Building::Awake()
        {
            __super::Awake();
            m_resourceSystem = &ResourceSystem::Instance();
        }

        void Building::Start()
        {
            __super::Start();
            m_resourceSystem->AddTotalResource(-stats.m_upgradeCost[m_nowLevel]);
        }
        void Building::Update(float deltaTime)
        {
            __super::Update(deltaTime);
            if (m_cat == CatType::None) {
                return;
            }
        }
    }
}