#pragma once
#include "pch.h"
#include "framework.h"

using Tag = JDGlobal::Base::GameTag;
namespace JDGameObject{
    namespace InGame{
        class DefaultObject : public JDGameObject::GameObject {
        
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
            Cat() : GameObject(L"Cat") {}
            Cat(const std::wstring& name) : GameObject(name) {}
            virtual void Awake() override;
            virtual void Start() override;                              // 최초 1회만 호출
            virtual void Update(float deltaTime) override;              // Update
            virtual void LateUpdate(float deltaTime) override;          // Update 후 호출
            virtual void FixedUpdate(float fixedDeltaTime) override;    // 물리 계산용
        };

        class Architecture : public JDGameObject::GameObject
        {
        public:
            Architecture() : GameObject(L"Cat") {}
            Architecture(const std::wstring& name) : GameObject(name) {}
            virtual void Awake() override;
            virtual void Start() override;                              // 최초 1회만 호출
            virtual void Update(float deltaTime) override;              // Update
            virtual void LateUpdate(float deltaTime) override;          // Update 후 호출
            virtual void FixedUpdate(float fixedDeltaTime) override;    // 물리 계산용
        };

    }
}


