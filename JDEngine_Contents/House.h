#pragma once
#include "GameObjectBase.h"

namespace JDGameSystem {
    class ResourceSystem;
}


namespace JDGameObject {
    namespace Content {
        class House : public JDGameObject::GameObject
        {
        public:
            using ResourceSystem = JDGameSystem::ResourceSystem;
            using Resource = JDGlobal::Contents::Resource;
        public:
            House() : GameObject(L"Building") { m_tag = Tag::PlayerBuilding; }
            House(const std::wstring& name) : GameObject(name) { m_tag = Tag::PlayerBuilding; }
        public:
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

        protected:
            JDGlobal::Contents::HouseStats stats;
            ResourceSystem* m_resourceSystem;
        };
    }
}
