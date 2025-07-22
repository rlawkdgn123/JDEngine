#pragma once
#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <tuple>
#include "ComponentList.h"

// Ŭ���� ���� ���赵
/*
Ŭ���� ���� ���赵 (���� ���� ���� �̰� �ƴ�)
=============================================================
                GameObjectBase  (�߻� �⺻ Ŭ����)
                       ��
                GeometryObject      (�⺻ ��ü ��� ����)
                       ��
        +---------------+---------------+
        |                               |
    RectangleObject					CircleObject
    (�簢�� ��ü)					 (�� ��ü)
=============================================================
*/



namespace JDModule{
    namespace JDGameObject {
        class GameObjectBase : public JDInterface::JDGameObject::GameObjectBase {
        public:
            using Component = JDModule::JDComponent::Component;
            using Transform = JDModule::JDComponent::D2DTM::Transform;
            using Vector2f = JDGlobal::Math::Vector2F;
            using MessageID = uint32_t;
        public:
            virtual ~GameObjectBase() {}

        protected:
            GameObjectBase() = default;
            GameObjectBase(const GameObjectBase&) = delete;

        public:
            template<typename T, typename... Args>
            T* AddComponent(Args&&... args);

            template<typename T>
            T* GetComponent() const; // ���� ������Ʈ�� ���� ���� ��� �޼��� ������Ʈ�� ��ȯ��ų��??

            virtual void Update(float deltaTime) override;

            virtual void SendComPonentMessage(const MessageID msg, void* data = nullptr) override;
            virtual void SendComPonentEvent(const std::string& ev) override;

            std::wstring GetName() const { return m_name; }

            bool IsActive() const { return m_active; }
            void SetActive(bool active) { m_active = active; }

            void SetTag(const std::wstring& tag) { m_tag = tag; }
            const std::wstring& GetTag() const { return m_tag; }

            void SetLayer(int layer) { m_layer = layer; }
            int GetLayer() const { return m_layer; }

        protected:

            std::vector<std::unique_ptr<Component>> m_components; // ������Ʈ ����Ʈ
            Vector2f m_direction = { 0.0f, 0.0f }; // ���� (���� ����) : �ʿ��Ѱ�? ����غ���
            std::wstring m_name;
            std::wstring m_tag = L"";
            int m_layer = 0;
            bool m_active = true;

        };
    }
}

namespace JDModule {
    namespace JDGameObject {
        class GameObject : public JDModule::JDGameObject::GameObjectBase {
        public:
            GameObject() { AddComponent<Transform>(); };
            GameObject(Vector2f pos) { AddComponent<Transform>(pos); }

            virtual ~GameObject() = default;

            // �Ļ����� �ʿ��� Ưȭ ��� ����
            void SetName(const std::wstring& name) { m_name = name; }
            void SetDirection(const Vector2f& dir) { m_direction = dir; }

            //tuple RenderPresentGet() override;
            // Ȥ�� �߰� Update �������̵� ����
        protected:
        };
    }
}
#include "GameObjectBase.inl"