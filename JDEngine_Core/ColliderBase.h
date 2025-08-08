#pragma once
#include "Component.h"

namespace JDComponent {
	namespace D2DTM { class Transform; }
}


namespace JDComponent {

	enum class ColliderType // 콜라이더 종류. 나중에 더 추가 가능.
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

		virtual void OnCollisionEnter(ColliderBase* other) { if (onCollisionEnter && !other) onCollisionEnter(other); m_isColliding = true; }
		virtual void OnCollisionStay(ColliderBase* other) { if (onCollisionStay && !other)  onCollisionStay(other); }
		virtual void OnCollisionExit(ColliderBase* other) { if (onCollisionExit && !other)  onCollisionExit(other); m_isColliding = false; }

		void    SetIndex(int idx) { m_index = idx; }
		int     GetIndex() const { return m_index; }

		void    SetOpen(bool o) { m_isOpen = o; }
		bool    IsOpen() const { return m_isOpen; }

		void    SetRenderCollider(bool o) { m_renderCollider = o; }
		bool    IsRenderCollider() const { return m_renderCollider; }

		bool GetColliding() const { return m_isColliding; }
		virtual bool IsMouseOver(Vec2 mousePos) = 0;

		void Update(float dt) override {}
		void OnEvent(const std::string& ev) override {}
		void OnEnable() override {}
		void OnDisable() override {}

	protected:
		Vec2  m_offset;
		ColliderType m_type;

		bool m_isOpen = false;
		int m_index = -1;

		bool m_isTrigger = true;
		bool m_isColliding = false;

		Callback onTriggerEnter;
		Callback onTriggerStay;
		Callback onTriggerExit;

		Callback onCollisionEnter;
		Callback onCollisionStay;
		Callback onCollisionExit;

		bool m_renderCollider = false;
	};
}