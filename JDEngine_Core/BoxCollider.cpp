#include "pch.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "D2DTransform.h"

namespace JDComponent {
	bool BoxCollider::Intersect(ColliderBase* other)
	{
		if (other == nullptr) { return false; }

		switch (other->GetColliderType())
		{
		case ColliderType::Box:
		{
			BoxCollider* b = static_cast<BoxCollider*>(other);
			Transform* tA = GetTransform();
			Transform* tB = b->GetTransform();
			if (!tA || !tB) { return false; }

			Vec2 posA = tA->GetPosition() + m_offset;
			Vec2 posB = tB->GetPosition() + b->m_offset;

			Vec2 halfA = m_halfSize;
			Vec2 halfB = b->m_halfSize;

			return std::abs(posA.x - posB.x) <= (halfA.x + halfB.x)
				&& std::abs(posA.y - posB.y) <= (halfA.y + halfB.y);
			break;
		}
		case ColliderType::Circle:
		{
			// 정확한 Box vs Circle
			CircleCollider* c = static_cast<CircleCollider*>(other);
			auto* tA = GetTransform();
			auto* tC = c->GetTransform();
			if (!tA || !tC) return false;

			// 원의 중심 좌표
			Vec2 centerC = tC->GetPosition() + c->GetColliderOffset();
			float radius = c->GetRadius();

			// 사각형 중심
			Vec2 centerA = tA->GetPosition() + m_offset;

			// 원 중심에서 사각형 중심으로의 벡터
			Vec2 d = centerC - centerA;

			// 사각형의 반크기(half-size)
			Vec2 h = m_halfSize;

			// 사각형 내부에서 원 중심에 가장 가까운 점을 찾음
			Vec2 closest{
				std::max(-h.x, std::min(d.x, h.x)),
				std::max(-h.y, std::min(d.y, h.y))
			};

			// 그 점에서 원 중심까지의 벡터
			Vec2 diff = d - closest;

			// 거리 제곱 ≤ 반경 제곱 이면 충돌
			return (diff.x * diff.x + diff.y * diff.y) <= (radius * radius);
			break;
		}
		}

		return false;
	}
}
