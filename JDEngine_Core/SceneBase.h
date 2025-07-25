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

        virtual void Update(float deltaTime) {};
            
        virtual void Render() {};
        
        template <typename T>
        inline void CreateGameObject(T&& gameObject);
        
        template <typename T>
        void CreateUIObject(T uiObject);

        SceneType GetType() const { return m_Type; }

        const std::string& GetID() const { return m_ID; }


        //sceneObjects��� GameObjectBase* �����͵��� ��� �ִ� ���͸� ������ �� ����(const) ������(&) ���·� ��ȯ
        //const std::vector<GamebjectBase*>& GetSceneObjects() const { return sceneObjects; };

    protected:

        const SceneType m_Type;
        const std::string m_ID = "None";
        std::vector<std::unique_ptr<GameObjectBase>> m_sceneObjects; // ���� ������ ptr. �˾Ƽ� �޸� ������ ������ �� Ȯ�����ش�.
        std::vector<std::unique_ptr<RenderPresent>> m_presents; // ���� ��Ҹ� ��Ƶ� ����ȭ �迭. sceneObjects���� ������ �ʿ��� ������ �����ؼ� �޾ƿ´�.
        

        int m_objectCount = 0; // �� ���� ��ü ī����
    };
}