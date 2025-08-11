#pragma once
#include "pch.h"
#include "SceneBase.h"
// SceneManager.h`

class SceneManager
{
public:
    using SceneBase = JDScene::SceneBase;

public:
    static SceneManager& Instance()
    {
        static SceneManager instance;
        return instance;
    }

private:

    SceneManager() = default;
    ~SceneManager() = default;

    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

public:
    void BroadcastResize(int w, int h);

    void RegisterScene(std::unique_ptr<SceneBase> scene); // 씬 생성

    void ChangeScene(const std::string& id); // 씬 전환 ->< 바꿀 씬의 ID만 저장하고 바로 리턴.

    void ProcessSceneChange();               // 실제 씬 전환 로직을 담당할 새로운 함수

    void Update(float deltaTime);

    void FixedUpdate(float fixedDeltaTime);

    void LateUpdate(float deltaTime);

    void Render(float dt);

    SceneBase* GetCurrentScene() const { return m_CurrentScene; }

    void SetSceneTimeScale(float timeScale);

    void ToggleDrawColider();

private:
    int m_viewW = 0, m_viewH = 0;

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

    SceneBase* m_CurrentScene = nullptr;    // 활성화된 씬에 대한 RawPtr
    //std::unique_ptr<JDScene::SceneBase> m_NextScene;
    std::string m_NextSceneId = ""; // 다음에 전환할 씬의 ID를 저장할 변수
};



