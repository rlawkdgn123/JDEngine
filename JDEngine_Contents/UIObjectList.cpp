#include "pch.h"
#include "framework.h"

using namespace std;
using RectTransform = JDComponent::D2DTM::RectTransform;
using PivotPreset = JDComponent::D2DTM::RectTransform::PivotPreset;

namespace JDGameObject {
	namespace Content {
		
		////////////////////////////////////////////////////////////////////////////////
		// Image
		////////////////////////////////////////////////////////////////////////////////

		void Image::Awake()
		{
			if (!m_active) { return; }

			if (!m_isAwaked) {
				m_isAwaked = true;
				// cout << "Image Awake" << endl;

				////////////////////////////////////////////////////////////////////////////////
				// Editor_Clickable
				AddComponent<Editor_Clickable>();

				////////////////////////////////////////////////////////////////////////////////
				// RectTransform
				auto rectTransform = GetComponent<RectTransform>();
				if (rectTransform)
				{
					D2D1_SIZE_F size = { m_size.x, m_size.y };
					rectTransform->SetSize(m_size);
					rectTransform->SetPivot(m_size);

					rectTransform->SetPosition(m_position);
					rectTransform->SetRotation(m_rotation);
					rectTransform->SetScale(m_scale);
				}

				////////////////////////////////////////////////////////////////////////////////
				// UI_ImageComponent
				AddComponent<UI_ImageComponent>("Test");
				auto imageComponent = GetComponent<JDComponent::UI_ImageComponent>();
				if (imageComponent) {
					imageComponent->SetTextureName(m_textureName);
					imageComponent->SetColor(m_color);
				}
			}
		}
		void Image::Start()
		{
			if (!m_active) { return; }

			if (!m_isStarted) {
				m_isStarted = true;
				// cout << "Image Start" << endl;
			}
		}
		void Image::Update(float deltaTime)
		{
			if (!m_active) { return; }
			// __super::Update(deltaTime);
		}
		void Image::LateUpdate(float deltaTime)
		{
			if (!m_active) { return; }
			// __super::LateUpdate(deltaTime);
		}
		void Image::FixedUpdate(float fixedDeltaTime)
		{
			if (!m_active) { return; }
			// __super::FixedUpdate(fixedDeltaTime);
		}



		////////////////////////////////////////////////////////////////////////////////
		// Text
		////////////////////////////////////////////////////////////////////////////////

		void Text::Awake()
		{
			if (!m_isAwaked) {
				m_isAwaked = true;
				// cout << "Button Awake" << endl;

				////////////////////////////////////////////////////////////////////////////////
				// Editor_Clickable
				AddComponent<Editor_Clickable>();

				////////////////////////////////////////////////////////////////////////////////
				// RectTransform
				auto rectTransform = GetComponent<RectTransform>();
				if (rectTransform)
				{
					rectTransform->SetPosition(m_position);
					rectTransform->SetSize(m_size);
					rectTransform->SetRotation(m_rotation);
					rectTransform->SetScale(m_scale);
				}

				////////////////////////////////////////////////////////////////////////////////
				// UI_TextComponent
				AddComponent<JDComponent::UI_TextComponent>();
				auto textComponent = GetComponent<JDComponent::UI_TextComponent>();

				D2DRenderer& renderer = D2DRenderer::Instance();
				if (textComponent) {
					textComponent->SetText(m_text);
					textComponent->SetColor(m_color);
					textComponent->SetTextFormatName(m_textFormatName);

					D2DRenderer& renderer = D2DRenderer::Instance();
					const auto& textFormats = renderer.GetTextFormats();
					auto it = textFormats.find(m_textFormatName);
					if (it == textFormats.end()) {
						// 원하는 포맷이 없으면 기본 포맷 사용
						textComponent->SetTextFormatName("MalgunGothic_14");
					}
				}
			}
		}
		void Text::Start()
		{
			if (!m_isStarted) {
				m_isStarted = true;
				// cout << "Image Start" << endl;
			}
		}
		void Text::Update(float deltaTime)
		{
			// __super::Update(deltaTime);
		}
		void Text::LateUpdate(float deltaTime)
		{
			// __super::LateUpdate(deltaTime);
		}
		void Text::FixedUpdate(float fixedDeltaTime)
		{
			// __super::FixedUpdate(fixedDeltaTime);
		}

		////////////////////////////////////////////////////////////////////////////////
		// Button
		////////////////////////////////////////////////////////////////////////////////

		void Button::Awake()
		{
			if (!m_isAwaked) {
				m_isAwaked = true;
				// cout << "Button Awake" << endl;

				////////////////////////////////////////////////////////////////////////////////
				// Editor_Clickable
				AddComponent<Editor_Clickable>();

				////////////////////////////////////////////////////////////////////////////////
				// RectTransform
				auto rectTransform = GetComponent<RectTransform>();
				if (rectTransform)
				{
					rectTransform->SetPosition(m_position);
					rectTransform->SetSize(m_size);
					rectTransform->SetRotation(m_rotation);
					rectTransform->SetScale(m_scale);
				}

				////////////////////////////////////////////////////////////////////////////////
				// UI_ImageComponent
				AddComponent<UI_ImageComponent>("Test");
				auto imageComponent = GetComponent<JDComponent::UI_ImageComponent>();
				if (imageComponent) {
					imageComponent->SetTextureName(m_textureName);
					imageComponent->SetColor(m_textureColor);
				}

				////////////////////////////////////////////////////////////////////////////////
				// UI_TextComponent
				AddComponent<JDComponent::UI_TextComponent>();
				auto textComponent = GetComponent<JDComponent::UI_TextComponent>();

				D2DRenderer& renderer = D2DRenderer::Instance();
				if (textComponent) {
					textComponent->SetText(m_text);
					textComponent->SetColor(m_textColor);
					textComponent->SetTextFormatName(m_textFormatName);

					D2DRenderer& renderer = D2DRenderer::Instance();
					const auto& textFormats = renderer.GetTextFormats();
					auto it = textFormats.find(m_textFormatName);
					if (it == textFormats.end()) {
						// 원하는 포맷이 없으면 기본 포맷 사용
						textComponent->SetTextFormatName("MalgunGothic_14");
					}
				}

				////////////////////////////////////////////////////////////////////////////////
				// UI_ButtonComponent
				AddComponent<JDComponent::UI_ButtonComponent>();
				auto buttonComponent = GetComponent<JDComponent::UI_ButtonComponent>();

				buttonComponent->SetOnClick(
					[]()
					{
						std::cout << "버튼이 눌렸습니다." << std::endl;
					}

				);
			}
		}
		void Button::Start()
		{
			if (!m_isStarted) {
				m_isStarted = true;
				// cout << "Button Start" << endl;
			}
		}
		void Button::Update(float deltaTime)
		{
			__super::Update(deltaTime);
		}
		void Button::LateUpdate(float deltaTime)
		{
			// __super::LateUpdate(deltaTime);
		}
		void Button::FixedUpdate(float fixedDeltaTime)
		{
			// __super::FixedUpdate(fixedDeltaTime);
		}
}
}