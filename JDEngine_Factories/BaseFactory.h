#pragma once
#include "pch.h"
#include "BaseFactoryFramework.h"

namespace BaseFactory {
    class GameTimerFactory {
    public:
        static std::unique_ptr<JDInterface::GameTimer> CreateUnique() {
            return std::make_unique<JDModule::GameTimer>();
        }

        static std::shared_ptr<JDInterface::GameTimer> CreateShared() {
            return std::make_shared<JDModule::GameTimer>();
        }
    };
}
