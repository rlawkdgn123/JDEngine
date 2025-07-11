#pragma once  

#ifndef GAMETIMER_H  
#define GAMETIMER_H  

class GameTimer  
{  
public:  
	GameTimer();  

	float TotalTime()const;		// 현재까지 흐른 전체 시간(초 단위).
	float DeltaTime()const;		// 한 프레임이 처리되는 데 걸리는 시간.

	float DeltaTimeMS()const;	// 위와 동일하지만 밀리초 단위로 반환 (추가된 메서드).

	void Reset();	// 타이머 리셋 (기준 시점 초기화)
	void Start();	// 타이머 시작
	void Stop();	// 타이머 일시 정지
	void Tick();	// 프레임마다 호출되어 DeltaTime을 계산
private:  
	double m_secondsPerCount;   // CPU 타이머 주기를 초 단위로 환산한 값
	double m_deltaTIme;			// 두 프레임 간 시간 차이 (초)

	__int64 m_baseTime;			// 타이머 시작 기준 시점
	__int64 m_pausedTime;		// 일시 정지 동안 누적된 시간 (총 시간에서 제외하기 위해 필요)
	__int64 m_stopTime;			// 타이머가 멈춘 시점
	__int64 m_prevTime;			// 이전 프레임의 시간 (Tick() 호출 기준)
	__int64 m_currTime;			// 현재 프레임의 시간

	bool m_stopped = false;		// 일시 정지 상태 플래그.
};  

#endif // GAMETIMER_H
