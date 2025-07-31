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
    

    void RegisterScene(std::unique_ptr<SceneBase> scene); // �� ����

    void ChangeScene(const std::string& id); // �� ��ȯ

    void Update(float deltaTime);

    void FixedUpdate(float fixedDeltaTime);

    void LateUpdate(float deltaTime);

    void Render(float dt);

    const SceneBase* GetCurrentScene() const { return m_CurrentScene; }

    void SetSceneTimeScale(float timeScale);

    void ToggleDrawColider();

private:

    struct SceneEntry
    {
        std::string id;
        std::unique_ptr<SceneBase> ptr;

    public:
        SceneEntry(std::string id, std::unique_ptr<SceneBase> ptr)
            : id(std::move(id)), ptr(std::move(ptr)) { // string�� unique_ptr ����
        }
    };

    std::vector<SceneEntry> m_SceneTable;

    SceneBase* m_CurrentScene; // Ȱ��ȭ�� ���� ���� RawPtr
};



