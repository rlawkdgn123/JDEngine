#pragma once
#include <string>
#include <vector>
#include <memory>
// 클래스 구현 설계도
/*
클래스 구현 설계도 (수정 아직 안함 이거 아님)
=============================================================
                GameObjectBase  (추상 기본 클래스)
                       ↑
                GeometryObject      (기본 객체 기능 구현)
                       ↑
        +---------------+---------------+
        |                               |
    RectangleObject					CircleObject
    (사각형 객체)					 (원 객체)
=============================================================
*/
class Component; // 추후 추가하기

namespace JDModule{
	//class GameObjectBase : public JDInterface::GameObjectBase {
	class GameObjectBase {
    protected:
        GameObjectBase() { };   // 직접 객체 생성 불가, 파생에서만 가능
        GameObjectBase(const GameObjectBase&) = delete;

    public:
        template<typename T, typename... Args>
        T* AddComponent(Args&&... args)
        {
            static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

            auto comp = std::make_unique<T>(std::forward<Args>(args)...);

            comp->SetOwner(this);

            T* ptr = comp.get();

            m_Components.emplace_back(std::move(comp));

            return ptr;
        }

        template<typename T>
        T* GetComponent() const
        {
            for (auto& com : m_Components)
            {
                if (auto ptr = dynamic_cast<T*>(com.get()))
                {
                    return ptr;
                }
            }

            return nullptr;
        }

        void Update(float deltaTime);

        void SendMessage(const CoreGlobal::MessageID msg, void* data = nullptr);

        void SendEvent(const std::string& ev);

        void SetDirty()
        {
            m_dirty = true;
            for (auto* child : m_children)
            {
                child->SetDirty();
            }
        }

    protected:
        
        Compontnts::D2DTM::Transform* m_tr;
        Vector2f m_dir = { 0.0f, 0.0f }; // 방향 (단위 벡터) : 필요한가? 고민해보자

        bool m_dirty; // 자식이 있는지 없는지 구분
        Transform* m_parent; // 부모 저장 변수 // 오브젝트로 바꾸기
        std::vector<Transform*> m_children; // 자식 저장 변수

        // 입력 청취자 여부 컴포넌트도 추가할지 고민해보자.
        std::vector<std::unique_ptr<Component>> m_Components;

    };

	};



}

