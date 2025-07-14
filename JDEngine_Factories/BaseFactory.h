#pragma once
#include "pch.h"
#include "BaseFactoryFramework.h"

using namespace std;

namespace Factory {
    class GameTimerFactory {
    public:
        static shared_ptr<Interface::GameTimer> Create() {
            return make_shared<Module::GameTimer>();
        }
    };
}
