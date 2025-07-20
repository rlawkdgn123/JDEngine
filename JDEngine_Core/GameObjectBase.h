#pragma once
#include <string>
#include <vector>
#include <memory>
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
class Component; // ���� �߰��ϱ�

namespace JDModule{
	//class GameObjectBase : public JDInterface::GameObjectBase {
	class GameObjectBase {
    protected:
        GameObjectBase() { };   // ���� ��ü ���� �Ұ�, �Ļ������� ����
        GameObjectBase(const GameObjectBase&) = delete;

    public:
        template<typename T, typename... Args>
        T* AddComponent(Args&&... args)
        {
            static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

            auto comp = std::make_unique<T>(std::forward<Args>(args)...);

            comp->SetOwner(this);

            T* ptr = comp.get();

            m_Components.emplace_back(std::move(comp));

            return ptr;
        }

        template<typename T>
        T* GetComponent() const
        {
            for (auto& com : m_Components)
            {
                if (auto ptr = dynamic_cast<T*>(com.get()))
                {
                    return ptr;
                }
            }

            return nullptr;
        }

        void Update(float deltaTime);

        void SendMessage(const CoreGlobal::MessageID msg, void* data = nullptr);

        void SendEvent(const std::string& ev);

        void SetDirty()
        {
            m_dirty = true;
            for (auto* child : m_children)
            {
                child->SetDirty();
            }
        }

    protected:
        
        Compontnts::D2DTM::Transform* m_tr;
        Vector2f m_dir = { 0.0f, 0.0f }; // ���� (���� ����) : �ʿ��Ѱ�? ����غ���

        bool m_dirty; // �ڽ��� �ִ��� ������ ����
        Transform* m_parent; // �θ� ���� ���� // ������Ʈ�� �ٲٱ�
        std::vector<Transform*> m_children; // �ڽ� ���� ����

        // �Է� û���� ���� ������Ʈ�� �߰����� ����غ���.
        std::vector<std::unique_ptr<Component>> m_Components;

    };

	};



}

