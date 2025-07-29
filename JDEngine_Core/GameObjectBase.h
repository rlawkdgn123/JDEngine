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
        static ULONG64 idCount; // id ī���� ��� : static++��. ���� ULONG64 Max��ŭ ������Ʈ ���� �� ���� �� ����.

        using Vector2f = JDGlobal::Math::Vector2F;
        using Tag = JDGlobal::Base::GameTag;
        using MessageID = JDGlobal::Core::MessageID;
        using Component = JDComponent::Component;
        using Transform = JDComponent::D2DTM::Transform;
        
    public:
        GameObjectBase(const std::wstring& name) : m_name(name) {}
        virtual ~GameObjectBase() {}

    protected:
        GameObjectBase() = default;
        GameObjectBase(const GameObjectBase&) = delete;

    public:
        template<typename T, typename... Args>
        T* AddComponent(Args&&... args);

        template<typename T>
        T* GetComponent() const; // ���� ������Ʈ�� ���� ���� ��� �޼��� ������Ʈ�� ��ȯ��ų��??

        virtual void Awake() = 0;                    // ������Ʈ�� ó�� ���� ���� ��, Active ������� ������ ����
        virtual void Start() = 0;                    // Ȱ��ȭ�� �� ù �����ӿ���
        virtual void Update(float deltaTime) = 0;    // �� ������ Update
        virtual void LateUpdate(float deltaTime) = 0; // Update �� ȣ��
        virtual void FixedUpdate(float fixedDeltaTime) = 0; // ���� ����
        virtual void OnDestroy() = 0;               // ���� ����

        virtual void OnCollisionEnter(GameObjectBase* other) = 0;
        virtual void OnCollisionStay(GameObjectBase* other) = 0;
        virtual void OnCollisionExit(GameObjectBase* other) = 0;

        virtual void OnTriggerEnter(GameObjectBase* other) = 0;
        virtual void OnTriggerStay(GameObjectBase* other) = 0;
        virtual void OnTriggerExit(GameObjectBase* other) = 0;

        virtual void SendComPonentMessage(const MessageID msg, void* data = nullptr) = 0;
        virtual void SendComPonentEvent(const std::string& ev) = 0;

        std::wstring GetID() const { return std::to_wstring(m_id); }
        std::wstring GetName() const { return m_name; }

        int GetLayer() const { return m_layer; }
    bool IsActive() const { return m_active; }

    void SetTag(const std::wstring& tag) { m_tag = tag; }
    void SetLayer(int layer) { m_layer = layer; }
    void SetActive(bool active) { 
        if (m_active == active) return; // ������ ������ ����
        m_active = active;
        for (auto& comp : m_components) {
            if (active && comp->GetEnabled()) { comp->OnEnable(); }
            else if (!active && comp->GetEnabled()) { comp->OnDisable(); }
        }
    }

    protected:
        std::vector<std::unique_ptr<Component>> m_components; // ������Ʈ ����Ʈ
        Vector2f m_direction = { 0.0f, 0.0f }; // ���� (���� ����) : �ʿ��Ѱ�? ����غ���
        std::wstring m_name;
        Tag m_tag = Tag::None;
        int m_layer = 0;

        int m_id = -1;
        bool m_active = true; // ���� ������Ʈ ��ü�� Ȱ��/��Ȱ��
        bool m_isAwaked = false;
        bool m_isStarted = false;
    };
}

namespace JDGameObject {
    class GameObject : public JDGameObject::GameObjectBase {
    public:
        GameObject() {
            m_name = L"GameObject";
            AddComponent<Transform>();
            m_transform = GetComponent<Transform>();
            m_id = idCount++;
        }

        GameObject(const std::wstring& name) {
            m_name = name;
            AddComponent<Transform>();
            m_transform = GetComponent<Transform>();
            m_id = idCount++;
        }

        virtual void Awake() override { if (m_isAwaked) { return; } m_isAwaked = true; }
        virtual void Start() override { if (m_isStarted || !m_active) { return; } m_isStarted = true;}
        virtual void Update(float deltaTime) override { if (!m_active) { return; } }
        virtual void LateUpdate(float deltaTime) override { if (!m_active) { return; } }
        virtual void FixedUpdate(float fixedDeltaTime) override { if (!m_active) { return; } }
        virtual void OnDestroy() override {}

        virtual void OnCollisionEnter(GameObjectBase* other) override {}
        virtual void OnCollisionStay(GameObjectBase* other) override {}
        virtual void OnCollisionExit(GameObjectBase* other) override {}

        virtual void OnTriggerEnter(GameObjectBase* other) override {}
        virtual void OnTriggerStay(GameObjectBase* other) override {}
        virtual void OnTriggerExit(GameObjectBase* other) override {}

        virtual void SendComPonentMessage(const MessageID msg, void* data = nullptr) override {}
        virtual void SendComPonentEvent(const std::string& ev) override {}

        virtual ~GameObject() = default;

        void SetName(const std::wstring& name) { m_name = name; }
        void SetDirection(const Vector2f& dir) { m_direction = dir; }

    private:
        Transform* m_transform = nullptr; // Ʈ������ RawPointer ( ��� ���� )
    };
}
#include "GameObjectBase.inl"