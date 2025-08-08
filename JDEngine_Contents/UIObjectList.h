#pragma once
#include "SceneBase.h"
#include "Editor_Clickable.h"
#include "UI_ImageComponent.h"
#include "UI_TextComponent.h"
#include "UI_ButtonComponent.h"
#include "UI_SliderComponent.h"

namespace JDGameObject {
	namespace Content {

		using SceneBase = JDScene::SceneBase;
		using Editor_Clickable = JDComponent::Editor_Clickable;
		using UI_ImageComponent = JDComponent::UI_ImageComponent;
		using UI_TextComponent = JDComponent::UI_TextComponent;
		using UI_ButtonComponent = JDComponent::UI_ButtonComponent;
		using UI_SliderComponent = JDComponent::UI_SliderComponent;

		class Image : public UIObject {
		public:
			using RectTransform = JDComponent::D2DTM::RectTransform;
		public:
			Image() : UIObject(L"Image") {}
			Image(const std::wstring& name) : UIObject(name) {}
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


		class Text : public UIObject {
		public:
			using RectTransform = JDComponent::D2DTM::RectTransform;
		public:
			Text() : UIObject(L"Text") {}
			Text(const std::wstring& name) : UIObject(name) {}
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
			// TextComponent 제어
			////////////////////////////////////////////////////////////////////////////////

			void SetText(const std::wstring& text) { if (auto comp = GetComponent<UI_TextComponent>()) comp->SetText(text); }
			void SetColor(const D2D1_COLOR_F& color) { if (auto comp = GetComponent<UI_TextComponent>()) comp->SetColor(color); }
			void SetTextFormatName(const std::string& formatName) { if (auto comp = GetComponent<UI_TextComponent>()) comp->SetTextFormatName(formatName); }

			// Getter
			std::wstring GetText() const {
				if (auto comp = GetComponent<UI_TextComponent>()) return comp->GetText();
				return L"";
			}

			////////////////////////////////////////////////////////////////////////////////
		};


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
		public:

			////////////////////////////////////////////////////////////////////////////////
			// RectTransform 제어
			////////////////////////////////////////////////////////////////////////////////

			void SetPosition(const Vector2f& pos) {
				if (auto comp = GetComponent<RectTransform>()) comp->SetPosition(pos);
			}
			void SetSize(const Vector2f& size) {
				if (auto comp = GetComponent<RectTransform>()) comp->SetSize(size);
			}
			void SetRotation(float angle) {
				if (auto comp = GetComponent<RectTransform>()) comp->SetRotation(angle);
			}
			void SetScale(const Vector2f& scale) {
				if (auto comp = GetComponent<RectTransform>()) comp->SetScale(scale);
			}

			// Getter
			Vector2f GetPosition() const {
				if (auto comp = GetComponent<RectTransform>()) return comp->GetPosition();
				return {};
			}
			Vector2f GetSize() const {
				if (auto comp = GetComponent<RectTransform>()) return comp->GetSize();
				return {};
			}

			////////////////////////////////////////////////////////////////////////////////
			// ImageComponent 제어
			////////////////////////////////////////////////////////////////////////////////

			void SetTextureName(const std::string& name) {
				if (auto comp = GetComponent<UI_ImageComponent>()) comp->SetTextureName(name);
			}
			void SetTextureColor(const D2D1_COLOR_F& color) {
				if (auto comp = GetComponent<UI_ImageComponent>()) comp->SetColor(color);
			}

			// Getter
			std::string GetTextureName() const {
				if (auto comp = GetComponent<UI_ImageComponent>()) return comp->GetTextureName();
				return "";
			}

			////////////////////////////////////////////////////////////////////////////////
			// TextComponent 제어
			////////////////////////////////////////////////////////////////////////////////

			void SetText(const std::wstring& text) {
				if (auto comp = GetComponent<UI_TextComponent>()) comp->SetText(text);
			}
			void SetTextColor(const D2D1_COLOR_F& color) {
				if (auto comp = GetComponent<UI_TextComponent>()) comp->SetColor(color);
			}
			void SetTextFormatName(const std::string& formatName) {
				if (auto comp = GetComponent<UI_TextComponent>()) comp->SetTextFormatName(formatName);
			}


			////////////////////////////////////////////////////////////////////////////////
			// ButtonComponent 제어
			////////////////////////////////////////////////////////////////////////////////

			// 이벤트 콜백 추가
			void AddOnEnter(const std::string& name, std::function<void()> callback) 
			{ if (auto comp = GetComponent<UI_ButtonComponent>()) comp->AddOnEnter(name, callback); }

			void AddOnExit(const std::string& name, std::function<void()> callback)
			{ if (auto comp = GetComponent<UI_ButtonComponent>()) comp->AddOnExit(name, callback); }

			void AddOnDown(const std::string& name, std::function<void()> callback)
			{ if (auto comp = GetComponent<UI_ButtonComponent>()) comp->AddOnDown(name, callback); }

			void AddOnClick(const std::string& name, std::function<void()> callback) 
			{ if (auto comp = GetComponent<UI_ButtonComponent>()) comp->AddOnClick(name, callback); }

			// 모든 콜백 제거
			void ClearAllCallbacks() { if (auto comp = GetComponent<UI_ButtonComponent>()) comp->ClearAllCallbacks(); }

			// 상호작용 설정
			void SetInteractable(bool isInteractable) { if (auto comp = GetComponent<UI_ButtonComponent>()) comp->SetInteractable(isInteractable); }

			// 상태 및 속성 Getter
			bool IsInteractable() const {
				if (auto comp = GetComponent<UI_ButtonComponent>()) return comp->GetInteractable();
				return false;
			}
			JDComponent::ButtonState GetButtonState() const {
				if (auto comp = GetComponent<UI_ButtonComponent>()) return comp->GetState();
				return JDComponent::ButtonState::Idle;
			}


			////////////////////////////////////////////////////////////////////////////////

			void SetSizeToOriginal() {
				auto imageComp = GetComponent<UI_ImageComponent>();
				if (!imageComp) return;

				std::string textureName = imageComp->GetTextureName();
				if (textureName.empty()) return;

				ID2D1Bitmap* bitmap = AssetManager::Instance().GetTexture(textureName);
				if (!bitmap) return;

				auto size = bitmap->GetSize();
				SetSize({ size.width, size.height });
			}

			Vector2f GetOriginalSize() const {
				auto imageComp = GetComponent<UI_ImageComponent>();
				if (!imageComp) return GetSize();

				std::string textureName = imageComp->GetTextureName();
				if (textureName.empty()) return GetSize();

				ID2D1Bitmap* bitmap = AssetManager::Instance().GetTexture(textureName);
				if (!bitmap) return GetSize();

				auto size = bitmap->GetSize();
				return { size.width, size.height };
			}
		};

		class Slider : public UIObject {
		public:
			using RectTransform = JDComponent::D2DTM::RectTransform;
		public:
			Slider() : UIObject(L"Slider") {}
			Slider(const std::wstring& name) : UIObject(name) {}

		public:
			void Awake() override;
			void Assemble(JDScene::SceneBase* scene);
			void SetActiveSlider(bool active);

			void SetPosition(const Vector2f& pos);
			void SetSize(const Vector2f& size);
			void SetRootSize(const Vector2f& size);

			void SetValue(float value) { if (auto comp = GetComponent<UI_SliderComponent>()) comp->SetValue(value); }
			float GetValue() const { if (auto comp = GetComponent<UI_SliderComponent>()) return comp->GetValue(); return 0.f; }
			void AddOnValueChanged(const std::string& name, std::function<void(float)> callback) { if (auto comp = GetComponent<UI_SliderComponent>()) comp->AddOnValueChanged(name, callback); }

			void SetBackgroundImage(const std::string& textureName);
			void SetFillImage(const std::string& textureName);
			void SetHandleImage(const std::string& textureName);

			void SetBackgroundImageSize(const Vector2f& size);
			void SetHandleImageSize(const Vector2f& size);

			void SetFillImagePosition(const Vector2f& pos);

			void EnableHandleAutoSize(bool enable);

		private:
			bool m_isAssembled = false;
			bool m_isHandleSizeManual = false;

			Image* m_backgroundImage = nullptr;
			Image* m_fillImage = nullptr;
			Image* m_handleImage = nullptr;
		};
	}
}