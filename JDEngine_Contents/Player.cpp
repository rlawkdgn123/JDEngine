#include "pch.h"
#include "framework.h"
#include "Player.h"
namespace JDGameObject {
    namespace Content {
        //Player
        void Player::Awake()
        {
            if (!m_isAwaked) {
                m_isAwaked = true;
                //cout << "Player Awake" << endl;
            }
            else { return; }
        }
        void Player::Start() {
            if (!m_isStarted) {
                m_isStarted = true;
                //cout << "Player Start" << endl;
            }
            else { return; }
        }
        void Player::Update(float deltaTime) {}
        void Player::LateUpdate(float deltaTime) {}
        void Player::FixedUpdate(float fixedDeltaTime) {}
    }
}