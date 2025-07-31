#pragma once
#include "pch.h"
#include "framework.h"

namespace JDGameObject{
    namespace Content{
        using Resource = JDGlobal::Contents::Resource;
        class DefaultObject : public JDGameObject::GameObject {
        public:
            DefaultObject() : GameObject(L"DefaultObject") {}
            DefaultObject(const std::wstring& name) : GameObject(name) {}
            void Awake() override;                               // ���� 1ȸ�� ȣ��
            void Start() override;                               // ���� 1ȸ�� ȣ��
            void Update(float deltaTime) override;               // Update
            void LateUpdate(float deltaTime) override;           // Update �� ȣ��
            void FixedUpdate(float fixedDeltaTime) override;     // ���� ����
            void OnDestroy() override;              // ���� ����

            void OnCollisionEnter(GameObjectBase* other) override;
            void OnCollisionStay(GameObjectBase* other) override;
            void OnCollisionExit(GameObjectBase* other) override;

            void OnTriggerEnter(GameObjectBase* other) override;
            void OnTriggerStay(GameObjectBase* other) override;
            void OnTriggerExit(GameObjectBase* other) override;

        };

        class Player : public JDGameObject::GameObject
        {
        public:
            Player() : GameObject(L"Player") {}
            Player(const std::wstring& name) : GameObject(name) {}
            void Awake() override;
            void Start() override;                              // ���� 1ȸ�� ȣ��
            void Update(float deltaTime) override;              // Update
            void LateUpdate(float deltaTime) override;          // Update �� ȣ��
            void FixedUpdate(float fixedDeltaTime) override;    // ���� ����
        };

        class Grid : public JDGameObject::GameObject
        {
        public:
            Grid() : GameObject(L"Player") {}
            Grid(const std::wstring& name) : GameObject(name) {}
            void Awake() override;
            void Start() override;                              // ���� 1ȸ�� ȣ��
            void Update(float deltaTime) override;              // Update
            void LateUpdate(float deltaTime) override;          // Update �� ȣ��
            void FixedUpdate(float fixedDeltaTime) override;    // ���� ����
        };
        class Cat : public JDGameObject::GameObject
        {
        public:
            Cat() : GameObject(L"Cat") {}
            Cat(const std::wstring& name) : GameObject(name) {}
        public:
            void Awake() override;
            void Start() override;
        protected:
            int m_populationCost;
            Resource m_CreateCost;               // ���� ���
            Resource m_resourceBonus;		// �ڿ� ���ʽ�
            Resource m_synergyBonus;		// �ó��� ���ʽ�
        };
        class Building : public JDGameObject::GameObject
        {
        public:
            Building() : GameObject(L"Building") {}
            Building(const std::wstring& name) : GameObject(name) {}
        protected:
            Resource m_upgradeCost[3] =         // ���׷��̵� ���
            { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };
            Resource m_buildCost;               // �Ǽ� ���
            Resource m_initResource;            // �ǹ� ��ġ �� ��ȸ�� ȹ�� �ڿ�
            Resource m_resourceGenPerSec;       // �ʴ� �ڿ� ȹ�淮
            Resource m_resourceUpkeepPerSec;    // �ʴ� �ڿ� ������� (�Ҹ�)
        };
    }
}


