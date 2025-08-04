#pragma once
#include "Editor_Clickable.h"
#include "UI_ImageComponent.h"
#include "UI_TextComponent.h"
#include "UI_ButtonComponent.h"

namespace JDGameObject {
	namespace Content {

		using Editor_Clickable = JDComponent::Editor_Clickable;
		using UI_ImageComponent = JDComponent::UI_ImageComponent;
		using UI_TextComponent = JDComponent::UI_TextComponent;
		using UI_ButtonComponent = JDComponent::UI_ButtonComponent;

		class EmptyMenu : public UIObject {
		public:
			using RectTransform = JDComponent::D2DTM::RectTransform;
		public:
			EmptyMenu() : UIObject(L"EmptyMenu") {}
			EmptyMenu(const std::wstring& name) : UIObject(name) {}
		public:
			void Awake() override;
			void Start() override;                              // 최초 1회만 호출
			void Update(float deltaTime) override;              // Update
			void LateUpdate(float deltaTime) override;          // Update 후 호출
			void FixedUpdate(float fixedDeltaTime) override;    // 물리 계산용
		public:

			////////////////////////////////////////////////////////////////////////////////
			// RectTransform 제어
			////////////////////////////////////////////////////////////////////////////////

			void SetPosition(const Vector2f& pos) { if (auto comp = GetComponent<RectTransform>()) comp->SetPosition(pos); }
			void SetSize(const Vector2f& size) { if (auto comp = GetComponent<RectTransform>()) comp->SetSize(size); }
			void SetRotation(float angle) { if (auto comp = GetComponent<RectTransform>()) comp->SetRotation(angle); }
			void SetScale(const Vector2f& scale) { if (auto comp = GetComponent<RectTransform>()) comp->SetScale(scale); }
			void SetAnchor(const Vector2f& anchor) { if (auto comp = GetComponent<RectTransform>()) comp->SetAnchor(anchor); }
			void SetPivot(const Vector2f& pivot) { if (auto comp = GetComponent<RectTransform>()) comp->SetPivot(pivot); }

			Vector2f GetSize() const {
				if (auto comp = GetComponent<RectTransform>()) return comp->GetSize();
				return {};
			}

			////////////////////////////////////////////////////////////////////////////////
			// ImageComponent 제어
			////////////////////////////////////////////////////////////////////////////////

			void SetTextureName(const std::string& name) { if (auto comp = GetComponent<UI_ImageComponent>()) comp->SetTextureName(name); }
			void SetColor(const D2D1_COLOR_F& color) { if (auto comp = GetComponent<UI_ImageComponent>()) comp->SetColor(color); }

			// 유틸리티 함수
			// 원본 크기로 설정하기
			void SetSizeToOriginal() {
				if (auto imageComponent = GetComponent<UI_ImageComponent>()) {
					imageComponent->SetSizeToOriginal();
				}
			}

			// Getter
			Vector2f GetOriginalSize() const {
				if (auto imageComponent = GetComponent<UI_ImageComponent>()) {
					return imageComponent->GetOriginalSize();
				}
				return GetSize(); // 컴포넌트가 없으면 현재 크기 반환
			}
			std::string GetTextureName() const {
				if (auto comp = GetComponent<UI_ImageComponent>()) return comp->GetTextureName();
				return "";
			}

			////////////////////////////////////////////////////////////////////////////////
		};
	}
}