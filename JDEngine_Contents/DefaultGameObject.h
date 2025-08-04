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
    }
}


