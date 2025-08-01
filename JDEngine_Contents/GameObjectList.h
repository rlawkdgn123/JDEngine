#pragma once
#include "pch.h"
#include "framework.h"

#include "ResourceSystem.h"
using ResourceSystem = JDGameSystem::ResourceSystem;

namespace JDGameObject{
    namespace Content{
        using Resource = JDGlobal::Contents::Resource;
        using CatType = JDGlobal::Contents::CatType;

        class DefaultObject : public JDGameObject::GameObject {
        public:
            DefaultObject() : GameObject(L"DefaultObject") {}
            DefaultObject(const std::wstring& name) : GameObject(name) {}
            void Awake() override;                               // 최초 1회만 호출
            void Start() override;                               // 최초 1회만 호출
            void Update(float deltaTime) override;               // Update
            void LateUpdate(float deltaTime) override;           // Update 후 호출
            void FixedUpdate(float fixedDeltaTime) override;     // 물리 계산용
            void OnDestroy() override;              // 삭제 직전

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
            void Start() override;                              // 최초 1회만 호출
            void Update(float deltaTime) override;              // Update
            void LateUpdate(float deltaTime) override;          // Update 후 호출
            void FixedUpdate(float fixedDeltaTime) override;    // 물리 계산용
        };

        class Grid : public JDGameObject::GameObject
        {
        public:
            Grid() : GameObject(L"Player") {}
            Grid(const std::wstring& name) : GameObject(name) {}
            void Awake() override;
            void Start() override;                              // 최초 1회만 호출
            void Update(float deltaTime) override;              // Update
            void LateUpdate(float deltaTime) override;          // Update 후 호출
            void FixedUpdate(float fixedDeltaTime) override;    // 물리 계산용
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
            CatType m_catType;              // 고양이 종류
            int m_populationCost;           // 인구 비용
            Resource m_resourceBonus;		// 자원 보너스
            Resource m_synergyBonus;		// 시너지 보너스
            Resource m_resourceSubPerSec;    // 초당 자원 유지비용 (소모)
            ResourceSystem* m_resourceSystem;
        };

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
            virtual void Awake() override;
            virtual void Start() override;
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


