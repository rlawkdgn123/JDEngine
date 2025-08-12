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
            , m_loop(false)
            , m_playOnMove(true)
        {}

        // --- ���� API ---
        void SetLoop(bool b) {
            m_loop = b;
            if (m_channel) {
                m_channel->setMode(b ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
                if (b) m_channel->setLoopCount(-1); // ���ѹݺ�
            }
        }

        void SetPlayOnMove(bool b) { m_playOnMove = b; } // ȶ���� false ����
        void SetVolume(float v) { m_volume = v; if (m_channel) m_channel->setVolume(m_volume); }

        // Ű ��ü�� �ʿ��ϸ� �״�� �θ� ���� ���
        void SetClipName(const std::string& key) override {
            if (m_key == key) return;
            // ��å��, ��ü �� ��� ���̸� ���߰� �ٽ� ����ϰ� �ʹٸ� �Ʒ� �ּ� ����
            // bool wasPlaying = IsPlaying();
            Stop();
            m_key = key;
            // if (wasPlaying) Play();
        }

        void OnEnable() override {
            if (auto tm = GetOwner()->GetComponent<Transform>()) {
                m_lastPos = tm->GetPosition();
            }
            m_first = false;
        }

        void Update(float dt)  {

            if (!m_playOnMove) return;

            auto tm = GetOwner()->GetComponent<Transform>();
            if (!tm) return;

            auto curr = tm->GetPosition();
            bool moved = (curr.x != m_lastPos.x) || (curr.y != m_lastPos.y);

            if (!m_first && moved) {
                Play();
            }
            m_lastPos = curr;
            m_first = false;
        }

        void Play() override {
            AudioManager::Instance().PlaySFX(m_key, &m_channel);
            if (m_channel) {
                m_channel->setMode(m_loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
                if (m_loop) m_channel->setLoopCount(-1);
                m_channel->setVolume(m_volume);
            }
        }

        void Stop() override {
            if (m_channel) {
                m_channel->stop();
                m_channel = nullptr;
            }
        }

        bool IsPlaying() const {
            if (!m_channel) return false;
            bool playing = false;
            m_channel->isPlaying(&playing);
            return playing;
        }

    private:
        float           m_volume;
        FMOD::Channel* m_channel;
        Vector2F        m_lastPos;
        bool            m_first;

        bool            m_loop;      
        bool            m_playOnMove;
    };
}
