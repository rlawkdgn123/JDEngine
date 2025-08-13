#pragma once
#include "Component.h"
#include "AudioManager.h"

namespace JDComponent
{
    class AudioSource : public Component {
    public:
        AudioSource(const std::string& key)
            : m_key(key) {
        }
        virtual ~AudioSource() {}
        virtual void SetClipName(const std::string& key) { m_key = key; }
        virtual void Play() = 0;
        virtual void Stop() = 0;

        void Update(float dt) override {}
        void OnEvent(const std::string& ev) override {}
        void OnEnable() override {}
        void OnDisable() override {}
    protected:
        std::string m_key;
    };
}
