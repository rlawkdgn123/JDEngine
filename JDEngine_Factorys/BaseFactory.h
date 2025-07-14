#pragma once
#include "BaseFactoryFramework.h"
using namespace std;

namespace Factory {
    class GameTimerFactory {
    public:
        static unique_ptr<Interface::GameTimer> Create() {
            return make_unique<GameTimerImpl>();
        }
    };
}
