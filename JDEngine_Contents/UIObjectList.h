#pragma once

namespace JDGameObject {
	namespace InGame {

		using UI_ImageComponent = JDComponent::UI_ImageComponent;

		class Button : public UIObject {
		public:
			using RectTransform = JDComponent::D2DTM::RectTransform;
		public:
			Button() : UIObject(L"Button") {}
			Button(const std::wstring& name) : UIObject(name) {}
		public:
			void Awake() override;
			void Start() override;                              // 최초 1회만 호출
			void Update(float deltaTime) override;              // Update
			void LateUpdate(float deltaTime) override;          // Update 후 호출
			void FixedUpdate(float fixedDeltaTime) override;    // 물리 계산용
		};
	}
}

