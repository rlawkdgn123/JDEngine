#pragma once
#include "ColliderBase.h"

namespace JDComponent {
	class BoxCollider : public ColliderBase {
	public:
		BoxCollider(Vec2 halfSize, Vec2 offset = Vec2{ 0,0 })
			: ColliderBase(ColliderType::Box), m_halfSize(halfSize) {}
		~BoxCollider() override = default;

		void SetHalfSize(Vec2 halfSize) { m_halfSize = halfSize; }
		Vec2 GetHalfSize() const { return m_halfSize; }
		Vec2 GetQuarterSize() const { return { m_halfSize.x / 2.0f, m_halfSize.y / 2.0f }; }
		Vec2 GetSize() const { return { m_halfSize.x * 2.0f, m_halfSize.y * 2.0f }; }

		bool Intersect(ColliderBase* other) override;
		bool IsMouseOver(Vec2 mousePos) override;
		
	private:
		Vec2 m_halfSize;
	};
}