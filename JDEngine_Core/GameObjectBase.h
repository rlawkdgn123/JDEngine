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
        static ULONG64 idCount; // id 카운팅 방법 : static++임. 만약 ULONG64 Max만큼 오브젝트 생성 시 터질 수 있음.

        using Vector2f = JDGlobal::Math::Vector2F;
        using Tag = JDGlobal::Base::GameTag;
        using MessageID = JDGlobal::Core::MessageID;
        using Component = JDComponent::Component;
        using Transform = JDComponent::D2DTM::Transform;
        
    public:
        GameObjectBase(const std::wstring& name) : m_name(name) {}
        virtual ~GameObjectBase() {}

    protected:
        GameObjectBase() = default;
        GameObjectBase(const GameObjectBase&) = delete;

    public:
        template<typename T, typename... Args>
        T* AddComponent(Args&&... args);

        template<typename T>
        T* GetComponent() const; // 같은 컴포넌트가 여러 개면 어떻게 뒷순번 컴포넌트를 반환시킬까??

        virtual void Awake() = 0;                    // 오브젝트가 처음 씬에 등장 시, Active 상관없이 무조건 실행
        virtual void Start() = 0;                    // 활성화가 된 첫 프레임에서
        virtual void Update(float deltaTime) = 0;    // 매 프레임 Update
        virtual void LateUpdate(float deltaTime) = 0; // Update 후 호출
        virtual void FixedUpdate(float fixedDeltaTime) = 0; // 물리 계산용
        virtual void OnDestroy() = 0;               // 삭제 직전

        virtual void OnCollisionEnter(GameObjectBase* other) = 0;
        virtual void OnCollisionStay(GameObjectBase* other) = 0;
        virtual void OnCollisionExit(GameObjectBase* other) = 0;

        virtual void OnTriggerEnter(GameObjectBase* other) = 0;
        virtual void OnTriggerStay(GameObjectBase* other) = 0;
        virtual void OnTriggerExit(GameObjectBase* other) = 0;

        virtual void SendComPonentMessage(const MessageID msg, void* data = nullptr) = 0;
        virtual void SendComPonentEvent(const std::string& ev) = 0;

        std::wstring GetID() const { return std::to_wstring(m_id); }
        std::wstring GetName() const { return m_name; }

        int GetLayer() const { return m_layer; }
    bool IsActive() const { return m_active; }

    void SetTag(const std::wstring& tag) { m_tag = tag; }
    void SetLayer(int layer) { m_layer = layer; }
    void SetActive(bool active) { 
        if (m_active == active) return; // 변경이 없으면 무시
        m_active = active;
        for (auto& comp : m_components) {
            if (active && comp->GetEnabled()) { comp->OnEnable(); }
            else if (!active && comp->GetEnabled()) { comp->OnDisable(); }
        }
    }

    protected:
        std::vector<std::unique_ptr<Component>> m_components; // 컴포넌트 리스트
        Vector2f m_direction = { 0.0f, 0.0f }; // 방향 (단위 벡터) : 필요한가? 고민해보자
        std::wstring m_name;
        Tag m_tag = Tag::None;
        int m_layer = 0;

        int m_id = -1;
        bool m_active = true; // 게임 오브젝트 전체의 활성/비활성
        bool m_isAwaked = false;
        bool m_isStarted = false;
    };
}

namespace JDGameObject {
    class GameObject : public JDGameObject::GameObjectBase {
    public:
        GameObject() {
            m_name = L"GameObject";
            AddComponent<Transform>();
            m_transform = GetComponent<Transform>();
            m_id = idCount++;
        }

        GameObject(const std::wstring& name) {
            m_name = name;
            AddComponent<Transform>();
            m_transform = GetComponent<Transform>();
            m_id = idCount++;
        }

        virtual void Awake() override { if (m_isAwaked) { return; } m_isAwaked = true; }
        virtual void Start() override { if (m_isStarted || !m_active) { return; } m_isStarted = true;}
        virtual void Update(float deltaTime) override { if (!m_active) { return; } }
        virtual void LateUpdate(float deltaTime) override { if (!m_active) { return; } }
        virtual void FixedUpdate(float fixedDeltaTime) override { if (!m_active) { return; } }
        virtual void OnDestroy() override {}

        virtual void OnCollisionEnter(GameObjectBase* other) override {}
        virtual void OnCollisionStay(GameObjectBase* other) override {}
        virtual void OnCollisionExit(GameObjectBase* other) override {}

        virtual void OnTriggerEnter(GameObjectBase* other) override {}
        virtual void OnTriggerStay(GameObjectBase* other) override {}
        virtual void OnTriggerExit(GameObjectBase* other) override {}

        virtual void SendComPonentMessage(const MessageID msg, void* data = nullptr) override {}
        virtual void SendComPonentEvent(const std::string& ev) override {}

        virtual ~GameObject() = default;

        void SetName(const std::wstring& name) { m_name = name; }
        void SetDirection(const Vector2f& dir) { m_direction = dir; }

    private:
        Transform* m_transform = nullptr; // 트랜스폼 RawPointer ( 사용 주의 )
    };
}
#include "GameObjectBase.inl"