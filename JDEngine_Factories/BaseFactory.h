#pragma once
#include "pch.h"
#include "BaseFactoryFramework.h"


using namespace std;

namespace BaseFactory {
    class GameTimerFactory {
    public:
        static std::unique_ptr<Interface::GameTimer> CreateUnique() {
            return std::make_unique<Module::GameTimer>();
        }

        static std::shared_ptr<Interface::GameTimer> CreateShared() {
            return std::make_shared<Module::GameTimer>();
        }
    };
}
