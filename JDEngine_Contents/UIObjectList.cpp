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

				AddComponent<Editor_Clickable>();
				AddComponent<UI_ImageComponent>("Test");
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

				AddComponent<Editor_Clickable>();
				AddComponent<UI_TextComponent>();
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
			__super::Update(deltaTime);

			if (m_index < m_fullText.size()) {
				m_timer += deltaTime;
				float interval = 1.0f / m_charsPerSecond;
				while (m_timer >= interval && m_index < m_fullText.size()) {
					m_timer -= interval;
					++m_index;
				}
				// 3) 현재까지 자른 문자열을 TextComponent에 넘기기
				SetText(m_fullText.substr(0, m_index));
			}
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

				AddComponent<Editor_Clickable>();
				AddComponent<UI_ImageComponent>("Test");
				AddComponent<JDComponent::UI_TextComponent>();
				AddComponent<JDComponent::UI_ButtonComponent>();
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
			if (m_index < m_fullText.size()) {
				m_timer += deltaTime;
				float interval = 1.0f / m_charsPerSecond;
				while (m_timer >= interval && m_index < m_fullText.size()) {
					m_timer -= interval;
					++m_index;
				}
				// 3) 현재까지 자른 문자열을 TextComponent에 넘기기
				SetText(m_fullText.substr(0, m_index));
			}
		}
		void Button::LateUpdate(float deltaTime)
		{
			// __super::LateUpdate(deltaTime);
		}
		void Button::FixedUpdate(float fixedDeltaTime)
		{
			// __super::FixedUpdate(fixedDeltaTime);
		}



		////////////////////////////////////////////////////////////////////////////////
		// Slider
		////////////////////////////////////////////////////////////////////////////////

		void Slider::Awake()
		{
			if (!m_isAwaked) {
				m_isAwaked = true;
				m_isHandleSizeManual = false;
				AddComponent<Editor_Clickable>();
				auto rootRect = AddComponent<RectTransform>();
				rootRect->SetSize({ 200.f, 20.f });
				AddComponent<UI_SliderComponent>();
			}
		}

		void Slider::Assemble(JDScene::SceneBase* scene)
		{
			if (m_isAssembled || !scene) return;
			m_isAssembled = true;

			auto rootRect = GetComponent<RectTransform>();
			if (!rootRect) return;

			m_backgroundImage = scene->CreateUIObject<Image>(L"Slider_Background");
			m_backgroundImage->GetComponent<RectTransform>()->SetParent(rootRect);
			m_backgroundImage->GetComponent<RectTransform>()->SetAnchor({ 0.5f, 0.5f });
			m_backgroundImage->GetComponent<RectTransform>()->SetPivot({ 0.5f, 0.5f });
			m_backgroundImage->SetColor(D2D1::ColorF(D2D1::ColorF::White, 0.f));

			m_fillImage = scene->CreateUIObject<Image>(L"Slider_Fill");
			auto fillRect = m_fillImage->GetComponent<RectTransform>();
			fillRect->SetParent(rootRect);
			fillRect->SetAnchor({ 0.0f, 0.5f });
			fillRect->SetPivot({ 0.0f, 0.5f });
			fillRect->SetPosition({ 0.0f, 0.0f });

			m_handleImage = scene->CreateUIObject<Image>(L"Slider_Handle");
			m_handleImage->GetComponent<RectTransform>()->SetParent(rootRect);
			m_handleImage->GetComponent<RectTransform>()->SetAnchor({ 0.0f, 0.5f });
			m_handleImage->GetComponent<RectTransform>()->SetPivot({ 0.5f, 0.5f });

			auto sliderComp = GetComponent<UI_SliderComponent>();
			if (sliderComp) {
				sliderComp->SetFillRect(m_fillImage->GetComponent<RectTransform>());
				sliderComp->SetHandleRect(m_handleImage->GetComponent<RectTransform>());
			}

			SetBackgroundImage("VOLUME_LINE_2");
			SetFillImage("VOLUME_LINE_1");
			SetHandleImage("VOLUME_CAT_1");
			SetSize(rootRect->GetSize());

			SetValue(0.0f);
		}

		void Slider::SetActiveSlider(bool active)
		{
			SetActive(active);
			if (m_backgroundImage) m_backgroundImage->SetActive(active);
			if (m_fillImage) m_fillImage->SetActive(active);
			if (m_handleImage) m_handleImage->SetActive(active);
		}

		void Slider::SetPosition(const Vector2f& pos)
		{
			if (auto comp = GetComponent<RectTransform>()) {
				comp->SetPosition(pos);
			}
		}

		void Slider::SetSize(const Vector2f& size)
		{
			if (auto comp = GetComponent<RectTransform>()) {
				comp->SetSize(size);
			}

			if (m_backgroundImage) {
				m_backgroundImage->SetSize(size);
			}
			if (m_fillImage) {
				// Fill Image는 너비는 UI_SliderComponent가 제어하므로 높이만 부모에 맞춰줍니다.
				auto fillRect = m_fillImage->GetComponent<RectTransform>();
				fillRect->SetSize({ fillRect->GetSize().x, size.y });
			}
			if (m_handleImage && !m_isHandleSizeManual) {
				Vector2f originalHandleSize = m_handleImage->GetOriginalSize(); // 실제 함수 이름으로 변경 필요
				if (originalHandleSize.y > 0) {
					float scale = size.y / originalHandleSize.y;
					m_handleImage->SetSize(originalHandleSize * scale);
				}
			}

			if (auto comp = GetComponent<UI_SliderComponent>()) {
				comp->SetValue(comp->GetValue(), false); // 값 변경 콜백은 보내지 않음
			}
		}

		void Slider::SetRootSize(const Vector2f& size)
		{
			if (auto comp = GetComponent<RectTransform>()) {
				comp->SetSize(size);
			}
		}

		void Slider::SetBackgroundImage(const std::string& textureName)
		{
			if (m_backgroundImage) m_backgroundImage->SetTextureName(textureName);
		}

		void Slider::SetFillImage(const std::string& textureName)
		{
			if (m_fillImage) m_fillImage->SetTextureName(textureName);
		}

		void Slider::SetHandleImage(const std::string& textureName)
		{
			if (m_handleImage) {
				m_handleImage->SetTextureName(textureName);
				if (auto comp = GetComponent<RectTransform>()) {
					SetSize(comp->GetSize());
				}
			}
		}

		void Slider::SetBackgroundImageSize(const Vector2f& size)
		{
			if (m_backgroundImage) m_backgroundImage->SetSize(size);
		}

		void Slider::SetHandleImageSize(const Vector2f& size)
		{
			if (m_handleImage) {
				m_handleImage->SetSize(size);
				m_isHandleSizeManual = true;
			}
		}

		void Slider::SetFillImagePosition(const Vector2f& pos)
		{
			if (m_fillImage) m_fillImage->SetPosition(pos);
		}

		void Slider::EnableHandleAutoSize(bool enable)
		{
			m_isHandleSizeManual = !enable;
			if (enable) {
				if (auto comp = GetComponent<RectTransform>()) {
					SetSize(comp->GetSize());
				}
			}
		}
		
	}
}