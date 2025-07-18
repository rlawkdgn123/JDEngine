#pragma once
#include "pch.h"
#include "framework.h"

namespace JDInterface {

    namespace Scene {
        class SceneBase {
        public:
            virtual ~SceneBase() = default;
            virtual void OnEnter() = 0;
            virtual void OnLeave() = 0;
            virtual void Update(float deltaTime) = 0;
            virtual void Render() = 0;
            virtual const std::string& GetID() const = 0;
        };
    }
    

    class SceneManager
    {
    public:
        using SceneBase = Scene::SceneBase;

        virtual ~SceneManager() = default;

        // 씬 등록, unique_ptr 기반 소유권 넘김
        virtual void RegisterScene(std::unique_ptr<SceneBase> scene) = 0;

        // 씬 전환: id 문자열 기반
        virtual void ChangeScene(const std::string& id) = 0;

        // 매 프레임 씬 업데이트
        virtual void Update(float deltaTime) = 0;

        // 매 프레임 씬 렌더링
        virtual void Render() = 0;

        // 현재 활성 씬 조회 (상수 포인터)
        virtual const SceneBase* GetCurrentScene() const = 0; 
    };
}
