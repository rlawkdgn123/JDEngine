#pragma once
#include "pch.h"
#include "Component.h"
#include "D2DTransform.h"
#include "ComponentList.h"
#include "Global.h"
#include "framework.h"
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


  
namespace JDGameObject {
    class GameObjectBase {
    public:
        using Vector2f = JDGlobal::Math::Vector2F;
        using MessageID = JDGlobal::Core::MessageID;
        using Component = JDComponent::Component;
        using Transform = JDComponent::D2DTM::Transform;
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

        virtual void Start();                   // ���� 1ȸ�� ȣ��
        virtual void Update(float deltaTime);   // Update
        virtual void LateUpdate(float deltaTime); // Update �� ȣ��
        virtual void FixedUpdate(float fixedDeltaTime); // ���� ����
        virtual void OnDestroy();              // ���� ����
        virtual void OnEnable();               // Ȱ��ȭ ��
        virtual void OnDisable();              // ��Ȱ��ȭ ��

        virtual void OnCollisionEnter(GameObjectBase* other);
        virtual void OnCollisionStay(GameObjectBase* other);
        virtual void OnCollisionExit(GameObjectBase* other);

        virtual void OnTriggerEnter(GameObjectBase* other);
        virtual void OnTriggerStay(GameObjectBase* other);
        virtual void OnTriggerExit(GameObjectBase* other);

        virtual void SendComPonentMessage(const MessageID msg, void* data = nullptr);
        virtual void SendComPonentEvent(const std::string& ev);

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


namespace JDGameObject {
    class GameObject : public  JDGameObject::GameObjectBase {
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
#include "GameObjectBase.inl"