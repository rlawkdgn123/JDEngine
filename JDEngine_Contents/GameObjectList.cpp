#include "pch.h"
#include "GameObjectList.h"

using namespace std;

namespace JDGameObject {
    namespace InGame {
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


        //Player
        void Player::Awake()
        {
            if (!m_isAwaked) {
                m_isAwaked = true;
                cout << "Player Awake" << endl;
            }
            else { return; }
        }
        void Player::Start() {
            if (!m_isStarted) {
                m_isStarted = true;
                cout << "Player Start" << endl;
            }
            else { return; }
        }                   
        void Player::Update(float deltaTime) {
        
        }
        void Player::LateUpdate(float deltaTime) {}
        void Player::FixedUpdate(float fixedDeltaTime) {}
    
        

    }
}