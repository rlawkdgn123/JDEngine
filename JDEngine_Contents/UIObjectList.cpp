#include "pch.h"
#include "framework.h"
#include "UIObjectList.h"

using namespace std;

namespace JDGameObject {
	namespace InGame {
		// Button
		void Button::Awake()
		{
			if (!m_isAwaked) {
				m_isAwaked = true;
				cout << "Button Awake" << endl;
			}

			//// 1. ImageComponent 
			//AddComponent<UI_ImageComponent>;
			//UI_ImageComponent* imageComponent = GetComponent<UI_ImageComponent>();

			//// 이미지 설정 하기.
			//ID2D1Bitmap* testTexture = ResourceManager::Instance().GetTexture("Test");
			//imageComponent->SetImage(testTexture);
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
		}

		void Button::LateUpdate(float deltaTime)
		{
		}

		void Button::FixedUpdate(float fixedDeltaTime)
		{
		}
	}
}