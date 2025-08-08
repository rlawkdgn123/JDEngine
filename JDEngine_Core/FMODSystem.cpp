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
    FMOD::ChannelGroup * group,          // �׷� ����
    FMOD::Channel * *outChannel)
{
    if (!coreSys_) {
        std::cerr << "[FMOD ERROR] coreSys_ is null\n";
        return;
    }

    FMOD::Sound* sound = nullptr;
    FMOD::Channel* channel = nullptr;
    FMOD_RESULT    res;

    // 1) ��Ʈ���� + ���� �ڵ� ����
    res = coreSys_->createStream(
        filepath.c_str(),
        FMOD_LOOP_NORMAL | FMOD_2D,
        nullptr,
        &sound
    );
    FMOD_CHECK(res);

    // 2) �׷��� �����ؼ� ���
    //    nullptr ��� group �� �ѱ��, playSound ���ο��� �ڵ����� setChannelGroup ó��
    res = coreSys_->playSound(sound, group, false, &channel);
    FMOD_CHECK(res);

    // 3) ȣ���ڿ��� ä�� ������ ����
    if (outChannel) {
        *outChannel = channel;
    }

    //std::cout << "[FMOD] PlayLooped(\"" << filepath << "\") ����\n";
}


void FMODSystem::PlayOneShot(const std::string& filepath, FMOD::ChannelGroup* group, FMOD::Channel** outChannel)
{
    if (!coreSys_) return;

    // 1) ���� �ε� (���� ����)
    FMOD::Sound* sound = nullptr;
    coreSys_->createSound(
        filepath.c_str(),
        FMOD_DEFAULT | FMOD_2D | FMOD_CREATESTREAM,
        nullptr,
        &sound
    );
    if (!sound) return;

    // 2) ���
    FMOD::Channel* channel = nullptr;
    coreSys_->playSound(sound, group, true, &channel);
    if (channel) {
        channel->setPaused(false);
    }
    if (outChannel) {
        *outChannel = channel;
    }
    // NOTE: sound �����ʹ� ���� �����ϰų�
    //       ���ø����̼� ���� �� �ϰ� �����ϼ���.
}
void FMODSystem::Update() {
    if (studioSys_) studioSys_->update();
    if (coreSys_)   coreSys_->update();
}