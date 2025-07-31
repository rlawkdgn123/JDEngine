#pragma once
#include "Component.h"

namespace JDComponent { 
	namespace D2DTM { class Transform; } 
}


namespace JDComponent {

	enum class ColliderType // �ݶ��̴� ����. ���߿� �� �߰� ����.
	{
		Circle,
		Box
	};

	class ColliderBase : public JDComponent::Component {
	public:
		using Vec2 = JDGlobal::Math::Vector2F;
		using Transform = JDComponent::D2DTM::Transform;
		using Callback = std::function<void(ColliderBase*)>;

		//ColliderBase(ColliderType type) : m_type(type), m_offset({ 0, 0 }) {}  
		ColliderBase(ColliderType type, Vec2 offset = Vec2{ 0,0 }) : m_type(type), m_offset(offset) {}
		virtual ~ColliderBase() {}

		void SetColliderOffset(Vec2 center) { m_offset = center; }
		Vec2 GetColliderOffset() const { return m_offset; }

		void SetColliderType(ColliderType type) { m_type = type; }
		ColliderType GetColliderType() const { return m_type; }

		virtual bool Intersect(ColliderBase* other) = 0;

		Transform* GetTransform() const;

		void SetTrigger(bool trigger) { m_isTrigger = trigger; }
		bool IsTrigger() const { return m_isTrigger; }

		void SetOnTriggerEnter(Callback cb) { onTriggerEnter = std::move(cb); }
		void SetOnTriggerStay(Callback cb) { onTriggerStay = std::move(cb); }
		void SetOnTriggerExit(Callback cb) { onTriggerExit = std::move(cb); }

		void SetOnCollisionEnter(Callback cb) { onTriggerEnter = std::move(cb); }
		void SetOnCollisionStay(Callback cb) { onTriggerStay = std::move(cb); }
		void SetOnCollisionExit(Callback cb) { onTriggerExit = std::move(cb); }

		virtual void OnTriggerEnter(ColliderBase* other) { if (onTriggerEnter) onTriggerEnter(other); m_isColliding = true; }
		virtual void OnTriggerStay(ColliderBase* other) { if (onTriggerStay)  onTriggerStay(other); }
		virtual void OnTriggerExit(ColliderBase* other) { if (onTriggerExit)  onTriggerExit(other); m_isColliding = false; }

		virtual void OnCollisionEnter(ColliderBase* other) { if (onCollisionEnter) onCollisionEnter(other); m_isColliding = true; }
		virtual void OnCollisionStay(ColliderBase* other) { if (onCollisionStay)  onCollisionStay(other); }
		virtual void OnCollisionExit(ColliderBase* other) { if (onCollisionExit)  onCollisionExit(other); m_isColliding = false; }

		bool GetColliding() const { return m_isColliding; }
		virtual bool IsMouseOver(Vec2 mousePos) = 0;

		void Update(float dt) override {}
		void OnEvent(const std::string& ev) override {}
		void OnEnable() override {}
		void OnDisable() override {}

	protected:
		Vec2  m_offset;
		ColliderType m_type;

		bool m_isTrigger = true;
		bool m_isColliding = false;

		Callback onTriggerEnter;
		Callback onTriggerStay;
		Callback onTriggerExit;

		Callback onCollisionEnter;
		Callback onCollisionStay;
		Callback onCollisionExit;

	};
}