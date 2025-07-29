#pragma once
#include "pch.h"
#include "Framework.h"

#include <functional>
#include <unordered_map>
#include <vector>
#include <string>
 
namespace JDGameObject {
    class GameObjectBase;
}


 
namespace JDComponent {
    class Component
    {
    public:
        using GameObjectBase = JDGameObject::GameObjectBase;
        using MessageID = JDGlobal::Core::MessageID;
        using HandlerType = std::function<void(void*)>;
    public:
        virtual ~Component() = default;
        virtual void Update(float dt) = 0;
        virtual void OnEvent(const std::string& ev) = 0;
        virtual void OnEnable() = 0;
        virtual void OnDisable() = 0;
        bool GetEnabled() const { return m_enabled; }
        void SetEnabled(bool value) { m_enabled = value; }
        void HandleMessage(MessageID msg, void* data)
        {
            auto it = m_MessageHandlers.find(msg);
            if (it != m_MessageHandlers.end())
            {
                for (auto& handler : it->second)
                    handler(data);
            }
        }
           
        void RegisterMessageHandler(MessageID msg, HandlerType handler)
        {
            m_MessageHandlers[msg].push_back(std::move(handler));
        }

        void SetOwner(GameObjectBase* owner) { m_Owner = owner; }
        GameObjectBase* GetOwner() { return m_Owner; }

    public:
        bool m_enabled = true;
    protected:
        GameObjectBase* m_Owner = nullptr;

        std::unordered_map<MessageID, std::vector<HandlerType>> m_MessageHandlers;
    };
}

