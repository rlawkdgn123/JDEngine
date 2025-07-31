#pragma once
#include "ColliderBase.h"

namespace JDComponent {
	class CircleCollider : public ColliderBase {
	public:
		CircleCollider(float radius, Vec2 offset = Vec2{ 0,0 })
			: ColliderBase(ColliderType::Circle), m_radius(radius) {
		}
		~CircleCollider() override = default;

		void SetRadius(float radius) { m_radius = radius; }
		float GetRadius() const { return m_radius; }

		bool Intersect(ColliderBase* other) override;
		bool IsMouseOver(Vec2 mousePos) override;

	private:
		float m_radius;
	};
}