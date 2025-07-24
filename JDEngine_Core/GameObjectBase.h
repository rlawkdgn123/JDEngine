#pragma once
#include "pch.h"
#include "Component.h"
#include "D2DTransform.h"
#include "ComponentList.h"
#include "Global.h"
#include "framework.h"
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


  
namespace JDGameObject {
    class GameObjectBase {
    public:
        using Vector2f = JDGlobal::Math::Vector2F;
        using MessageID = JDGlobal::Core::MessageID;
        using Component = JDComponent::Component;
        using Transform = JDComponent::D2DTM::Transform;
    public:
        virtual ~GameObjectBase() {}

    protected:
        GameObjectBase() = default;
        GameObjectBase(const GameObjectBase&) = delete;

    public:
        template<typename T, typename... Args>
        T* AddComponent(Args&&... args);

        template<typename T>
        T* GetComponent() const; // 같은 컴포넌트가 여러 개면 어떻게 뒷순번 컴포넌트를 반환시킬까??

        virtual void Start();                   // 최초 1회만 호출
        virtual void Update(float deltaTime);   // Update
        virtual void LateUpdate(float deltaTime); // Update 후 호출
        virtual void FixedUpdate(float fixedDeltaTime); // 물리 계산용
        virtual void OnDestroy();              // 삭제 직전
        virtual void OnEnable();               // 활성화 시
        virtual void OnDisable();              // 비활성화 시

        virtual void OnCollisionEnter(GameObjectBase* other);
        virtual void OnCollisionStay(GameObjectBase* other);
        virtual void OnCollisionExit(GameObjectBase* other);

        virtual void OnTriggerEnter(GameObjectBase* other);
        virtual void OnTriggerStay(GameObjectBase* other);
        virtual void OnTriggerExit(GameObjectBase* other);

        virtual void SendComPonentMessage(const MessageID msg, void* data = nullptr);
        virtual void SendComPonentEvent(const std::string& ev);

        std::wstring GetName() const { return m_name; }

        bool IsActive() const { return m_active; }
        void SetActive(bool active) { m_active = active; }

        void SetTag(const std::wstring& tag) { m_tag = tag; }
        const std::wstring& GetTag() const { return m_tag; }

        void SetLayer(int layer) { m_layer = layer; }
        int GetLayer() const { return m_layer; }

    protected:

        std::vector<std::unique_ptr<Component>> m_components; // 컴포넌트 리스트
        Vector2f m_direction = { 0.0f, 0.0f }; // 방향 (단위 벡터) : 필요한가? 고민해보자
        std::wstring m_name;
        std::wstring m_tag = L"";
        int m_layer = 0;
        bool m_active = true;

    };
}


namespace JDGameObject {
    class GameObject : public  JDGameObject::GameObjectBase {
    public:
        GameObject() { AddComponent<Transform>(); };
        GameObject(Vector2f pos) { AddComponent<Transform>(pos); }

        virtual ~GameObject() = default;

        // 파생에서 필요한 특화 기능 정의
        void SetName(const std::wstring& name) { m_name = name; }
        void SetDirection(const Vector2f& dir) { m_direction = dir; }

        //tuple RenderPresentGet() override;
        // 혹은 추가 Update 오버라이드 가능
    protected:
    };
}
#include "GameObjectBase.inl"