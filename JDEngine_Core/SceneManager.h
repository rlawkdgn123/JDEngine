#pragma once
#include "pch.h"
#include "framework.h"
#include "SceneBase.h"

// SceneManager.h

using namespace std;
class SceneManager
{
private:
    SceneManager() : m_SceneCount(0), m_CurrentScene(nullptr) {} // �ܺ� ���� �Ұ�

    //struct SceneEntry
    //{
    //    const char* id;
    //    SceneBase* ptr;
    //};

    //SceneEntry m_SceneTable[MAX_SCENES];
    
    // Scene ���� unordered_map
    // SceneList - enum, SceneBase - class
    // ���� �������� �Ͻ��� ����ȯ�� �̿��Ͽ� int�� ���� �����.
    unordered_map<SceneList, SceneBase*> m_SceneTable; 

    int        m_SceneCount;

    SceneBase* m_CurrentScene; // Ȱ��ȭ�� ���� ���� RawPtr
public:
    static SceneManager& Instance() // �� �Ŵ��� ����ȭ (�̱���)
    {
        static SceneManager instance;
        return instance;
    }

    ~SceneManager() = default; // scene Ptr�̶� delete ���ص� ��. 

    void Initalize();

    void RegisterScene(unique_ptr<SceneBase> scene);

    void ChangeScene(const char* id);

    void Update(float deltaTime);

    void Render();

    const SceneBase* GetCurrentScene() const { return m_CurrentScene; }
};

