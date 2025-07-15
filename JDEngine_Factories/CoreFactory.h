#pragma once
#include "pch.h"
#include "CoreFactoryFramework.h"

namespace CoreFactory {
    class D2DRendererFactory {
    public:
        static std::unique_ptr<Interface::D2DRenderer> CreateUnique() {
            return std::make_unique<Module::D2DRenderer>();
        }

        static std::shared_ptr<Interface::D2DRenderer> CreateShared() {
            return std::make_shared<Module::D2DRenderer>();
        }
    };
}  
