#pragma once
#include "pch.h"
#include "framework.h"

class DefaultObject : public JDGameObject::GameObject {
    void Start();                   // ���� 1ȸ�� ȣ��
    void Update(float deltaTime);   // Update
    void LateUpdate(float deltaTime); // Update �� ȣ��
    void FixedUpdate(float fixedDeltaTime); // ���� ����
    void OnDestroy();              // ���� ����
    void OnEnable();               // Ȱ��ȭ ��
    void OnDisable();              // ��Ȱ��ȭ ��

    void OnCollisionEnter(GameObjectBase* other);
    void OnCollisionStay(GameObjectBase* other);
    void OnCollisionExit(GameObjectBase* other);

    void OnTriggerEnter(GameObjectBase* other);
    void OnTriggerStay(GameObjectBase* other);
    void OnTriggerExit(GameObjectBase* other);

};

class Player : public JDGameObject::GameObject
{
    void Start();                   // ���� 1ȸ�� ȣ��
    void Update(float deltaTime);   // Update
    void LateUpdate(float deltaTime); // Update �� ȣ��
    void FixedUpdate(float fixedDeltaTime); // ���� ����
};

