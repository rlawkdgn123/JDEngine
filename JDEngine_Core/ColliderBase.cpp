#include "pch.h"
#include "ColliderBase.h"
#include "GameObjectBase.h" 

namespace JDComponent {
	using Transform = JDComponent::D2DTM::Transform;

	Transform* ColliderBase::GetTransform() const
	{
		if (m_Owner == nullptr) return nullptr;
		return  m_Owner->GetComponent<Transform>();
	}
}