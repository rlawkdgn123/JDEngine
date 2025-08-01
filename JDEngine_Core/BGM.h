#pragma once
#include "AudioBase.h"

namespace JDComponent
{
    class BGM : public AudioSource {
    public:
        BGM(const std::string& key)
            : AudioSource(key) {
        }

        void OnEnable() override {
            AudioManager::Instance().PlayBGM(m_key, &m_coreChannel);
        }

        void Play() override {
            if (m_coreChannel)
                m_coreChannel->setPaused(false);
        }
        void Stop() override {
            if (m_coreChannel)
                m_coreChannel->stop();
        }

    private:
        FMOD::Channel* m_coreChannel = nullptr;
    };
}
