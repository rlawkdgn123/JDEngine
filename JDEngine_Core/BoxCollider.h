#pragma once
#include "ColliderBase.h"

enum class ColliderPurpose {
	Debug,    // 화면에만 그릴 용도
	Physics   // 실제 충돌 검사 등 게임 로직용
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