#pragma once
#include"GameObjectList.h"

namespace JDGameObject{
	namespace Content {
		class DefaultCat : public Cat
		{
			void Awake() override;
			void Start() override;                              // 최초 1회만 호출
			void Update(float deltaTime) override;              // Update
			void LateUpdate(float deltaTime) override;          // Update 후 호출
			void FixedUpdate(float fixedDeltaTime) override;    // 물리 계산용
		};
	}
}


