#pragma once
#include "AudioBase.h"
#include "D2DTransform.h"

namespace JDComponent
{
    class SFX : public AudioSource {
    public:
        SFX(const std::string& key, float volume = 1.0f)
            : AudioSource(key)
            , m_volume(volume)
            , m_channel(nullptr)
            , m_lastPos({ 0, 0 })
            , m_first(true)
        {}

        void OnEnable() override {
            if (auto tm = GetOwner()->GetComponent<Transform>()) {
                m_lastPos = tm->GetPosition();
            }
            m_first = false;
        }

        void Update(float dt)  {
            auto tm = GetOwner()->GetComponent<Transform>();
            if (!tm) return;

            auto curr = tm->GetPosition();
            // 최초 호출 또는 위치가 바뀌었을 때만 재생
            bool moved = (curr.x != m_lastPos.x) || (curr.y != m_lastPos.y);

            // 최초 프레임은 이동 판정 생략
            if (!m_first && moved) {
                Play();
            }
            m_lastPos = curr;
            m_first = false;
        }

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
        Vector2F        m_lastPos;
        bool            m_first;
    };
}
