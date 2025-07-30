#pragma once
#include "pch.h"
#include "framework.h"
#include "GameObjectBase.h"
#include "Component.h"
#include "RectTransform.h"

namespace JDGameObject {
	class UIObject : public JDGameObject::GameObjectBase {

		using RectTransform = JDComponent::D2DTM::RectTransform;

	public:
		UIObject() {
			m_name = L"UIObject";
			AddComponent<RectTransform>();
			m_rectTransform = GetComponent<RectTransform>();
			m_id = idCount++;
		}

		UIObject(const std::wstring& name) {
			m_name = name;
			AddComponent<RectTransform>();
			m_rectTransform = GetComponent<RectTransform>();
			m_id = idCount++;
		}

		virtual void Awake() override { if (m_isAwaked) { return; } m_isAwaked = true; }
		virtual void Start() override { if (m_isStarted || !m_active) { return; } m_isStarted = true; }
		virtual void Update(float deltaTime) override { 
			if (!m_active) { return; }
			else {
				for (auto& comp : m_components) {
					comp->Update(deltaTime);
					std::cout << "업데이트" << std::endl;
				}
			}
		}
		virtual void LateUpdate(float deltaTime) override { if (!m_active) { return; } }
		virtual void FixedUpdate(float fixedDeltaTime) override { if (!m_active) { return; } }
		virtual void OnDestroy() override {}

		virtual void OnCollisionEnter(GameObjectBase* other) override {}
		virtual void OnCollisionStay(GameObjectBase* other) override {}
		virtual void OnCollisionExit(GameObjectBase* other) override {}

		virtual void OnTriggerEnter(GameObjectBase* other) override {}
		virtual void OnTriggerStay(GameObjectBase* other) override {}
		virtual void OnTriggerExit(GameObjectBase* other) override {}

		virtual void SendComPonentMessage(const MessageID msg, void* data = nullptr) override {}
		virtual void SendComPonentEvent(const std::string& ev) override {}

		virtual ~UIObject() = default;

		void SetName(const std::wstring& name) { m_name = name; }
		void SetDirection(const Vector2f& dir) { m_direction = dir; }

	private:
		RectTransform* m_rectTransform = nullptr;

	};
}