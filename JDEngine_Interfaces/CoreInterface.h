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

        // �� ���, unique_ptr ��� ������ �ѱ�
        virtual void RegisterScene(std::unique_ptr<SceneBase> scene) = 0;

        // �� ��ȯ: id ���ڿ� ���
        virtual void ChangeScene(const std::string& id) = 0;

        // �� ������ �� ������Ʈ
        virtual void Update(float deltaTime) = 0;

        // �� ������ �� ������
        virtual void Render() = 0;

        // ���� Ȱ�� �� ��ȸ (��� ������)
        virtual const SceneBase* GetCurrentScene() const = 0;
    };
    namespace JDGameObject {
        class GameObjectBase {
        public:
            virtual ~GameObjectBase() = default;

            // �� ������ ȣ��Ǵ� ������Ʈ �Լ�
            virtual void Update(float deltaTime) = 0;

            // ������Ʈ �޽��� ���� (�޽��� ID�� ���� ������)
            virtual void SendComPonentMessage(uint32_t msg, void* data = nullptr) = 0;

            // ������Ʈ �̺�Ʈ ���� (���ڿ� �̺�Ʈ��)
            virtual void SendComPonentEvent(const std::string& ev) = 0;

            // ������Ʈ �̸� ��ȸ
            virtual std::wstring GetName() const = 0;

            // Ȱ�� ���� ��ȸ
            virtual bool IsActive() const = 0;

            // Ȱ�� ���� ����
            virtual void SetActive(bool active) = 0;

            // �±� ����
            virtual void SetTag(const std::wstring& tag) = 0;

            // �±� ��ȸ
            virtual const std::wstring& GetTag() const = 0;

            // ���̾� ����
            virtual void SetLayer(int layer) = 0;

            // ���̾� ��ȸ
            virtual int GetLayer() const = 0;
        };
    }

    namespace JDComponent {
        class Component {
            using MessageID = uint32_t; // ���� ���� �� ����
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
