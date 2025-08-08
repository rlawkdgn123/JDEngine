// AudioManager.h
#pragma once
#include "FMODSystem.h"

class AudioManager {
public:
    static AudioManager& Instance();

    // 1) �ʱ�ȭ
    void Initialize(int maxChannels = 32);

    // �� ������
    void Update();

    // 2) Ű �� ��� ��� (�ؽ�ó �Ŵ����� LoadTexture �� ����)
    bool LoadAudio(const std::string& key, const std::string& filepath, bool isBGM = true);

    // 3) Ű�� ���
    bool PlayBGM(const std::string& key, FMOD::Channel** out = nullptr);
    bool PlaySFX(const std::string& key, FMOD::Channel** out = nullptr);

    /// BGM ���� (0.0 ~ 1.0)
    void SetMusicVolume(float vol);
    float GetMusicVolume() const;

    /// SFX ����
    void SetSFXVolume(float vol);
    float GetSFXVolume() const;

    /// ��ü ���� (Master Volume)
    void SetMasterVolume(float vol);
    float GetMasterVolume() const;

    /// ���� ��Ʈ ���
    void SetMute(bool mute);
    bool IsMuted() const;

private:
    AudioManager() = default;
    ~AudioManager() = default;

    // member
    std::map<std::string, std::string> audioPaths_;
    std::vector<std::string>           bgmKeys_, sfxKeys_;

    FMOD::ChannelGroup* masterGroup_ = nullptr;
    FMOD::ChannelGroup* musicGroup_ = nullptr;
    FMOD::ChannelGroup* sfxGroup_ = nullptr;

    float musicVolume_ = 1.0f;
    float sfxVolume_ = 1.0f;
    float masterVolume_ = 1.0f;
    bool  isMuted_ = false;
};
