#pragma once
#include "pch.h"
#include "framework.h"

class DefaultObject : public JDGameObject::GameObject {
    void Start();                   // 최초 1회만 호출
    void Update(float deltaTime);   // Update
    void LateUpdate(float deltaTime); // Update 후 호출
    void FixedUpdate(float fixedDeltaTime); // 물리 계산용
    void OnDestroy();              // 삭제 직전
    void OnEnable();               // 활성화 시
    void OnDisable();              // 비활성화 시

    void OnCollisionEnter(GameObjectBase* other);
    void OnCollisionStay(GameObjectBase* other);
    void OnCollisionExit(GameObjectBase* other);

    void OnTriggerEnter(GameObjectBase* other);
    void OnTriggerStay(GameObjectBase* other);
    void OnTriggerExit(GameObjectBase* other);

};

class Player : public JDGameObject::GameObject
{
    void Start();                   // 최초 1회만 호출
    void Update(float deltaTime);   // Update
    void LateUpdate(float deltaTime); // Update 후 호출
    void FixedUpdate(float fixedDeltaTime); // 물리 계산용
};

