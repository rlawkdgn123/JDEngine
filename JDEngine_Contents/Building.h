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

            virtual void Awake() override;                               // 최초 1회만 호출
            virtual void Start() override;                               // 최초 1회만 호출
            virtual void Update(float deltaTime) override;
            void ChangeCat(CatType catType);
            bool LevelUp();
            bool LevelDown();

        protected:

            JDGlobal::Contents::BuildingStats stats;
            int m_curLevel = 1;
            CatType m_cat = CatType::None;
            float m_secondTimer = 0.f;
            ResourceSystem* m_resourceSystem;
        };
    }
}