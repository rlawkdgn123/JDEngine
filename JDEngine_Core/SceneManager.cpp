#include "pch.h"
#include "framework.h"
#include "SceneBase.h"
#include "SceneManager.h"

 
    // 참고 : 인터페이스에서 SceneBase의 Using선언 되어있음
    void SceneManager::RegisterScene(std::unique_ptr<SceneBase> scene)
    {
        assert(scene != nullptr);
        assert(!scene->GetID().empty());

        std::cout << "씬 생성 : " << scene->GetID() << std::endl;

        if (m_SceneTable.size() >= JDGlobal::Core::MAX_SCENES)
        {
            assert(false && "씬 개수 제한 초과. 해결하려면 MAX_SCENES의 값을 변경하세요.");
            return;
        }

        std::string id = scene->GetID();

        for (const auto& sc : m_SceneTable)
        {
            if (sc.id == id) // id가 동일하면
            {
                assert(false && "중복된 씬 ID 등록");
                return;
            }
        }

        m_SceneTable.emplace_back(SceneEntry(std::move(id), std::move(scene)));
    }

    void SceneManager::ChangeScene(const std::string& id) // string& 쓰는 이유 : 주소로 바로 참조한 뒤 전환 (변경 방지)
    {
        assert(!id.empty());

        for (const auto& sc : m_SceneTable)
        {
            if (sc.id == id) // id가 동일하면
            {
                if (m_CurrentScene)
                    m_CurrentScene->OnLeave();  // 현재 씬 나가기

                m_CurrentScene = sc.ptr.get();
                m_CurrentScene->OnEnter();
                return;
            }
        }

        assert(false && "지정된 ID의 씬이 존재하지 않음");
    }

    void SceneManager::Update(float deltaTime)
    {
        if (m_CurrentScene)
        {
            float ts = m_CurrentScene->GetTimeScale();
            m_CurrentScene->Update(deltaTime * ts);
        }
    }

    void SceneManager::FixedUpdate(float fixedDeltaTime)
    {
        if (m_CurrentScene)
        {
            float ts = m_CurrentScene->GetTimeScale();
            m_CurrentScene->FixedUpdate(fixedDeltaTime * ts);
        }
    }

    void SceneManager::LateUpdate(float deltaTime)
    {
        if (m_CurrentScene)
        {
            float ts = m_CurrentScene->GetTimeScale();
            m_CurrentScene->LateUpdate(deltaTime * ts);
        }
    }

    void SceneManager::Render(float dt)
    {
        if (m_CurrentScene)
        {
            float ts = m_CurrentScene->GetTimeScale();
            m_CurrentScene->Render(dt * ts);
        }
            
    }

    void SceneManager::SetSceneTimeScale(float timeScale)
    {
        if (m_CurrentScene)
        {
            m_CurrentScene->SetTimeScale(timeScale);
        }
    }


