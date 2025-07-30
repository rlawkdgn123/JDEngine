#pragma once
#include "pch.h"
#include "framework.h"
#include "GameObjectBase.h"
#include <memory>

//GameObjectBase; // 전방 선언

namespace JDScene {
    using Transform = JDComponent::D2DTM::Transform;
    using ShapeType = JDGlobal::Core::SceneType;

    struct RenderPresent {
        RenderPresent(const Transform& tr, const ShapeType& shape) : tr(tr), shape(shape) {}

    private:
        RenderPresent() = delete;
        const Transform& tr;
        const ShapeType shape;
    public:
        Transform TransformGet() const { return tr; }
        ShapeType ShapeTypeGet() const { return shape; }
    };

    struct CollisionPair { // 충돌 정보 저장용 구조체. 나중에 충돌점이나 다른 정보들도 함께 넣을 수 있음.
        JDGameObject::GameObjectBase* A = nullptr;
        JDGameObject::GameObjectBase* B = nullptr;
    };

    class SceneBase
    {
    public:
        using SceneType = JDGlobal::Core::SceneType;
        using GameObjectBase =  JDGameObject::GameObjectBase;

        SceneBase(SceneType type, std::string id) : m_Type(type), m_ID(id) {}
        virtual ~SceneBase() {}

    private:

        SceneBase() = delete; // 기본 생성자 금지
        SceneBase(const SceneBase&) = delete; // 복사 생성자 금지
        SceneBase& operator=(const SceneBase&) = delete; // 대입 연산자 금지

    public:

        virtual void OnEnter() {};

        virtual void OnLeave() {};
        
        virtual void FixedUpdate(float fixedDeltaTime) { for (auto& obj : m_sceneObjects) { if (obj) obj->FixedUpdate(fixedDeltaTime); } };

        virtual void Update(float deltaTime) { for (auto& obj : m_sceneObjects) { if (obj) obj->Update(deltaTime); }  CheakCollision();};
        
        virtual void LateUpdate(float deltaTime) { for (auto& obj : m_sceneObjects) { if (obj) obj->LateUpdate(deltaTime); } }; // LateUpdate 실행 후 마지막 실행에 파괴 큐 오브젝트 제거
        
        virtual void Render() {};
        
        template<typename T, typename... Args>
        T* CreateGameObject(Args&&... args);
        
        //template <typename T>
        //void CreateUIObject(T uiObject);

        void DestroyObject(GameObjectBase* object) {
            if (object == nullptr) { return; }
            if (std::find(m_destroyQueue.begin(), m_destroyQueue.end(), object) == m_destroyQueue.end()) { // 이터레이터 끝까지 순회해도 m_destroyQueue에 이미 있지 않으면
                m_destroyQueue.push_back(object); // m_destroyQueue에 추가
            }
        }

        void ProcessDestroyQueue() { // m_destroyQueue에 저장되어있는 오브젝트 제거
            for (auto& obj : m_destroyQueue) {
                //obj가 m_sceneObjects에 존재하고, obj가 m_destroyQueue에 존재한다면
                if (obj) { 
                    obj->OnDestroy(); // 파괴 시 OnDestroy()함수 호출
                    /*
                       std::remove_if : 조건에 맞는 요소를 뒤로 몰고, 새로운 끝을 리턴
                       std::vector::erase(start, end) : 지정된 구간의 요소를 벡터에서 제거
                       즉, 해당 함수는 remove_if로 지울 요소를 뒤로 민 동시에,
                       밀린 삭제될 요소 시작 인덱스부터 맨 끝 인덱스까지 제거
                    */
                    m_sceneObjects.erase(
                        std::remove_if(
                            m_sceneObjects.begin(), 
                            m_sceneObjects.end(),
                            [&](const std::unique_ptr<GameObjectBase>& target) {
                                return target.get() == obj; 
                            }),
                        m_sceneObjects.end()
                    );
                }
            }
            m_destroyQueue.clear(); // 오브젝트 파괴 큐는 한 프레임에 한 번 비워야 함
        }

        SceneType GetType() const { return m_Type; }

        const std::string& GetID() const { return m_ID; }

        //sceneObjects라는 GameObjectBase* 포인터들을 담고 있는 벡터를 수정할 수 없는(const) 참조자(&) 형태로 반환
        //const std::vector<GamebjectBase*>& GetSceneObjects() const { return sceneObjects; };

        void CheakCollision(); // 충돌 체크

    protected:

        const SceneType m_Type;
        const std::string m_ID = "None";
        std::vector<std::unique_ptr<GameObjectBase>> m_sceneObjects; // 벡터 형태의 ptr. 알아서 메모리 공간이 부족할 때 확보해준다.
        std::vector<std::unique_ptr<RenderPresent>> m_presents; // 렌더 요소만 모아둔 최적화 배열. sceneObjects에서 렌더에 필요한 정보만 복사해서 받아온다.
        std::vector<GameObjectBase*> m_destroyQueue; // 안전하게 오브젝트를 제거하기 위한 큐. LateUpdate() 맨 마지막에 해당 큐의 오브젝트를 제거한다.
        std::vector<CollisionPair> m_prevPairs; // 이전 충돌 정보 보관용. 이것을 통해 두 오브젝트가 충돌하고 있었는지 알 수 있음.
        std::vector<CollisionPair> m_currPairs; // 현재 충돌 정보 보관용. 충돌한 오브젝트 쌍의 정보를 저장하고 이전 충돌 정보에 넘겨줌.
        int m_objectCount = 0; // 각 씬의 객체 카운팅
    };
}
#include "SceneBase.Inl"