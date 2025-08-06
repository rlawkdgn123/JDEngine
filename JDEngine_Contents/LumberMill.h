#pragma once
#include "GameObjectList.h"

namespace JDGameObject {
    namespace Content {
        class LumberMill : public JDGameObject::Content::Building
        {
        public:
            LumberMill() : Building(L"DefaultObject") {}
            LumberMill(const std::wstring& name) : Building(name) {}
            void Awake() override;                               // ���� 1ȸ�� ȣ��
            void Start() override;                               // ���� 1ȸ�� ȣ��
            void Update(float deltaTime) override;               // Update
            void LateUpdate(float deltaTime) override;           // Update �� ȣ��
            void FixedUpdate(float fixedDeltaTime) override;     // ���� ����
            void OnDestroy() override;              // ���� ����

            //void OnCollisionEnter(GameObjectBase* other) override;
            //void OnCollisionStay(GameObjectBase* other) override;
            //void OnCollisionExit(GameObjectBase* other) override;

            //void OnTriggerEnter(GameObjectBase* other) override;
            //void OnTriggerStay(GameObjectBase* other) override;
            //void OnTriggerExit(GameObjectBase* other) override;
        };
    }
}