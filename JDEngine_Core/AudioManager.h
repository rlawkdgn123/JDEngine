// AudioManager.h
#pragma once
#include "FMODSystem.h"

class AudioManager {
public:
    static AudioManager& Instance();

    // 1) 초기화
    void Initialize(int maxChannels = 32);

    // 매 프레임
    void Update();

    // 2) 키 ⇔ 경로 등록 (텍스처 매니저의 LoadTexture 와 유사)
    bool LoadAudio(const std::string& key, const std::string& filepath, bool isBGM = true);

    // 3) 키로 재생
    bool PlayBGM(const std::string& key, FMOD::Channel** out = nullptr);
    bool PlaySFX(const std::string& key, FMOD::Channel** out = nullptr);

    /// BGM 볼륨 (0.0 ~ 1.0)
    void SetMusicVolume(float vol);
    float GetMusicVolume() const;

    /// SFX 볼륨
    void SetSFXVolume(float vol);
    float GetSFXVolume() const;

    /// 전체 볼륨 (Master Volume)
    void SetMasterVolume(float vol);
    float GetMasterVolume() const;

    /// 전역 뮤트 토글
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
