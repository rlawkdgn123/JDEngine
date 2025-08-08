#include "pch.h"
#include "framework.h"
#include "Soldier.h"
namespace JDGameObject {
    namespace Content {
        void Soldier::Awake()
        {
            if (!m_isAwaked) {
                m_isAwaked = true;
                //cout << "Player Awake" << endl;
            }
            else { return; }
        }
        void Soldier::Start() {
            if (!m_isStarted) {
                m_isStarted = true;
                //cout << "Player Start" << endl;
            }
            else { return; }
        }
        void Soldier::Update(float deltaTime) {}
        void Soldier::LateUpdate(float deltaTime) {}
        void Soldier::FixedUpdate(float fixedDeltaTime) {}
    }
}