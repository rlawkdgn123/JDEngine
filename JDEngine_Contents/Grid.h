#pragma once
#include "pch.h"
#include "framework.h"


namespace JDGameObject {
    namespace Content {
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
    }
}