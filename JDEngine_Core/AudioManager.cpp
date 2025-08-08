#include "pch.h"
#include "framework.h"
#include "AudioManager.h"

// 싱글톤 인스턴스 반환
AudioManager& AudioManager::Instance() {
    static AudioManager inst;
    return inst;
}

void AudioManager::Initialize(int maxChannels) {
    // 1) FMOD Low-level 시스템 초기화
    FMODSystem::Instance().Initialize(maxChannels);
    FMOD::System* coreSys = FMODSystem::Instance().GetCoreSystem();

    // 2) 마스터 그룹과 서브 그룹 생성/연결
    coreSys->getMasterChannelGroup(&masterGroup_);
    coreSys->createChannelGroup("Music", &musicGroup_);
    coreSys->createChannelGroup("SFX", &sfxGroup_);
    masterGroup_->addGroup(musicGroup_);
    masterGroup_->addGroup(sfxGroup_);
    masterGroup_->setVolume(masterVolume_);
    musicGroup_->setVolume(musicVolume_);
    sfxGroup_->setVolume(sfxVolume_);
}

void AudioManager::Update() {
    FMODSystem::Instance().Update();
}

bool AudioManager::LoadAudio(const std::string& key, const std::string& filepath, bool isBGM) {
    // TODO: 파일 존재 체크 로직 추가 가능
    audioPaths_[key] = filepath;
    if (isBGM)      bgmKeys_.push_back(key);
    else            sfxKeys_.push_back(key);
    return true;
}

void AudioManager::SetMusicVolume(float vol) {
    if (musicGroup_) {
        musicGroup_->setVolume(vol);
        std::cout << "[AudioManager] MusicGroup volume = " << vol << "\n";
    }
    else {
        std::cerr << "[AudioManager ERROR] musicGroup_ is null\n";
    }
    musicVolume_ = vol;
}

float AudioManager::GetMusicVolume() const {
    return musicVolume_;
}

void AudioManager::SetSFXVolume(float vol) {
    if (sfxGroup_) {
        sfxGroup_->setVolume(vol);
        std::cout << "[AudioManager] SFXGroup volume = " << vol << "\n";
    }
    else {
        std::cerr << "[AudioManager ERROR] sfxGroup_ is null\n";
    }
    sfxVolume_ = vol;
}

float AudioManager::GetSFXVolume() const {
    return sfxVolume_;
}

void AudioManager::SetMasterVolume(float vol) {
    
        if (masterGroup_) {
            masterGroup_->setVolume(vol);
            std::cout << "[AudioManager] MasterGroup_ volume = " << vol << "\n";
        }
        else {
            std::cerr << "[AudioManager ERROR] masterGroup_ is null\n";
        }
        masterVolume_ = vol;
}

float AudioManager::GetMasterVolume() const {
    return masterVolume_;
}

void AudioManager::SetMute(bool mute) {
    if (masterGroup_) masterGroup_->setMute(mute);
    isMuted_ = mute;
}

bool AudioManager::IsMuted() const {
    return isMuted_;
}

bool AudioManager::PlayBGM(const std::string& key, FMOD::Channel** out) {
    auto it = audioPaths_.find(key);
    if (it == audioPaths_.end()) return false;

    FMODSystem::Instance().PlayLooped(it->second, musicGroup_, out);
    return true;
}

bool AudioManager::PlaySFX(const std::string& key, FMOD::Channel** out) {
    auto it = audioPaths_.find(key);
    if (it == audioPaths_.end()) return false;

    FMODSystem::Instance().PlayOneShot(it->second, sfxGroup_, out);
    return true;
}
