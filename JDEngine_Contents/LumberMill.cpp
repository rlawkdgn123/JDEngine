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
            DataTableManager::Instance().GetLumberMillTable(m_stats);
            m_stats.PrintStats();
        }
        void LumberMill::Start()
        {
            __super::Start();
        }
        void LumberMill::Update(float deltaTime)
        {
            __super::Update(deltaTime);
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