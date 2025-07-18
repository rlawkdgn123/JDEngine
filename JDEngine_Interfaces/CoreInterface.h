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
}
