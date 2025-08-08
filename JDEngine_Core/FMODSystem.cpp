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
    FMOD::ChannelGroup * group,          // 그룹 인자
    FMOD::Channel * *outChannel)
{
    if (!coreSys_) {
        std::cerr << "[FMOD ERROR] coreSys_ is null\n";
        return;
    }

    FMOD::Sound* sound = nullptr;
    FMOD::Channel* channel = nullptr;
    FMOD_RESULT    res;

    // 1) 스트리밍 + 루프 자동 지정
    res = coreSys_->createStream(
        filepath.c_str(),
        FMOD_LOOP_NORMAL | FMOD_2D,
        nullptr,
        &sound
    );
    FMOD_CHECK(res);

    // 2) 그룹을 지정해서 재생
    //    nullptr 대신 group 을 넘기면, playSound 내부에서 자동으로 setChannelGroup 처리
    res = coreSys_->playSound(sound, group, false, &channel);
    FMOD_CHECK(res);

    // 3) 호출자에게 채널 포인터 전달
    if (outChannel) {
        *outChannel = channel;
    }

    //std::cout << "[FMOD] PlayLooped(\"" << filepath << "\") 성공\n";
}


void FMODSystem::PlayOneShot(const std::string& filepath, FMOD::ChannelGroup* group, FMOD::Channel** outChannel)
{
    if (!coreSys_) return;

    // 1) 사운드 로드 (루프 없음)
    FMOD::Sound* sound = nullptr;
    coreSys_->createSound(
        filepath.c_str(),
        FMOD_DEFAULT | FMOD_2D | FMOD_CREATESTREAM,
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
    // NOTE: sound 포인터는 이후 해제하거나
    //       어플리케이션 종료 시 일괄 관리하세요.
}
void FMODSystem::Update() {
    if (studioSys_) studioSys_->update();
    if (coreSys_)   coreSys_->update();
}