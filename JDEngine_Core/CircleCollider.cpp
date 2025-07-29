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
			break;
		}
		case ColliderType::Box:
		{
			return static_cast<BoxCollider*>(other)->Intersect(this);;
			break;
		}
		}

		return false;
	}
}
