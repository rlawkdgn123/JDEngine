#pragma once

class FMODSystem {
public:
    /// 싱글턴 인스턴스
    static FMODSystem& Instance() {
        static FMODSystem inst;
        return inst;
    }

    /// FMOD Studio + Core 시스템 초기화
    /// \param maxChannels 동시 재생 가능한 최대 채널 수
    void Initialize(int maxChannels = 32) {
        // 1) Studio 시스템 생성 & 초기화
        FMOD::Studio::System::create(&studioSys_);
        studioSys_->initialize(
            maxChannels,
            FMOD_STUDIO_INIT_NORMAL,
            FMOD_INIT_NORMAL,
            nullptr
        );

        // 2) Core 시스템 포인터 얻기
        studioSys_->getCoreSystem(&coreSys_);
    }
    
    /// 매 프레임 한 번씩 호출해야 합니다.
    void Update();

    void PlayOneShot(const std::string& filepath, FMOD::ChannelGroup* group, FMOD::Channel** outChannel = nullptr);

    void PlayLooped(const std::string& filepath, FMOD::ChannelGroup* group, FMOD::Channel** outChannel = nullptr);

    FMOD::Studio::System* GetStudioSystem() const {
        return studioSys_;
    }

    FMOD::System* GetCoreSystem() const {
        return coreSys_;
    }
private:
    FMODSystem() = default;
    ~FMODSystem() = default;
    FMODSystem(const FMODSystem&) = delete;
    FMODSystem& operator=(const FMODSystem&) = delete;

    FMOD::Studio::System* studioSys_ = nullptr;
    FMOD::System* coreSys_ = nullptr;
};
