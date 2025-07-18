#pragma once
#include "pch.h"
#include "CoreFactoryFramework.h"

namespace CoreFactory {
    class D2DRendererFactory {
    public:
        static std::unique_ptr<JDInterface::D2DRenderer> CreateUnique() {
            return std::make_unique<JDModule::D2DRenderer>();
        }

        static std::shared_ptr<JDInterface::D2DRenderer> CreateShared() {
            return std::make_shared<JDModule::D2DRenderer>();
        }
    };

    class SceneManagerFactory {
    public:
        static std::unique_ptr<JDInterface::SceneManager> CreateUnique() {
            return std::make_unique<JDModule::SceneManager>();
        }

        static std::shared_ptr<JDInterface::SceneManager> CreateShared() {
            return std::make_shared<JDModule::SceneManager>();
        }
    };

    namespace Scene {
        class SceneFactory {
            using SceneType = CoreGlobal::SceneType;
            using JDScene = JDModule::Scene;
        public:
            static std::unique_ptr<JDInterface::Scene::SceneBase> CreateUnique(SceneType type) {
                switch (type) {
                case SceneType::SCENE_TITLE:
                    //return std::make_unique<JDScene::SceneTitle>();
                case SceneType::SCENE_PLAY:
                    //return std::make_unique<SceneGameplay>();
                case SceneType::SCENE_ENDING:
                    //return std::make_unique<SceneEnding>();
                default:
                    return nullptr;
                }
            }
        };
    }
}  
