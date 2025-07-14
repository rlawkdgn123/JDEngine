#pragma once
#include "pch.h"

namespace Interface {

    /// <summary>
    /// Timer.h
    /// </summary>
    class GameTimer {
    public:
        virtual ~GameTimer() = default;

        // 현재까지 흐른 전체 시간(초 단위).
        virtual float TotalTime() const = 0;

        // 한 프레임이 처리되는 데 걸리는 시간.
        virtual float DeltaTime() const = 0;

        // 위와 동일하지만 밀리초 단위로 반환 (추가된 메서드).
        virtual float DeltaTimeMS() const = 0;

        // 타이머 리셋 (기준 시점 초기화)
        virtual void Reset() = 0;

        // 타이머 시작
        virtual void Start() = 0;

        // 타이머 일시 정지
        virtual void Stop() = 0;

        // 프레임마다 호출되어 DeltaTime을 계산
        virtual void Tick() = 0;
    };
}
