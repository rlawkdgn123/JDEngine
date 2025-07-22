#pragma once
#include "pch.h"
#include "framework.h"
#include <functional>

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
    namespace JDGameObject {
        class GameObjectBase {
        public:
            virtual ~GameObjectBase() = default;

            // 매 프레임 호출되는 업데이트 함수
            virtual void Update(float deltaTime) = 0;

            // 컴포넌트 메시지 전송 (메시지 ID와 임의 데이터)
            virtual void SendComPonentMessage(uint32_t msg, void* data = nullptr) = 0;

            // 컴포넌트 이벤트 전송 (문자열 이벤트명)
            virtual void SendComPonentEvent(const std::string& ev) = 0;

            // 오브젝트 이름 조회
            virtual std::wstring GetName() const = 0;

            // 활성 상태 조회
            virtual bool IsActive() const = 0;

            // 활성 상태 설정
            virtual void SetActive(bool active) = 0;

            // 태그 설정
            virtual void SetTag(const std::wstring& tag) = 0;

            // 태그 조회
            virtual const std::wstring& GetTag() const = 0;

            // 레이어 설정
            virtual void SetLayer(int layer) = 0;

            // 레이어 조회
            virtual int GetLayer() const = 0;
        };
    }

    namespace JDComponent {
        class Component {
            using MessageID = uint32_t; // 추후 따로 둘 예정
            using GameObject = JDInterface::JDGameObject::GameObjectBase;
            using HandlerType = std::function<void(void*)>;
        public:
            virtual ~Component() = default;
            virtual void Update(float dt) = 0;
            virtual void OnEvent(const std::string& ev) = 0;
            virtual void HandleMessage(MessageID msg, void* data) = 0;
            virtual void RegisterMessageHandler(MessageID msg, HandlerType handler) = 0;
            virtual void SetOwner(GameObject* owner) = 0;
        };
    }


}
