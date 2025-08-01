#include "pch.h"
#include "Grid.h"

using namespace std;

namespace JDGameObject {
    namespace Content {
        void Grid::Awake()
        {
            if (!m_isAwaked) {
                m_isAwaked = true;
                //cout << "Player Awake" << endl;
            }
            else { return; }
        }
        void Grid::Start() {
            if (!m_isStarted) {
                m_isStarted = true;
                //cout << "Player Start" << endl;
            }
            else { return; }
        }
        void Grid::Update(float deltaTime) {

        }
        void Grid::LateUpdate(float deltaTime) {}
        void Grid::FixedUpdate(float fixedDeltaTime) {}
    }
}