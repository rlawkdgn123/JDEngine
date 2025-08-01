#pragma once

#include "ResourceSystem.h"
using ResourceSystem = JDGameSystem::ResourceSystem;
namespace JDGameObject {
    namespace Content {
        class Building : public JDGameObject::GameObject
        {
        public:
            //using ResourceSystem = JDGameSystem::ResourceSystem;
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
        public:
            virtual void Awake() override;                               // ���� 1ȸ�� ȣ��
            virtual void Start() override;                               // ���� 1ȸ�� ȣ��
        protected:
            Resource m_upgradeCost[3] =         // ���׷��̵� ���
            { { 0,0,0 }, { 0,0,0 }, { 0,0,0 } };
            Resource m_buildCost;               // �Ǽ� ���
            Resource m_initResource;            // �ǹ� ��ġ �� ��ȸ�� ȹ�� �ڿ�
            Resource m_resourceGenPerSec;       // �ʴ� �ڿ� ȹ�淮
            Resource m_resourceSubPerSec;       // �ʴ� �ڿ� ������� (�Ҹ�)
            ResourceSystem* m_resourceSystem;
        };
    }
}