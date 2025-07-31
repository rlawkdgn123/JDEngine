#pragma once
#include "Editor_Clickable.h"

namespace JDGameObject {
	namespace InGame {

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
			void SetColor(const D2D1_COLOR_F& color) { 
				m_color = color; 

				auto uiComponent = GetComponent<UI_ImageComponent>();
				if (uiComponent) {
					uiComponent->SetColor(m_color);
				}
			}
		private:
			Vector2f	m_position = { 0.f, 0.f };
			Vector2f	m_size = { 100.f, 100.f };
			float		m_rotation = { 0.f };
			Vector2f	m_scale = { 1.f, 1.f };

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
			void SetTextFormat(IDWriteTextFormat* format) { 
				m_textFormat = format; 

				auto textComponent = GetComponent<UI_TextComponent>();
				if (textComponent) {
					textComponent->SetTextFormat(m_textFormat.Get());
				}
			}
		private:
			Vector2f	m_position = { 0.f, 0.f };
			Vector2f	m_size = { 100.f, 100.f };
			float		m_rotation = { 0.f };
			Vector2f	m_scale = { 1.f, 1.f };

			std::wstring    m_text = L"Text";
			D2D1_COLOR_F    m_color = D2D1::ColorF(D2D1::ColorF::Black);
			JDComponent::ComPtr<IDWriteTextFormat> m_textFormat;
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
			void SetTextFormat(IDWriteTextFormat* format) {
				m_textFormat = format;

				auto textComponent = GetComponent<UI_TextComponent>();
				if (textComponent) {
					textComponent->SetTextFormat(m_textFormat.Get());
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
			JDComponent::ComPtr<IDWriteTextFormat> m_textFormat;
		};
	}
}

