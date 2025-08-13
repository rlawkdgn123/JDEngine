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
    auto it = audioPaths_.find(key);
    if (it == audioPaths_.end()) return false;

    FMOD::System* sys = FMODSystem::Instance().GetCoreSystem();
    if (!sys || !musicGroup_) return false;

    // 출력 샘플레이트 가져오기 (DSP Clock 계산용)
    int rate = 48000; // 기본값
    FMOD_SPEAKERMODE sm;
    int num;
    sys->getSoftwareFormat(&rate, &sm, &num);

    // 현재 사용 중인 채널 / 비사용 채널 선택
    FMOD::Channel* cur = usingA_ ? musicChA_ : musicChB_;
    FMOD::Channel** nextOut = usingA_ ? &musicChB_ : &musicChA_;

    // 1) 다음 트랙을 일단 일시정지 상태로 로드
    FMODSystem::Instance().PlayLooped(it->second, musicGroup_, nextOut);
    FMOD::Channel* next = *nextOut;
    if (!next) return false;

    next->setPaused(true);
    next->setVolume(0.0f);
    next->setVolumeRamp(true);  // 페이드 시 계단현상 방지

    // 2) DSP Clock 기준 페이드 스케줄링
    unsigned long long dspClock = 0, parent = 0;
    musicGroup_->getDSPClock(&dspClock, &parent);

    const unsigned long long fadeTicks = (unsigned long long)(fadeSec * (double)rate);
    const unsigned long long t0 = dspClock + (rate / 1000); // 1ms 후 시작(여유)

    // 현재 채널이 있으면 out 페이드 설정
    if (cur) {
        cur->setVolumeRamp(true);
        cur->addFadePoint(t0, 1.0f);                 // 지금 볼륨을 1.0으로 가정
        cur->addFadePoint(t0 + fadeTicks, 0.0f);     // fadeSec 동안 0까지
        // 페이드가 끝나면 자동 정지는 직접 처리하거나, 약간 뒤에 stop 예약도 가능:
        // cur->setDelay(0, t0 + fadeTicks + rate/50, true); // 20ms 뒤 stop (선택)
    }

    // 다음 채널은 0 → 1.0 으로 상승
    next->addFadePoint(t0, 0.0f);
    next->addFadePoint(t0 + fadeTicks, 1.0f);
    next->setPaused(false); // 스케줄 적용 후 재생 시작

    usingA_ = !usingA_; // 다음부터는 이쪽이 "현재"가 됨

    // (선택) 너무 많은 채널이 쌓이지 않도록 이전 채널을 나중에 정리
    // 안전하게 하려면 Update에서 "페이드 끝난 후 정지"를 검사해도 됩니다.
    return true;
}

