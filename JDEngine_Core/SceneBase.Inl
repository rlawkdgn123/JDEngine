#include "SceneBase.h"
/*
항목	    설명
확장자	    .inl (inline file)
용도	    템플릿/인라인 함수 구현을 분리하여 헤더에 포함시키기 위한 용도
포함 위치	보통 .h 파일 하단에서 #include "MyClass.inl" 형태로 사용
특징	    실제로는 C++ 문법상 특별한 의미는 없고, 관습적으로 쓰는 확장자임

🔷 .cpp에 넣지 않고 .inl로 따로 빼는 이유
.cpp에 정의하면 템플릿 인스턴스화 시점에 링커가 구현을 못 찾을 수 있음.

.inl 파일은 일반적으로 헤더와 함께 include되는 전제로, 컴파일 타임에 포함되기 때문에 템플릿 구현에 적합.

왜 .h의 맨 아래에서 ini를 include 하나요?
✅ 클래스 선언과 멤버 함수 선언이 모두 끝난 뒤에 템플릿 정의를 포함해야 하기 때문입니다.
*/

// SceneBase.inl
namespace JDScene {
	template<typename T, typename... Args>
	T* SceneBase::CreateGameObject(Args&&... args)
	{
		//static_assert : 컴파일 검사
		//is_base_of_v<A,B> : A가 B의 부모인가?? c++17, 
		static_assert(std::is_base_of<GameObjectBase, T>::value, "T는 BaseClass를 상속해야 합니다.");
		
		auto obj = std::make_unique<T>(std::forward<Args>(args)...);  // T 객체 생성
		T* ptr = obj.get();
		m_gameObjects.emplace_back(std::move(obj)); // unique타입이기 때문에 매개변수 gameObject를 forward를 통한 이전
		m_objectCount++;

		std::wcout << L"ptr->GetName().empty()? : " << ptr->GetName().empty() << std::endl;

		std::wcout << L"CreateGameObj Name :" << ptr->GetName() << L"\nCreateGameObj id :" << ptr->GetID() << std::endl;
		ptr->Awake();
		ptr->Start();

		return ptr;
	}

	template<typename T, typename... Args>
	T* SceneBase::CreateUIObject(Args&&... args)
	{
		//static_assert : 컴파일 검사
		//is_base_of_v<A,B> : A가 B의 부모인가?? c++17, 
		static_assert(std::is_base_of<GameObjectBase, T>::value, "T는 BaseClass를 상속해야 합니다.");

		auto obj = std::make_unique<T>(std::forward<Args>(args)...);  // T 객체 생성
		T* ptr = obj.get();
		m_gameUiObjects.emplace_back(std::move(obj)); // unique타입이기 때문에 매개변수 gameObject를 forward를 통한 이전
		m_objectCount++;

		std::wcout << L"ptr->GetName().empty()? : " << ptr->GetName().empty() << std::endl;

		std::wcout << L"CreateGameObj Name :" << ptr->GetName() << L"\nCreateGameObj id :" << ptr->GetID() << std::endl;
		ptr->Awake();
		ptr->Start();

		return ptr;
	}

	/*
	    template<typename T, typename... Args>
    T* GameObjectBase::AddComponent(Args&&... args)
    {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

        auto comp = std::make_unique<T>(std::forward<Args>(args)...);
        comp->SetOwner(this);

        T* ptr = comp.get();
        m_components.emplace_back(std::move(comp));
        return ptr;
    }

	*/
}