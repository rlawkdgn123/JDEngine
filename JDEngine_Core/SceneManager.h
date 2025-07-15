#pragma once
#include "pch.h"
#include "framework.h"
#include "SceneBase.h"

// SceneManager.h

using namespace std;
class SceneManager
{
private:
    SceneManager() : m_SceneCount(0), m_CurrentScene(nullptr) {} // 외부 생성 불가

    //struct SceneEntry
    //{
    //    const char* id;
    //    SceneBase* ptr;
    //};

    //SceneEntry m_SceneTable[MAX_SCENES];
    
    // Scene 모음 unordered_map
    // SceneList - enum, SceneBase - class
    // 기존 열거형의 암시적 형변환을 이용하여 int형 맵을 만든다.
    unordered_map<SceneList, SceneBase*> m_SceneTable; 

    int        m_SceneCount;

    SceneBase* m_CurrentScene; // 활성화된 씬에 대한 RawPtr
public:
    static SceneManager& Instance() // 씬 매니저 단일화 (싱글톤)
    {
        static SceneManager instance;
        return instance;
    }

    ~SceneManager() = default; // scene Ptr이라 delete 안해도 됨. 

    void Initalize();

    void RegisterScene(unique_ptr<SceneBase> scene);

    void ChangeScene(const char* id);

    void Update(float deltaTime);

    void Render();

    const SceneBase* GetCurrentScene() const { return m_CurrentScene; }
};

