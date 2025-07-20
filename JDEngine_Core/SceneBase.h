#pragma once
#include "pch.h"
#include "framework.h"
//GameObjectBase; // ���� ����

using namespace CoreGlobal;

namespace JDModule {
    namespace Scene {
        class SceneBase : public JDInterface::Scene::SceneBase
        {
        public:
            SceneBase(SceneType type, std::string id)
                : m_Type(type), m_ID(id) {
            }
            virtual ~SceneBase() {}

        private:

            SceneBase() = delete; // �⺻ ������ ����
            SceneBase(const SceneBase&) = delete; // ���� ������ ����
            SceneBase& operator=(const SceneBase&) = delete; // ���� ������ ����

        public:

            virtual void OnEnter() = 0;

            virtual void OnLeave() = 0;

            virtual void Update(float deltaTime) = 0;

            virtual void Render() = 0; // ������ �������� ���� (�������μ��� ��ǻ� ���� ���� �� ����)

            SceneType GetType() const { return m_Type; }

            const std::string& GetID() const { return m_ID; }

            //sceneObjects��� GameObjectBase* �����͵��� ��� �ִ� ���͸� ������ �� ����(const) ������(&) ���·� ��ȯ
            //const std::vector<GamebjectBase*>& GetSceneObjects() const { return sceneObjects; };

        protected:

            const SceneType m_Type;
            const std::string m_ID = "None";

            std::vector<std::unique_ptr<GameObjectBase>> m_sceneObjects; // ���� ������ ptr. �˾Ƽ� �޸� ������ ������ �� Ȯ�����ش�.
            //vector<unique_ptr<RenderPresent>> m_presents; // ���� ��Ҹ� ��Ƶ� ����ȭ �迭. sceneObjects���� �ʿ��� ������ �����ؼ� �޾ƿ´�.

            int m_objectCount = 0; // �� ���� ��ü ī����
        };
    }
}

