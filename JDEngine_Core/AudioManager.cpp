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

FMOD::Channel* AudioManager::GetCurrentBGMChannel() const {
    return usingA_ ? musicChA_ : musicChB_;
}

static bool IsPlaying(FMOD::Channel* ch) {
    if (!ch) return false;
    bool playing = false;
    return (ch->isPlaying(&playing) == FMOD_OK) && playing;
}

bool AudioManager::PlayBGM(const std::string& key, FMOD::Channel** out)
{
    auto it = audioPaths_.find(key);
    if (it == audioPaths_.end()) return false;

    FMOD::System* core = FMODSystem::Instance().GetCoreSystem();
    if (!core || !musicGroup_) return false;

    // 현재 슬롯(A/B)과 반대 슬롯
    FMOD::Channel*& cur = usingA_ ? musicChA_ : musicChB_;
    FMOD::Channel*& other = usingA_ ? musicChB_ : musicChA_;

    // 1) 같은 곡이 이미 재생 중이면 재시작하지 않음
    if (!currentBGMKey_.empty() && currentBGMKey_ == key && IsPlaying(cur)) {
        if (out) *out = cur; // 조회용 포인터 복사만
        return true;
    }

    // 2) 다른 곡이거나(키 다름) 같은 곡인데 멈춰있으면: 기존 채널들 정리
    if (cur) { cur->stop();   cur = nullptr; }
    if (other) { other->stop(); other = nullptr; }

    // 3) 새 곡을 현재 슬롯에 재생
    //    PlayLooped가 void이므로 호출 후 cur이 채워졌는지로 성공 판정
    FMODSystem::Instance().PlayLooped(it->second, musicGroup_, &cur);
    if (!cur) return false;

    // 4) 기본 설정
    cur->setVolumeRamp(true);
    cur->setVolume(1.0f);
    cur->setPaused(false);

    currentBGMKey_ = key;

    if (out) *out = cur; // 필요 시 조회용으로 돌려줌(소유권은 AudioManager)

    return true;
}

bool AudioManager::PlaySFX(const std::string& key, FMOD::Channel** out) {
    auto it = audioPaths_.find(key);
    if (it == audioPaths_.end()) return false;

    FMODSystem::Instance().PlayOneShot(it->second, sfxGroup_, out);
    return true;
}

void AudioManager::StopBGM()
{
    FMOD::Channel* cur = usingA_ ? musicChA_ : musicChB_;
    if (cur) {
        cur->stop();
        cur = nullptr; // 내부 포인터 정리
    }
    currentBGMKey_.clear();
}

bool AudioManager::ChangeBGM(const std::string& key, float fadeSec)
{
    // 같은 곡이면 중복 호출 방지
    FMOD::Channel* curNow = usingA_ ? musicChA_ : musicChB_;
    if (!currentBGMKey_.empty() && currentBGMKey_ == key && IsPlaying(curNow))
        return true;

    auto it = audioPaths_.find(key);
    if (it == audioPaths_.end() || !musicGroup_) return false;

    FMOD::System* sys = FMODSystem::Instance().GetCoreSystem();
    if (!sys) return false;

    int rate = 48000; FMOD_SPEAKERMODE sm; int num;
    sys->getSoftwareFormat(&rate, &sm, &num);

    // 현재/다음 슬롯
    FMOD::Channel* cur = usingA_ ? musicChA_ : musicChB_;
    FMOD::Channel** nextOut = usingA_ ? &musicChB_ : &musicChA_;
    *nextOut = nullptr;

    // 다음 트랙 준비 — 반드시 group에 붙여서 재생되어야 함 (PlayLooped 내부 확인!)
    FMODSystem::Instance().PlayLooped(it->second, musicGroup_, nextOut);
    FMOD::Channel* next = *nextOut;
    if (!next) return false;

    // 그룹 DSPClock
    unsigned long long gclk = 0, parent = 0;
    musicGroup_->getDSPClock(&gclk, &parent);
    const unsigned long long fadeTicks = (unsigned long long)(fadeSec * (double)rate);

    // 새 채널: 0.05f → 1.0f (가상화 회피 + 부드러운 페이드)
    next->setPaused(true);
    next->setVolumeRamp(true);
    next->removeFadePoints(0, ULLONG_MAX);
    next->setVolume(0.05f);                       // ★ 초기 볼륨 = 첫 포인트 값과 일치
    next->addFadePoint(gclk, 0.05f);  // now
    next->addFadePoint(gclk + fadeTicks, 1.0f);   // +fadeSec
    next->addFadePoint(gclk + fadeTicks + rate / 50, 1.0f);
    next->setPaused(false);                       // 즉시 시작(스케줄에 맞춰 상승)

    // 현재 채널: 현재 볼륨 → 0.0f
    if (cur) {
        cur->setVolumeRamp(true);
        cur->removeFadePoints(0, ULLONG_MAX);

        float curVol = 1.0f;
        cur->getVolume(&curVol);
        cur->addFadePoint(gclk, curVol);
        cur->addFadePoint(gclk + fadeTicks, 0.0f);

        // 페이드 끝나면 자동 정지(조금 여유)
        cur->setDelay(0, gclk + fadeTicks + rate / 50, true);
    }

    // 상태 갱신 ★
    usingA_ = !usingA_;
    currentBGMKey_ = key;

    FMOD::Channel* now = usingA_ ? musicChA_ : musicChB_;
    if (now) { now->setVolume(1.0f); }

    return true;
}

