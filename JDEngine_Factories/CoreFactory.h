#pragma once
#include "pch.h"
#include "CoreFactoryFramework.h"
namespace JDScene = JDModule::JDScene;


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
    
    class InputManagerFactory {
    public:
        static std::unique_ptr<JDInterface::InputManager> CreateUnique() {
            return std::make_unique<JDModule::InputManager>();
        }

        static std::shared_ptr<JDInterface::InputManager> CreateShared() {
            return std::make_shared<JDModule::InputManager>();
        }
    };

    namespace JDScene {
        class SceneFactory {
            using SceneType = CoreGlobal::SceneType;
        public:
            static std::unique_ptr<JDInterface::JDScene::SceneBase> CreateUnique(SceneType type, std::string id) {
                switch (type) {
                case SceneType::SCENE_TEST:
                    return std::make_unique<JDModule::JDScene::TestScene>(CoreGlobal::SceneType::SCENE_TEST,id);
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
