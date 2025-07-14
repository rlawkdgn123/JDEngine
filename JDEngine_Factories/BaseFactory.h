#pragma once
#include "pch.h"
#include "BaseFactoryFramework.h"
using namespace std;

namespace Factory {
    class GameTimerFactory {
    public:
        static unique_ptr<Interface::GameTimer> Create() {
            return make_unique<Module::GameTimer>();
        }
    };
}
