#pragma once
#include "pch.h"
#include "framework.h"
#include "GameObjectBase.h"
#include <memory>

//GameObjectBase; // ���� ����

namespace JDScene {
    using Transform = JDComponent::D2DTM::Transform;
    using ShapeType = JDGlobal::Core::SceneType;

    struct RenderPresent {
        RenderPresent(const Transform& tr, const ShapeType& shape) : tr(tr), shape(shape) {}

    private:
        RenderPresent() = delete;
        const Transform& tr;
        const ShapeType shape;
    public:
        Transform TransformGet() const { return tr; }
        ShapeType ShapeTypeGet() const { return shape; }
    };

    struct CollisionPair { // �浹 ���� ����� ����ü. ���߿� �浹���̳� �ٸ� �����鵵 �Բ� ���� �� ����.
        JDGameObject::GameObjectBase* A = nullptr;
        JDGameObject::GameObjectBase* B = nullptr;
    };

    class SceneBase
    {
    public:
        using SceneType = JDGlobal::Core::SceneType;
        using GameObjectBase =  JDGameObject::GameObjectBase;

        SceneBase(SceneType type, std::string id) : m_Type(type), m_ID(id) {}
        virtual ~SceneBase() {}

    private:

        SceneBase() = delete; // �⺻ ������ ����
        SceneBase(const SceneBase&) = delete; // ���� ������ ����
        SceneBase& operator=(const SceneBase&) = delete; // ���� ������ ����

    public:

        virtual void OnEnter() {};

        virtual void OnLeave() {};
        
        virtual void FixedUpdate(float fixedDeltaTime) { for (auto& obj : m_gameObjects) { if (obj) obj->FixedUpdate(fixedDeltaTime); } };

        virtual void Update(float deltaTime) { for (auto& obj : m_gameObjects) { if (obj) obj->Update(deltaTime); }  CheakCollision();};
        
        virtual void LateUpdate(float deltaTime) { for (auto& obj : m_gameObjects) { if (obj) obj->LateUpdate(deltaTime); } }; // LateUpdate ���� �� ������ ���࿡ �ı� ť ������Ʈ ����

        virtual void Render() {};
        
        template<typename T, typename... Args>
        T* CreateGameObject(Args&&... args);
        
        template <typename T>
        void CreateUIObject(T uiObject);

        void DestroyObject(GameObjectBase* object) {
            if (object == nullptr) { return; }
            if (std::find(m_destroyQueue.begin(), m_destroyQueue.end(), object) == m_destroyQueue.end()) { // ���ͷ����� ������ ��ȸ�ص� m_destroyQueue�� �̹� ���� ������
                m_destroyQueue.push_back(object); // m_destroyQueue�� �߰�
            }
        }

        void ProcessDestroyQueue() { // m_destroyQueue�� ����Ǿ��ִ� ������Ʈ ����
            for (auto& obj : m_destroyQueue) {
                //obj�� m_sceneObjects�� �����ϰ�, obj�� m_destroyQueue�� �����Ѵٸ�
                if (obj) { 
                    obj->OnDestroy(); // �ı� �� OnDestroy()�Լ� ȣ��
                    /*
                       std::remove_if : ���ǿ� �´� ��Ҹ� �ڷ� ����, ���ο� ���� ����
                       std::vector::erase(start, end) : ������ ������ ��Ҹ� ���Ϳ��� ����
                       ��, �ش� �Լ��� remove_if�� ���� ��Ҹ� �ڷ� �� ���ÿ�,
                       �и� ������ ��� ���� �ε������� �� �� �ε������� ����
                    */
                    m_gameObjects.erase(
                        std::remove_if(
                            m_gameObjects.begin(),
                            m_gameObjects.end(),
                            [&](const std::unique_ptr<GameObjectBase>& target) {
                                return target.get() == obj; 
                            }),
                        m_gameObjects.end()
                    );
                }
            }
            m_destroyQueue.clear(); // ������Ʈ �ı� ť�� �� �����ӿ� �� �� ����� ��
        }

        SceneType GetType() const { return m_Type; }

        const std::string& GetID() const { return m_ID; }

        //sceneObjects��� GameObjectBase* �����͵��� ��� �ִ� ���͸� ������ �� ����(const) ������(&) ���·� ��ȯ
        //const std::vector<GamebjectBase*>& GetSceneObjects() const { return sceneObjects; };

        void CheakCollision(); // �浹 üũ

    protected:
        D2D1_COLOR_F m_highlightColor = D2D1::ColorF(D2D1::ColorF::Red);
        int m_highlightedIndex = -1;

        const SceneType m_Type;
        const std::string m_ID = "None";

        std::vector<std::unique_ptr<GameObjectBase>> m_gameObjects; // ���� ������ ptr. �˾Ƽ� �޸� ������ ������ �� Ȯ�����ش�.
        std::vector<std::unique_ptr<RenderPresent>> m_presents; // ���� ��Ҹ� ��Ƶ� ����ȭ �迭. sceneObjects���� ������ �ʿ��� ������ �����ؼ� �޾ƿ´�.
        std::vector<GameObjectBase*> m_destroyQueue; // �����ϰ� ������Ʈ�� �����ϱ� ���� ť. LateUpdate() �� �������� �ش� ť�� ������Ʈ�� �����Ѵ�.
        std::vector<CollisionPair> m_prevPairs; // ���� �浹 ���� ������. �̰��� ���� �� ������Ʈ�� �浹�ϰ� �־����� �� �� ����.
        std::vector<CollisionPair> m_currPairs; // ���� �浹 ���� ������. �浹�� ������Ʈ ���� ������ �����ϰ� ���� �浹 ������ �Ѱ���.

        int m_objectCount = 0; // �� ���� ��ü ī����
    };
}
#include "SceneBase.Inl"