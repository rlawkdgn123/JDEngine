#pragma once

class FMODSystem {
public:
    /// �̱��� �ν��Ͻ�
    static FMODSystem& Instance() {
        static FMODSystem inst;
        return inst;
    }

    /// FMOD Studio + Core �ý��� �ʱ�ȭ
    /// \param maxChannels ���� ��� ������ �ִ� ä�� ��
    void Initialize(int maxChannels = 32) {
        // 1) Studio �ý��� ���� & �ʱ�ȭ
        FMOD::Studio::System::create(&studioSys_);
        studioSys_->initialize(
            maxChannels,
            FMOD_STUDIO_INIT_NORMAL,
            FMOD_INIT_NORMAL,
            nullptr
        );

        // 2) Core �ý��� ������ ���
        studioSys_->getCoreSystem(&coreSys_);
    }
    
    /// �� ������ �� ���� ȣ���ؾ� �մϴ�.
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
