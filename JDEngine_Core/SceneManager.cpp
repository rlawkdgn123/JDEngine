#include "pch.h"
#include "framework.h"
#include "SceneManager.h"


void SceneManager::Initalize()
{
}

void SceneManager::RegisterScene(unique_ptr<SceneBase> scene)
{
	assert(scene != nullptr);
	assert(m_SceneCount < MAX_SCENES);

	std::string id_str = scene->GetID();
	assert(id != nullptr);

	// 중복 확인
	for (int i = 0; i < m_SceneCount; ++i)
	{
		if (std::strcmp(m_SceneTable[i].id, id) == 0)
		{
			assert(false && "중복된 씬 ID 등록");
			return;
		}
	}

	m_SceneTable[m_SceneCount].id = id;
	m_SceneTable[m_SceneCount].ptr = scene;

	++m_SceneCount;
}

void SceneManager::ChangeScene(const char* id)
{
	assert(id != nullptr);

	for (int i = 0; i < m_SceneCount; ++i)
	{
		if (std::strcmp(m_SceneTable[i].id, id) == 0)
		{
			if (m_CurrentScene)
				m_CurrentScene->OnLeave();

			m_CurrentScene = m_SceneTable[i].ptr;
			m_CurrentScene->OnEnter();
			return;
		}
	}

	assert(false && "지정된 ID의 씬이 존재하지 않음");\

}

void SceneManager::Update(float deltaTime)
{
	if (m_CurrentScene)
		m_CurrentScene->Update(deltaTime); // 현재 씬 Update()로 이동
}

void SceneManager::Render()
{
	if (m_CurrentScene)
		m_CurrentScene->Render();
}