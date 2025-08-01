#pragma once

#include"GameObjectList.h"
namespace JDGameObject {
    namespace Content {

        class Cat : public JDGameObject::GameObject
        {
        public:
            Cat() : GameObject(L"Cat"), m_catType(CatType::Felis),
                m_populationCost(1),
                m_resourceBonus(1.f, 1.f, 1.f),
                m_synergyBonus(1.f, 1.f, 1.f),
                m_resourceSubPerSec(1.f, 1.f, 1.f)
            {
            }
            Cat(const CatType type) : GameObject(L"Cat"), m_catType(type),
                m_populationCost(1),
                m_resourceBonus(1.f, 1.f, 1.f),
                m_synergyBonus(1.f, 1.f, 1.f),
                m_resourceSubPerSec(1.f, 1.f, 1.f)
            {
            }
            Cat(const std::wstring& name, const CatType type) : GameObject(name), m_catType(type),
                m_populationCost(1),
                m_resourceBonus(1.f, 1.f, 1.f),
                m_synergyBonus(1.f, 1.f, 1.f),
                m_resourceSubPerSec(1.f, 1.f, 1.f)
            {
            }
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

