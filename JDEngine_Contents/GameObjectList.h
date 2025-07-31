#pragma once
#include "pch.h"
#include "framework.h"
#include "ResourceSystem.h"

namespace JDGameObject{
    namespace Content{
        using Resource = JDGlobal::Contents::Resource;
        using CatType = JDGlobal::Contents::CatType;

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
            Cat() : GameObject(L"Cat"), m_catType(CatType::Felis),
                m_populationCost(1), 
                m_resourceBonus(1.f,1.f,1.f),
                m_synergyBonus(1.f,1.f,1.f),
                m_resourceSubPerSec(1.f,1.f,1.f)
            {}
            Cat(const CatType type) : GameObject(L"Cat"), m_catType(type),
                m_populationCost(1),
                m_resourceBonus(1.f, 1.f, 1.f),
                m_synergyBonus(1.f, 1.f, 1.f),
                m_resourceSubPerSec(1.f, 1.f, 1.f)
            {}
            Cat(const std::wstring& name, const CatType type) : GameObject(name), m_catType(type),
                m_populationCost(1),
                m_resourceBonus(1.f, 1.f, 1.f),
                m_synergyBonus(1.f, 1.f, 1.f),
                m_resourceSubPerSec(1.f, 1.f, 1.f)
            {}
        public:
            virtual void Awake() override;
            virtual void Start() override;
        protected:
            CatType m_catType;              // ����� ����
            int m_populationCost;           // �α� ���
            Resource m_resourceBonus;		// �ڿ� ���ʽ�
            Resource m_synergyBonus;		// �ó��� ���ʽ�
            Resource m_resourceSubPerSec;    // �ʴ� �ڿ� ������� (�Ҹ�)
        };

        class Building : public JDGameObject::GameObject
        {
        public:
            //using ResourceSystem = JDGameManager::ResourceSystem;
        public:
            Building() : GameObject(L"Building"), 
                m_buildCost{ 0,0,0 },
                m_initResource{ 0,0,0 },
                m_resourceGenPerSec{ 0,0,0 },
                m_resourceSubPerSec{ 0,0,0 } 
            {
                m_tag = Tag::PlayerBuilding;
            }
            
            Building(const std::wstring& name) : GameObject(name),
                m_buildCost{ 0,0,0 },
                m_initResource{ 0,0,0 },
                m_resourceGenPerSec{ 0,0,0 },
                m_resourceSubPerSec{ 0,0,0 }
            {
                m_tag = Tag::PlayerBuilding;
            }

        protected:
            Resource m_upgradeCost[3] =         // ���׷��̵� ���
            { { 0,0,0 }, { 0,0,0 }, { 0,0,0 } };
            Resource m_buildCost;               // �Ǽ� ���
            Resource m_initResource;            // �ǹ� ��ġ �� ��ȸ�� ȹ�� �ڿ�
            Resource m_resourceGenPerSec;       // �ʴ� �ڿ� ȹ�淮
            Resource m_resourceSubPerSec;    // �ʴ� �ڿ� ������� (�Ҹ�)
        };
    }
}


