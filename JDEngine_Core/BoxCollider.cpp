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
			// ��Ȯ�� Box vs Circle
			CircleCollider* c = static_cast<CircleCollider*>(other);
			auto* tA = GetTransform();
			auto* tC = c->GetTransform();
			if (!tA || !tC) return false;

			// ���� �߽� ��ǥ
			Vec2 centerC = tC->GetPosition() + c->GetColliderOffset();
			float radius = c->GetRadius();

			// �簢�� �߽�
			Vec2 centerA = tA->GetPosition() + m_offset;

			// �� �߽ɿ��� �簢�� �߽������� ����
			Vec2 d = centerC - centerA;

			// �簢���� ��ũ��(half-size)
			Vec2 h = m_halfSize;

			// �簢�� ���ο��� �� �߽ɿ� ���� ����� ���� ã��
			Vec2 closest{
				std::max(-h.x, std::min(d.x, h.x)),
				std::max(-h.y, std::min(d.y, h.y))
			};

			// �� ������ �� �߽ɱ����� ����
			Vec2 diff = d - closest;

			// �Ÿ� ���� �� �ݰ� ���� �̸� �浹
			return (diff.x * diff.x + diff.y * diff.y) <= (radius * radius);
			break;
		}
		}

		return false;
	}
}
