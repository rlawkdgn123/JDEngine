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
            virtual void Awake() override;                               // 최초 1회만 호출
            virtual void Start() override;                               // 최초 1회만 호출
        protected:
            Resource m_upgradeCost[3] =         // 업그레이드 비용
            { { 0,0,0 }, { 0,0,0 }, { 0,0,0 } };
            Resource m_buildCost;               // 건설 비용
            Resource m_initResource;            // 건물 설치 시 일회성 획득 자원
            Resource m_resourceGenPerSec;       // 초당 자원 획득량
            Resource m_resourceSubPerSec;       // 초당 자원 유지비용 (소모)
            ResourceSystem* m_resourceSystem;
        };
    }
}