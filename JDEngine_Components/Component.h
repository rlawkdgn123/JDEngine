#pragma once
#include "pch.h"
#include "Framework.h"
namespace JDModule {
    namespace JDComponent {
        class Component : public JDInterface::JDComponent::Component
        {
            using MessageID = uint32_t; // 추후 따로 둘 예정
            using GameObject = JDInterface::JDGameObject::GameObjectBase;
        public:
            virtual ~Component() = default;
            virtual void Update(float dt) = 0;
            virtual void OnEvent(const std::string& ev) abstract;

            void HandleMessage(MessageID msg, void* data)
            {
                auto it = m_MessageHandlers.find(msg);
                if (it != m_MessageHandlers.end())
                {
                    for (auto& handler : it->second)
                        handler(data);
                }
            }

            using HandlerType = std::function<void(void*)>;
            void RegisterMessageHandler(MessageID msg, HandlerType handler)
            {
                m_MessageHandlers[msg].push_back(std::move(handler));
            }

            void SetOwner(GameObject* owner) { m_Owner = owner; }

        protected:
            GameObject* m_Owner = nullptr;

            std::unordered_map<MessageID, std::vector<HandlerType>> m_MessageHandlers;
        };
    }
}
