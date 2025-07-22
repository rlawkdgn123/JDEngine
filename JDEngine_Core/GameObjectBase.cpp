#include "pch.h"
#include "framework.h"
#include "GameObjectBase.h"

namespace JDModule {
	namespace JDGameObject {
		using MessageID = uint32_t;
		using Component = JDInterface::JDComponent::Component;

		void GameObjectBase::Update(float deltaTime) {
			for (auto& com : m_components)
			{
				com->Update(deltaTime);
			}
		}

		void GameObjectBase::SendComPonentMessage(const MessageID msg, void* data)
		{
			for (auto& com : m_components)
			{
				com->HandleMessage(msg, data);
			}
		}

		void GameObjectBase::SendComPonentEvent(const std::string& ev)
		{
			for (auto& com : m_components)
			{
				com->OnEvent(ev);
			}
		}
	}
}