#include "pch.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "D2DTransform.h"

namespace JDComponent {
	bool CircleCollider::Intersect(ColliderBase* other)
	{
		if (other == nullptr) { return false; }

		switch (other->GetColliderType())
		{
		case ColliderType::Circle:
		{
			CircleCollider* b = static_cast<CircleCollider*>(other);
			Transform* tA = GetTransform();
			Transform* tB = b->GetTransform();
			if (!tA || !tB) { return false; }

			Vec2 posA = tA->GetPosition() + m_offset;
			Vec2 posB = tB->GetPosition() + b->m_offset;

			float radA = m_radius;
			float radB = b->m_radius;

			Vec2 diff = posB - posA;

			return diff.Length() <= (radA + radB);
		}
		case ColliderType::Box:
		{
			return static_cast<BoxCollider*>(other)->Intersect(this);;
		}
		}

		return false;
	}

	bool CircleCollider::IsMouseOver(Vec2 mousePos)
	{
		Transform* tm = GetTransform();
		Vec2 pos = tm->GetPosition();

		Vec2 colPos = { pos.x + m_offset.x, pos.y + m_offset.y };
		Vec2 diff = colPos - mousePos;

		if (diff.Length() <= m_radius) {
			return true;
		}
		return false;
	}
}
