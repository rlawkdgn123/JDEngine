#include "pch.h"
#include "Component.h"
#include "GameObjectBase.h"
#include "SceneBase.h"

 
	namespace JDScene {

		template<typename T>
		inline void SceneBase::CreateGameObject(T&& gameObject) 
		{
			//static_assert : 컴파일 검사
			//is_base_of_v<A,B> : A가 B의 부모인가?? c++17, 
			static_assert(std::is_base_of_v<GameObjectBase, T>, "T는 BaseClass를 상속해야 합니다."); 
			m_sceneObjects.push_back(std::make_unique<T>(std::move(gameObject))); // unique타입이기 때문에 매개변수 gameObject를 move를 통한 이전
			++m_objectCount;
		}
	}
