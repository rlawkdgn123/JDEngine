#pragma once

#include "ResourceSystem.h"
#include "JDGlobal.h"
namespace JDGameObject {
    namespace Content {
        class Building : public JDGameObject::GameObject
        {
        public:
            using ResourceSystem = JDGameSystem::ResourceSystem;
            using CatType = JDGlobal::Contents::CatType;
        public:
            Building() : GameObject(L"Building")
            {
                m_tag = Tag::PlayerBuilding;
            }

            Building(const std::wstring& name) : GameObject(name)
            {
                m_tag = Tag::PlayerBuilding;
            }

        public:

            virtual void Awake() override;                               // ���� 1ȸ�� ȣ��
            virtual void Start() override;                               // ���� 1ȸ�� ȣ��
            virtual void Update(float deltaTime) override;
            virtual void OnDestroy() override;              // ���� ����

            bool ChangeCat(CatType newCat);
            bool LevelUp();
            bool LevelDown();
            void PrintCat();

        protected:

            JDGlobal::Contents::BuildingStats m_stats;
            int m_curLevel = 0;
            CatType m_cat = CatType::None;
            ResourceSystem* m_resourceSystem;
        };
    }
}