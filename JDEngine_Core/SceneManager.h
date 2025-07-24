#pragma once
#include "pch.h"
#include "SceneBase.h"
// SceneManager.h`

namespace JDEngine {
    class SceneManager 
    {
    public:
        using SceneBase = JDEngine::JDScene::SceneBase;

        SceneManager() : m_CurrentScene(nullptr) {} // �ܺ� ���� �Ұ�
    private:
        // ���� ������, ���� ���� ������ ���� (��ü ���� ����)
        SceneManager(const SceneManager&) = delete;
        SceneManager& operator=(const SceneManager&) = delete;

        // �̵� ������, �̵� ���� ������ ���� (��ü �̵� ����)
        SceneManager(SceneManager&&) = delete;
        SceneManager& operator=(SceneManager&&) = delete;

    public:
        //static SceneManager& Instance() // �� �Ŵ��� ����ȭ (�̱���)
        //{
        //    static SceneManager instance;
        //    return instance;
        //}

        ~SceneManager() = default; // m_SceneTable ����Ʈ �����Ͷ� delete ���ص� ��. 

        void RegisterScene(std::unique_ptr<SceneBase> scene); // �� ����

        void ChangeScene(const std::string& id); // �� ��ȯ

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
                : id(std::move(id)), ptr(std::move(ptr)) { // string�� unique_ptr ����
            }
        };

        std::vector<SceneEntry> m_SceneTable;

        SceneBase* m_CurrentScene; // Ȱ��ȭ�� ���� ���� RawPtr

        
    };
}

