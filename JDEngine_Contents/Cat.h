#pragma once

#include"GameObjectList.h"

namespace JDGameObject{
	namespace Content {
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



