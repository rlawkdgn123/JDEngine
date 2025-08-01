#pragma once
#include "AudioBase.h"

namespace JDComponent
{
    class SFX : public AudioSource {
    public:
        SFX(const std::string& key, float volume = 1.0f)
            : AudioSource(key)
            , m_volume(volume)
            , m_channel(nullptr)
        {}

        void OnEnable() override {}

        void Play() override {
            AudioManager::Instance().PlaySFX(m_key, &m_channel);
            if (m_channel) {
                m_channel->setVolume(m_volume);
            }
        }

        void Stop() override {
            if (m_channel) {
                m_channel->stop();
                m_channel = nullptr;
            }
        }

    private:
        float           m_volume;
        FMOD::Channel* m_channel;
    };
}
