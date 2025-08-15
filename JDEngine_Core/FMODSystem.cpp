#include "pch.h"
#include "framework.h"
#include "FMODSystem.h"
#include <fmod_errors.h>

#define FMOD_CHECK(_result)                                          \
    do {                                                             \
        FMOD_RESULT _r = (_result);                                  \
        if (_r != FMOD_OK) {                                         \
            std::cerr << "[FMOD ERROR] "                             \
                      << FMOD_ErrorString(_r)                        \
                      << " (" << __FILE__ << ":" << __LINE__ << ")\n"; \
        }                                                            \
    } while (0)

void FMODSystem::PlayLooped(
    const std::string & filepath,
    FMOD::ChannelGroup * group,
    FMOD::Channel * *outChannel)
{
    if (!coreSys_) { std::cerr << "[FMOD ERROR] coreSys_ is null\n"; return; }

    FMOD::Sound* sound = nullptr;
    FMOD::Channel* channel = nullptr;

    // 1) 스트리밍 + 루프 + 2D
    FMOD_RESULT res = coreSys_->createStream(
        filepath.c_str(),
        FMOD_LOOP_NORMAL | FMOD_2D,   // (createStream이므로 STREAM 플래그는 이미 포함)
        nullptr,
        &sound
    );
    FMOD_CHECK(res);

    // 2) 반드시 group에 붙여 '일시정지 상태'로 시작 (paused=true)
    res = coreSys_->playSound(sound, group, true /* paused */, &channel);
    FMOD_CHECK(res);

    // (여기서 setVolume/setPaused 하지 마세요. 페이드는 ChangeBGM에서!)
    if (outChannel) *outChannel = channel;
}


void FMODSystem::PlayOneShot(const std::string& filepath, FMOD::ChannelGroup* group, FMOD::Channel** outChannel)
{
    if (!coreSys_) return;

    // 1) 사운드 로드 (루프 없음)
    FMOD::Sound* sound = nullptr;
    coreSys_->createSound(
        filepath.c_str(),
        FMOD_DEFAULT | FMOD_2D | FMOD_CREATESAMPLE,
        nullptr,
        &sound
    );
    if (!sound) return;

    // 2) 재생
    FMOD::Channel* channel = nullptr;
    coreSys_->playSound(sound, group, true, &channel);
    if (channel) {
        channel->setPaused(false);
    }
    if (outChannel) {
        *outChannel = channel;
    }
    //if (sound) sound->release();

    // NOTE: sound 포인터는 이후 해제하거나
    //       어플리케이션 종료 시 일괄 관리하세요.
}
void FMODSystem::Update() {
    if (studioSys_) studioSys_->update();
    if (coreSys_)   coreSys_->update();
}