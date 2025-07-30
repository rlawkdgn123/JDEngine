#include "pch.h"
#include "framework.h"
#include "SceneBase.h"
#include "SceneManager.h"

 
    // ���� : �������̽����� SceneBase�� Using���� �Ǿ�����
    void SceneManager::RegisterScene(std::unique_ptr<SceneBase> scene)
    {
        assert(scene != nullptr);
        assert(!scene->GetID().empty());

        std::cout << "�� ���� : " << scene->GetID() << std::endl;

        if (m_SceneTable.size() >= JDGlobal::Core::MAX_SCENES)
        {
            assert(false && "�� ���� ���� �ʰ�. �ذ��Ϸ��� MAX_SCENES�� ���� �����ϼ���.");
            return;
        }

        std::string id = scene->GetID();

        for (const auto& sc : m_SceneTable)
        {
            if (sc.id == id) // id�� �����ϸ�
            {
                assert(false && "�ߺ��� �� ID ���");
                return;
            }
        }

        m_SceneTable.emplace_back(SceneEntry(std::move(id), std::move(scene)));
    }

    void SceneManager::ChangeScene(const std::string& id) // string& ���� ���� : �ּҷ� �ٷ� ������ �� ��ȯ (���� ����)
    {
        assert(!id.empty());

        for (const auto& sc : m_SceneTable)
        {
            if (sc.id == id) // id�� �����ϸ�
            {
                if (m_CurrentScene)
                    m_CurrentScene->OnLeave();  // ���� �� ������

                m_CurrentScene = sc.ptr.get();
                m_CurrentScene->OnEnter();
                return;
            }
        }

        assert(false && "������ ID�� ���� �������� ����");
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


