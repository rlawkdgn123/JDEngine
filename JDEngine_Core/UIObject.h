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
			m_id = static_cast<int>(idCount++);
		}

		UIObject(const std::wstring& name) {
			m_name = name;
			AddComponent<RectTransform>();
			m_rectTransform = GetComponent<RectTransform>();
			m_id = static_cast<int>(idCount++);
		}

		virtual void Awake() override { if (m_isAwaked) { return; } m_isAwaked = true; }
		virtual void Start() override { if (m_isStarted || !m_active) { return; } m_isStarted = true; }
		virtual void Update(float deltaTime) override { 
			if (!m_active) { return; }
			else {
				for (auto& comp : m_components) {
					//std::cout << " 업데이트 출력" << std::endl;
					comp->Update(deltaTime);
				}
			}
		}
		virtual void LateUpdate(float deltaTime) override { if (!m_active) { return; } }
		virtual void FixedUpdate(float fixedDeltaTime) override { if (!m_active) { return; } }
		virtual void OnDestroy() override {}

		//virtual void OnCollisionEnter(GameObjectBase* other) override {}
		//virtual void OnCollisionStay(GameObjectBase* other) override {}
		//virtual void OnCollisionExit(GameObjectBase* other) override {}

		//virtual void OnTriggerEnter(GameObjectBase* other) override {}
		//virtual void OnTriggerStay(GameObjectBase* other) override {}
		//virtual void OnTriggerExit(GameObjectBase* other) override {}

		virtual void SendComPonentMessage(const MessageID msg, void* data = nullptr) override {}
		virtual void SendComPonentEvent(const std::string& ev) override {}

		virtual ~UIObject() = default;

		// 부모·자식 관계 함수
		void AddChild(UIObject* child) {
			if (!child || child == this)
				return;

			// 1) UIObject 계층 관계
			if (child->m_parent)
				child->m_parent->RemoveChild(child);
			m_children.push_back(child);
			child->m_parent = this;

			// 2) RectTransform 포인터 유효성 검사
			auto* childRect = child->GetComponent<RectTransform>();
			if (!childRect) {
				std::cerr << "[ERROR] AddChild: child has no RectTransform!\n";
			}
			if (!m_rectTransform) {
				std::cerr << "[ERROR] AddChild: this has no RectTransform!\n";
			}

			if (childRect && m_rectTransform) {
				// 3) 트랜스폼 계층 연결
				childRect->DetachFromParent();  // 안전하게 분리
				childRect->SetParent(m_rectTransform);
			}
		}

		void RemoveChild(UIObject* child) {
			if (!child || child->m_parent != this) return;
			// 트랜스폼 계층 분리
			child->GetComponent<RectTransform>()->DetachFromParent();

			m_children.erase(
				std::remove(m_children.begin(), m_children.end(), child),
				m_children.end()
			);
			child->m_parent = nullptr;
		}

		UIObject* GetParent() const { return m_parent; }

		const std::vector<UIObject*>& GetChildren() const { return m_children; }

		void SetName(const std::wstring& name) { m_name = name; }
		void SetDirection(const Vector2f& dir) { m_direction = dir; }

	private:
		RectTransform* m_rectTransform = nullptr;
		UIObject* m_parent = nullptr;
		std::vector<UIObject*>   m_children;
	};
}