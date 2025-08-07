#pragma once

#include "ResourceSystem.h"
#include "JDGlobal.h"
using ResourceSystem = JDGameSystem::ResourceSystem;
namespace JDGameObject {
    namespace Content {
        class Building : public JDGameObject::GameObject
        {
        public:
            //using ResourceSystem = JDGameSystem::ResourceSystem;
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

        protected:
            JDGlobal::Contents::BuildingStats stats;
            int m_nowLevel = 0;
            int m_catCount = 0;
            JDGlobal::Contents::Cats cats;
            float m_secondTimer = 0.f;
            ResourceSystem* m_resourceSystem;
        };
    }
}