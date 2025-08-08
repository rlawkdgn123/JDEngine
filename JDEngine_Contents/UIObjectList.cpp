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
		// Button
		////////////////////////////////////////////////////////////////////////////////

		void Slider::Awake()
		{
			if (!m_isAwaked) {
				m_isAwaked = true;

				// Awake에서는 자기 자신에게 필요한 최소한의 컴포넌트만 추가합니다.
				AddComponent<Editor_Clickable>();
				auto rootRect = AddComponent<RectTransform>();
				rootRect->SetSize({ 200.f, 20.f }); // 기본 크기
				AddComponent<UI_SliderComponent>();
			}
		}

		void Slider::Assemble(JDScene::SceneBase* scene)
		{
			// 이미 조립되었거나 씬 정보가 없으면 실행하지 않습니다.
			if (m_isAssembled || !scene) return;
			m_isAssembled = true;

			auto rootRect = GetComponent<RectTransform>();
			if (!rootRect) return;

			// --- 씬의 CreateUIObject 함수를 사용하여 자식들을 생성하고 조립합니다 ---

			// 1. 자식으로 배경(Background) 이미지 생성 및 설정
			m_backgroundImage = scene->CreateUIObject<Image>(L"Slider_Background");
			m_backgroundImage->SetTextureName("VOLUME_LINE_2"); // 기본 배경 텍스처
			m_backgroundImage->SetSizeToOriginal(); // 텍스처 원본 크기로 자동 설정
			auto bgRect = m_backgroundImage->GetComponent<RectTransform>();
			bgRect->SetParent(rootRect);

			// 중요: 슬라이더 루트의 크기를 배경 이미지의 크기와 동일하게 맞춥니다.
			rootRect->SetSize(bgRect->GetSize());

			// 2. 자식으로 채우기(Fill) 이미지 생성 및 설정
			m_fillImage = scene->CreateUIObject<Image>(L"Slider_Fill");
			m_fillImage->SetTextureName("VOLUME_LINE_1"); // 기본 채우기 텍스처
			auto fillRect = m_fillImage->GetComponent<RectTransform>();
			fillRect->SetParent(rootRect);
			// Fill 이미지가 왼쪽에서 오른쪽으로 채워지도록 설정
			fillRect->SetAnchor({ 0.0f, 0.5f });
			fillRect->SetPivot({ 0.0f, 0.5f });
			fillRect->SetPosition({ 0.0f, 0.0f });
			// 채우기 영역의 크기도 배경 크기와 동일하게 초기화합니다.
			fillRect->SetSize(rootRect->GetSize());

			// 3. 자식으로 핸들(Handle) 이미지 생성 및 설정
			m_handleImage = scene->CreateUIObject<Image>(L"Slider_Handle");
			m_handleImage->SetTextureName("VOLUME_CAT_1"); // 기본 핸들 텍스처
			m_handleImage->SetSizeToOriginal(); // 핸들도 텍스처 원본 크기로 자동 설정
			auto handleRect = m_handleImage->GetComponent<RectTransform>();
			handleRect->SetParent(rootRect);

			// 핸들이 슬라이더의 왼쪽-중앙을 기준으로 움직이도록 Anchor를 설정하고,
			// 핸들 자체의 중심을 기준으로 위치가 정해지도록 Pivot을 설정합니다.
			handleRect->SetAnchor({ 0.0f, 0.5f });
			handleRect->SetPivot({ 0.5f, 0.5f });

			// 4. 슬라이더 로직에 Fill과 Handle의 RectTransform을 연결
			auto sliderComp = GetComponent<UI_SliderComponent>();
			if (sliderComp)
			{
				sliderComp->SetFillRect(fillRect);
				sliderComp->SetHandleRect(handleRect);
			}
		}

		void Slider::SetBackgroundImage(const std::string& textureName)
		{
			if (m_backgroundImage) {
				m_backgroundImage->SetTextureName(textureName);
			}
		}

		void Slider::SetFillImage(const std::string& textureName)
		{
			if (m_fillImage) {
				m_fillImage->SetTextureName(textureName);
			}
		}

		void Slider::SetHandleImage(const std::string& textureName)
		{
			if (m_handleImage) {
				m_handleImage->SetTextureName(textureName);
			}
		}

		void Slider::Start()
		{
			if (!m_isStarted) {
				m_isStarted = true;
			}
		}

		void Slider::Update(float deltaTime)
		{
			__super::Update(deltaTime);
		}

		void Slider::LateUpdate(float deltaTime)
		{
			// __super::LateUpdate(deltaTime);
		}

		void Slider::FixedUpdate(float fixedDeltaTime)
		{
			// __super::FixedUpdate(fixedDeltaTime);
		}
		
}
}