#include "pch.h"
#include "GameObjectList.h"

using namespace std;

namespace JDGameObject {
    namespace Content {
        void DefaultObject::Awake()
        {
            if (!m_isAwaked) {
                m_isAwaked = true;
            }
            else { return; }
        }

        void DefaultObject::Start()
        {
            if (!m_isStarted) {
                m_isStarted = true;
            }
            else { return; }
        }

        void DefaultObject::Update(float deltaTime)
        {
        }

        void DefaultObject::LateUpdate(float deltaTime)
        {
        }

        void DefaultObject::FixedUpdate(float fixedDeltaTime)
        {
        }

        void DefaultObject::OnDestroy()
        {
        }

    }
}