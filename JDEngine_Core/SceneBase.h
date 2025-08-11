#pragma once
#include "pch.h"
#include "framework.h"
#include "GameObjectBase.h"
#include "UIObject.h"
#include "Animation.h"
#include "Texture.h"

//GameObjectBase; // 전방 선언
namespace JDScene {
    using Transform = JDComponent::D2DTM::Transform;
    using ShapeType = JDGlobal::Core::SceneType;
    using GameObject = JDGameObject::GameObject;
    using UIObject = JDGameObject::UIObject;
    using Vec2 = JDGlobal::Math::Vector2F;

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
        
        virtual void FixedUpdate(float fixedDeltaTime) { 
            for (auto& obj : m_gameObjects) { if (obj) obj->FixedUpdate(fixedDeltaTime); }

            // UI는 물리 안쓸 것 같아서 주석처리 해놓았음. 
            // for (auto& uiobj : m_gameUiObjects) { if (uiobj) uiobj->FixedUpdate(fixedDeltaTime); }
        };

        virtual void Update(float deltaTime) { 
            for (auto& obj : m_gameObjects) { if (obj) obj->Update(deltaTime); }
            for (auto& obj : m_uiObjects) { if (obj) obj->Update(deltaTime); }

            CheckCollision();
        };

        virtual void LateUpdate(float deltaTime) { 
            for (auto& obj : m_gameObjects) { if (obj) obj->LateUpdate(deltaTime); } 
            for (auto& obj : m_uiObjects) { if (obj) obj->LateUpdate(deltaTime); }

            ProcessDestroyQueue();
        }; // LateUpdate 실행 후 마지막 실행에 파괴 큐 오브젝트 제거


        virtual void Render(float deltaTime)
        {
            using JDGlobal::Base::RenderLayerInfo;
            using JDGlobal::Base::SortingLayer;

            // 오브젝트 → 정렬 키 추출 (컴포넌트가 여러 개면 더 뒤에 그려질 쪽을 채택)
            auto layerKey = [](const JDScene::GameObject* obj) -> RenderLayerInfo
                {
                    RenderLayerInfo key{ SortingLayer::None, 0 };

                    if (auto ar = obj->GetComponent<JDComponent::AnimationRender>()) {
                        key = ar->GetLayerInfo();
                    }
                    if (auto tr = obj->GetComponent<JDComponent::TextureRenderer>()) {
                        const auto trInfo = tr->GetLayerInfo();
                        if (key < trInfo) key = trInfo; // 더 큰(뒤에 그려질) 키로 교체
                    }
                    return key;
                };

            //정렬
            std::stable_sort(m_gameObjects.begin(), m_gameObjects.end(),
                [&](const std::unique_ptr<JDScene::GameObject>& a,
                    const std::unique_ptr<JDScene::GameObject>& b)
                {
                    return layerKey(a.get()) < layerKey(b.get());
                });
        }
        
        virtual void OnResize(int width, int height) {}

        template<typename T, typename... Args>
        T* CreateGameObject(Args&&... args);
        
        template<typename T, typename... Args>
        T* CreateUIObject(Args&&... args);

        void DestroyObject(GameObjectBase* object) {
            if (object == nullptr) { return; }
            if (std::find(m_destroyQueue.begin(), m_destroyQueue.end(), object) == m_destroyQueue.end()) { // 이터레이터 끝까지 순회해도 m_destroyQueue에 이미 있지 않으면
                m_destroyQueue.push_back(object); // m_destroyQueue에 추가
            }
        }

        // 컨테이너의 ptr과 동일한지 체크
        // 오브젝트 리스트에서이 이미 파생 클래스를 구분해두었기 때문에, 해당 타입의 컨테이너와 ptr이 동일한지만으로 비교해서 처리한다.
        void ProcessDestroyQueue() { // m_destroyQueue에 저장되어있는 오브젝트 제거
            for (auto& obj : m_destroyQueue) {
                if (!obj) continue;

                obj->OnDestroy();
                
                // GameObject 제거
                m_gameObjects.erase(
                    std::remove_if(
                        m_gameObjects.begin(),
                        m_gameObjects.end(),
                        [&](const std::unique_ptr<GameObject>& target) {
                            return target.get() == obj;
                        }),
                    m_gameObjects.end()
                );

                // UIObject 제거
                m_uiObjects.erase(
                    std::remove_if(
                        m_uiObjects.begin(),
                        m_uiObjects.end(),
                        [&](const std::unique_ptr<UIObject>& target) {
                            return target.get() == obj;
                        }),
                    m_uiObjects.end()
                );
            }

            m_destroyQueue.clear();
        }

        SceneType GetType() const { return m_Type; }

        const std::string& GetID() const { return m_ID; }

        //sceneObjects라는 GameObjectBase* 포인터들을 담고 있는 벡터를 수정할 수 없는(const) 참조자(&) 형태로 반환
        //const std::vector<GamebjectBase*>& GetSceneObjects() const { return sceneObjects; };

        void CheckCollision(); // 충돌 체크

        void SetTimeScale(float timeScale) { m_timeScale = timeScale; }
        float GetTimeScale() const { return m_timeScale; }

        void ToggleDrawColider() { m_drawCollider = !m_drawCollider; }

        void DrawColider(); // 콜라이더 그리기. 씬베이스를 상속받은 씬에서 구현하자. 여긴 렌더를 몰라서..

        Vec2 GetMouseWorldPos(); // 마우스 월드 좌표 얻기.

        ////////////////////////////////////////////////////////////////////////////////

        void SetSelectedObject(GameObjectBase* obj) { m_SelectObject = obj; }
        GameObjectBase* GetSelectedObject() const { return m_SelectObject; }

        const std::vector<std::unique_ptr<GameObject>>& GetGameObjects() const {
            return m_gameObjects;
        }

        const std::vector<std::unique_ptr<UIObject>>& GetUIObjects() const {
            return m_uiObjects;
        }

    protected:
        D2D1_COLOR_F m_highlightColor = D2D1::ColorF(D2D1::ColorF::Red);

        bool    m_prevLeftPressed = false;
        int     m_highlightedIndex = -1;

        const SceneType m_Type;
        const std::string m_ID = "None";

        std::vector<std::unique_ptr<GameObject>> m_gameObjects; // 벡터 형태의 ptr. 알아서 메모리 공간이 부족할 때 확보해준다.
        std::vector<std::unique_ptr<UIObject>> m_uiObjects;
        GameObjectBase* m_SelectObject = nullptr;               // 선택한 오브젝트


        std::vector<std::unique_ptr<RenderPresent>> m_presents; // 렌더 요소만 모아둔 최적화 배열. sceneObjects에서 렌더에 필요한 정보만 복사해서 받아온다.
        std::vector<GameObjectBase*> m_destroyQueue; // 안전하게 오브젝트를 제거하기 위한 큐. LateUpdate() 맨 마지막에 해당 큐의 오브젝트를 제거한다.
        std::vector<CollisionPair> m_prevPairs; // 이전 충돌 정보 보관용. 이것을 통해 두 오브젝트가 충돌하고 있었는지 알 수 있음.
        std::vector<CollisionPair> m_currPairs; // 현재 충돌 정보 보관용. 충돌한 오브젝트 쌍의 정보를 저장하고 이전 충돌 정보에 넘겨줌.
        int m_objectCount = 0; // 각 씬의 객체 카운팅

        float m_timeScale = 1.0f; // 배속. 0이면 정지, 1이면 기본, 2면 2배속...

        bool m_drawCollider = true; // 디버그용 콜라이더 그릴지.

    };
}
#include "SceneBase.Inl"