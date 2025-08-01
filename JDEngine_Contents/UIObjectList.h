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

			void SetPosition(const Vector2f& pos) { 
				m_position = pos; 

				auto rectTransform = GetComponent<RectTransform>();
				if (rectTransform) {
					rectTransform->SetPosition(m_position);
				}
			}
			void SetSize(const Vector2f& size) { 
				m_size = size; 

				auto rectTransform = GetComponent<RectTransform>();
				if (rectTransform) {
					rectTransform->SetSize(m_size);
				}
			}
			void SetRotation(float angle) { 
				m_rotation = angle; 

				auto rectTransform = GetComponent<RectTransform>();
				if (rectTransform) {
					rectTransform->SetRotation(m_rotation);
				}
			}
			void SetScale(const Vector2f& scale) { 
				m_scale = scale; 

				auto rectTransform = GetComponent<RectTransform>();
				if (rectTransform) {
					rectTransform->SetScale(m_scale);
				}
			}

			void SetAnchor(const Vector2f& anchor) {
				m_anchor = anchor;

				auto rectTransform = GetComponent<RectTransform>();
				if (rectTransform) {
					rectTransform->SetAnchor(m_anchor);
				}
			}

			void SetPivot(const Vector2f& pivot) {
				m_pivot = pivot;

				auto rectTransform = GetComponent<RectTransform>();
				if (rectTransform) {
					rectTransform->SetPivot(m_pivot);
				}
			}

			////////////////////////////////////////////////////////////////////////////////

			void SetTextureName(const std::string& name) { 
				m_textureName = name; 

				auto uiComponent = GetComponent<UI_ImageComponent>();
				if (uiComponent) {
					uiComponent->SetTextureName(m_textureName);
				}
			}
			void SetColor(const D2D1_COLOR_F& color) { 
				m_color = color; 

				auto uiComponent = GetComponent<UI_ImageComponent>();
				if (uiComponent) {
					uiComponent->SetColor(m_color);
				}
			}

			////////////////////////////////////////////////////////////////////////////////

			// 원본 크기로 설정하기
			void SetSizeToOriginal() {
				auto imageComponent = GetComponent<UI_ImageComponent>();
				if (imageComponent) {
					imageComponent->SetSizeToOriginal();
				}
			}

			// 원본 크기 가져오기
			Vector2f GetOriginalSize() const {
				auto imageComponent = GetComponent<UI_ImageComponent>();
				if (imageComponent) {
					return imageComponent->GetOriginalSize();
				}
				return m_size;
			}

		private:
			Vector2f	m_size		= { 100.f, 100.f };
			Vector2f	m_position	= { 0.f, 0.f };
			float		m_rotation	= { 0.f };
			Vector2f	m_scale		= { 1.f, 1.f };

			Vector2f	m_pivot		= { 0.5f, 0.5f };
			Vector2f	m_anchor	= { 0.5f, 0.5f };

			//std::string     m_textureName = "Default";
			std::string     m_textureName = "Test";
			D2D1_COLOR_F    m_color = D2D1::ColorF(D2D1::ColorF::White);
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
			void SetPosition(const Vector2f& pos) {
				m_position = pos;

				auto rectTransform = GetComponent<RectTransform>();
				if (rectTransform) {
					rectTransform->SetPosition(m_position);
				}
			}
			void SetSize(const Vector2f& size) {
				m_size = size;

				auto rectTransform = GetComponent<RectTransform>();
				if (rectTransform) {
					rectTransform->SetSize(m_size);
				}
			}
			void SetRotation(float angle) {
				m_rotation = angle;

				auto rectTransform = GetComponent<RectTransform>();
				if (rectTransform) {
					rectTransform->SetRotation(m_rotation);
				}
			}
			void SetScale(const Vector2f& scale) {
				m_scale = scale;

				auto rectTransform = GetComponent<RectTransform>();
				if (rectTransform) {
					rectTransform->SetScale(m_scale);
				}
			}

			void SetText(const std::wstring& text) { 
				m_text = text;

				auto textComponent = GetComponent<UI_TextComponent>();
				if (textComponent) {
					textComponent->SetText(m_text);
				}
			}
			void SetColor(const D2D1_COLOR_F& color) { 
				m_color = color; 

				auto textComponent = GetComponent<UI_TextComponent>();
				if (textComponent) {
					textComponent->SetColor(m_color);
				}
			}
			void SetTextFormatName(const std::string& formatName) {
				m_textFormatName = formatName;
				auto textComponent = GetComponent<UI_TextComponent>();
				if (textComponent) {
					textComponent->SetTextFormatName(m_textFormatName);
				}
			}
		private:
			Vector2f	m_size = { 100.f, 100.f };
			Vector2f	m_position = { 0.f, 0.f };
			float		m_rotation = { 0.f };
			Vector2f	m_scale = { 1.f, 1.f };

			Vector2f	m_pivot = { 0.5f, 0.5f };
			Vector2f	m_anchor = { 0.5f, 0.5f };

			std::wstring    m_text = L"Text";
			D2D1_COLOR_F    m_color = D2D1::ColorF(D2D1::ColorF::Black);
			std::string     m_textFormatName = "MalgunGothic_14";
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
			void SetPosition(const Vector2f& pos) {
				m_position = pos;

				auto rectTransform = GetComponent<RectTransform>();
				if (rectTransform) {
					rectTransform->SetPosition(m_position);
				}
			}
			void SetSize(const Vector2f& size) {
				m_size = size;

				auto rectTransform = GetComponent<RectTransform>();
				if (rectTransform) {
					rectTransform->SetSize(m_size);
				}
			}
			void SetRotation(float angle) {
				m_rotation = angle;

				auto rectTransform = GetComponent<RectTransform>();
				if (rectTransform) {
					rectTransform->SetRotation(m_rotation);
				}
			}
			void SetScale(const Vector2f& scale) {
				m_scale = scale;

				auto rectTransform = GetComponent<RectTransform>();
				if (rectTransform) {
					rectTransform->SetScale(m_scale);
				}
			}

			void SetTextureName(const std::string& name) {
				m_textureName = name;

				auto uiComponent = GetComponent<UI_ImageComponent>();
				if (uiComponent) {
					uiComponent->SetTextureName(m_textureName);
				}
			}
			void SetTextureColor(const D2D1_COLOR_F& color) {
				m_textureColor = color;

				auto uiComponent = GetComponent<UI_ImageComponent>();
				if (uiComponent) {
					uiComponent->SetColor(m_textureColor);
				}
			}

			void SetText(const std::wstring& text) {
				m_text = text;

				auto textComponent = GetComponent<UI_TextComponent>();
				if (textComponent) {
					textComponent->SetText(m_text);
				}
			}
			void SetTextColor(const D2D1_COLOR_F& color) {
				m_textColor = color;

				auto textComponent = GetComponent<UI_TextComponent>();
				if (textComponent) {
					textComponent->SetColor(m_textColor);
				}
			}
			void SetTextFormatName(const std::string& formatName) {
				m_textFormatName = formatName;
				auto textComponent = GetComponent<UI_TextComponent>();
				if (textComponent) {
					textComponent->SetTextFormatName(m_textFormatName);
				}
			}

			////////////////////////////////////////////////////////////////////////////////

			// 원본 크기로 설정하기
			void SetSizeToOriginal() {
				if (!m_textureName.empty()) {

					// 텍스처 매니저나 리소스 매니저에서 크기 가져오기
					ID2D1Bitmap* bitmap = AssetManager::Instance().GetTexture(m_textureName);
					if (!bitmap)
						return;

					auto size = bitmap->GetSize(); // D2D1_SIZE_F

					Vector2f sizeVector;
					sizeVector.x = static_cast<float>(size.width);
					sizeVector.y = static_cast<float>(size.height);

					SetSize(sizeVector);
				}
			}

			// 원본 크기 가져오기
			Vector2f GetOriginalSize() const {
				if (!m_textureName.empty()) {

					// 텍스처 매니저나 리소스 매니저에서 크기 가져오기
					ID2D1Bitmap* bitmap = AssetManager::Instance().GetTexture(m_textureName);
					if (!bitmap)
						return m_size;

					auto size = bitmap->GetSize(); // D2D1_SIZE_F

					Vector2f sizeVector;
					sizeVector.x = static_cast<float>(size.width);
					sizeVector.y = static_cast<float>(size.height);

					return sizeVector;
				}
			}

		private:
			Vector2f	m_position		= { 0.f, 0.f };
			Vector2f	m_size			= { 100.f, 100.f };
			float		m_rotation		= { 0.f };
			Vector2f	m_scale			= { 1.f, 1.f };

			//std::string     m_textureName = "Default";
			std::string     m_textureName = "Test";
			D2D1_COLOR_F    m_textureColor = D2D1::ColorF(D2D1::ColorF::White);

			std::wstring    m_text = L"Text";
			D2D1_COLOR_F    m_textColor = D2D1::ColorF(D2D1::ColorF::Black);
			std::string     m_textFormatName = "MalgunGothic_14";
		};
	}
}