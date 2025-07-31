#pragma once
#include "ColliderBase.h"

enum class ColliderPurpose {
	Debug,    // ȭ�鿡�� �׸� �뵵
	Physics   // ���� �浹 �˻� �� ���� ������
};

namespace JDComponent {
	class BoxCollider : public ColliderBase {
	public:
		BoxCollider(Vec2 halfSize, Vec2 offset = Vec2{ 0,0 }, ColliderPurpose purpose = ColliderPurpose::Physics)
			: ColliderBase(ColliderType::Box), m_halfSize(halfSize), m_purpose(purpose){}
		~BoxCollider() override = default;

		void SetHalfSize(Vec2 halfSize) { m_halfSize = halfSize; }
		Vec2 GetHalfSize() const { return m_halfSize; }
		ColliderPurpose Purpose() const { return m_purpose; }

		bool Intersect(ColliderBase* other) override;
		bool IsMouseOver(Vec2 mousePos) override;
		
	private:
		Vec2 m_halfSize;
		ColliderPurpose m_purpose;
	};
}