#include "pch.h"
#include "framework.h"

using namespace std;
using RectTransform = JDComponent::D2DTM::RectTransform;

namespace JDGameObject {
	namespace InGame {

		// Button
		void Button::Awake()
		{
			if (!m_isAwaked) {
				m_isAwaked = true;
				cout << "Button Awake" << endl;

				////////////////////////////////////////////////////////////////////////////////
				// RectTransform
				auto rtf = GetComponent<RectTransform>();
				rtf->SetPosition({ 0.f, 0.f });
				rtf->SetScale({ 1.f, 1.f });
				rtf->SetSize({ 100.f, 100.f });

				////////////////////////////////////////////////////////////////////////////////
				// UI_ImageComponent
				AddComponent<UI_ImageComponent>("Test");

				////////////////////////////////////////////////////////////////////////////////
				// UI_TextComponent
				AddComponent<JDComponent::UI_TextComponent>();
				auto textComponent = GetComponent<JDComponent::UI_TextComponent>();

				D2DRenderer& renderer = D2DRenderer::Instance();
				IDWriteTextFormat* textFormat = renderer.GetTextFormat();
				textComponent->SetTextFormat(textFormat);
				textComponent->SetText(L"Test");
				textComponent->SetColor(D2D1::ColorF(D2D1::ColorF::LightSkyBlue));

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
				cout << "Button Start" << endl;
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