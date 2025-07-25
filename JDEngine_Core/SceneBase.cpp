#include "pch.h"
#include "Component.h"
#include "GameObjectBase.h"
#include "SceneBase.h"

 
	namespace JDScene {

		template<typename T>
		inline void SceneBase::CreateGameObject(T&& gameObject) 
		{
			//static_assert : ������ �˻�
			//is_base_of_v<A,B> : A�� B�� �θ��ΰ�?? c++17, 
			static_assert(std::is_base_of_v<GameObjectBase, T>, "T�� BaseClass�� ����ؾ� �մϴ�."); 
			m_sceneObjects.push_back(std::make_unique<T>(std::move(gameObject))); // uniqueŸ���̱� ������ �Ű����� gameObject�� move�� ���� ����
			++m_objectCount;
		}
	}
