#include "pch.h"
#include "framework.h"
#include "Cat.h"
using namespace std;

namespace JDGameObject {
    namespace Content {
        void Cat::Awake()
        {
            m_resourceSystem = &ResourceSystem::Instance();
        }

        void Cat::Start()
        {
            m_resourceSystem->AddCurPopulation(-stats.m_populationCost);
        }
    }
}