#include "pch.h"
#include "framework.h"
#include "LumberMill.h"
using namespace JDComponent;
namespace JDGameObject {
    namespace Content {
        void LumberMill::Awake()
        {
            __super::Awake();
            DataTableManager::Instance().ParseLumberMillTable(stats);
            //AddComponent<TextureRenderer>(); µøπŒ¿Ã ºˆ¡§
            stats.PrintStats();
        }
        void LumberMill::Start()
        {
        }
        void LumberMill::Update(float deltaTime)
        {
        }
        void LumberMill::LateUpdate(float deltaTime)
        {
        }
        void LumberMill::FixedUpdate(float fixedDeltaTime)
        {
        }
        void LumberMill::OnDestroy()
        {
        }
    }
}