#pragma once
#include "pch.h"
#include "framework.h"
//GameObjectBase; // 전방 선언

using namespace CoreGlobal;

namespace JDModule {
    namespace Scene {
        class SceneBase : public JDInterface::Scene::SceneBase
        {
        public:
            SceneBase(SceneType type, std::string id)
                : m_Type(type), m_ID(id) {
            }
            virtual ~SceneBase() {}

        private:

            SceneBase() = delete; // 기본 생성자 금지
            SceneBase(const SceneBase&) = delete; // 복사 생성자 금지
            SceneBase& operator=(const SceneBase&) = delete; // 대입 연산자 금지

        public:

            virtual void OnEnter() = 0;

            virtual void OnLeave() = 0;

            virtual void Update(float deltaTime) = 0;

            virtual void Render() = 0; // 렌더는 렌더러에 위임 (지금으로서는 사실상 쓰지 않을 것 같다)

            SceneType GetType() const { return m_Type; }

            const std::string& GetID() const { return m_ID; }

            //sceneObjects라는 GameObjectBase* 포인터들을 담고 있는 벡터를 수정할 수 없는(const) 참조자(&) 형태로 반환
            //const std::vector<GamebjectBase*>& GetSceneObjects() const { return sceneObjects; };

        protected:

            const SceneType m_Type;
            const std::string m_ID = "None";

            std::vector<std::unique_ptr<GameObjectBase>> m_sceneObjects; // 벡터 형태의 ptr. 알아서 메모리 공간이 부족할 때 확보해준다.
            //vector<unique_ptr<RenderPresent>> m_presents; // 렌더 요소만 모아둔 최적화 배열. sceneObjects에서 필요한 정보만 복사해서 받아온다.

            int m_objectCount = 0; // 각 씬의 객체 카운팅
        };
    }
}

