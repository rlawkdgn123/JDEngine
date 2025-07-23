#pragma once
#include "pch.h"

// SceneManager.h`

using namespace CoreGlobal;
using SceneBase = JDInterface::JDScene::SceneBase;

namespace JDModule {
    class SceneManager : public JDInterface::SceneManager
    {
    public:
        SceneManager() : m_CurrentScene(nullptr) {} // 외부 생성 불가
    private:
        // 복사 생성자, 복사 대입 연산자 삭제 (객체 복사 금지)
        SceneManager(const SceneManager&) = delete;
        SceneManager& operator=(const SceneManager&) = delete;

        // 이동 생성자, 이동 대입 연산자 삭제 (객체 이동 금지)
        SceneManager(SceneManager&&) = delete;
        SceneManager& operator=(SceneManager&&) = delete;

    public:
        //static SceneManager& Instance() // 씬 매니저 단일화 (싱글톤)
        //{
        //    static SceneManager instance;
        //    return instance;
        //}

        ~SceneManager() = default; // m_SceneTable 스마트 포인터라 delete 안해도 됨. 

        void RegisterScene(std::unique_ptr<SceneBase> scene); // 씬 생성

        void ChangeScene(const std::string& id); // 씬 전환

        void Update(float deltaTime);

        void FixedUpdate(float fixedDeltaTime);

        void LateUpdate(float deltaTime);

        void Render();

        const SceneBase* GetCurrentScene() const { return m_CurrentScene; }

    private:

        struct SceneEntry
        {
            std::string id;
            std::unique_ptr<SceneBase> ptr;

        public:
            SceneEntry(std::string id, std::unique_ptr<SceneBase> ptr)
                : id(std::move(id)), ptr(std::move(ptr)) { // string과 unique_ptr 이전
            }
        };

        std::vector<SceneEntry> m_SceneTable;

        SceneBase* m_CurrentScene; // 활성화된 씬에 대한 RawPtr

    };
}

