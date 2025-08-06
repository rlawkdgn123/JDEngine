#pragma once

#include"GameObjectList.h"
namespace JDGameObject {
    namespace Content {

        class Cat : public JDGameObject::GameObject
        {
        public:
            Cat() : GameObject(L"Cat"), m_catType(CatType::Felis){}
            Cat(const CatType type) : GameObject(L"Cat"), m_catType(type){}
            Cat(const std::wstring& name, const CatType type) : GameObject(name), m_catType(type) {}
        public:
            virtual void Awake() override;
            virtual void Start() override;
        protected:
            CatType m_catType;  // 고양이 종류
            JDGlobal::Contents::WorkerStats stats;
            ResourceSystem* m_resourceSystem;
        };


        class DefaultCat : public Cat
        {
            void Awake() override;
            void Start() override;                              // ���� 1ȸ�� ȣ��
            void Update(float deltaTime) override;              // Update
            void LateUpdate(float deltaTime) override;          // Update �� ȣ��
            void FixedUpdate(float fixedDeltaTime) override;    // ���� ����
        };
    }
}

