#include "pch.h"
#include "GameTimer.h"
/// <summary>
/// QueryPerformanceFrequency : 1초당 카운트 수(ticks per second)를 반환
/// mSecondsPerCount에 저장 → tick을 초 단위로 변환하는 비율이 됨
/// 
/// QueryPerformanceCounter : 고해상도 타이머 기능 제공
/// </summary>

namespace JDEngine {
	GameTimer::GameTimer()
		: m_secondsPerCount(0.0), m_deltaTIme(-1.0), m_baseTime(0), m_stopTime(0),
		m_pausedTime(0), m_prevTime(0), m_currTime(0), m_stopped(false)
	{
		__int64 countsPerSec;
		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
		m_secondsPerCount = 1.0 / (double)countsPerSec;
	}

	float GameTimer::TotalTime() const
	{
		if (m_stopped) // 멈췄을 경우
		{
			return (float)(((m_stopTime - m_pausedTime) - m_baseTime) * m_secondsPerCount); // 정지한 시간 - 멈춰있는 시간 * 현재시간
		}
	}

	float GameTimer::DeltaTime() const
	{
		return (float)m_deltaTIme;
	}


	float GameTimer::DeltaTimeMS() const
	{
		return (float)(m_deltaTIme * 1000.0);
	}

	void GameTimer::Reset()
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		m_baseTime = currTime;
		m_prevTime = currTime;
		m_stopTime = 0;
		m_stopped = false;
	}

	void GameTimer::Start()
	{
		__int64 startTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

		if (m_stopped)
		{
			m_pausedTime += (startTime - m_stopTime);

			m_prevTime = startTime;
			m_stopTime = 0;
			m_stopped = false;
		}
	}

	void GameTimer::Stop()
	{
		if (!m_stopped)
		{
			__int64 currTime;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

			m_stopTime = currTime;
			m_stopped = true;
		}
	}

	void GameTimer::Tick() // 틱카운트 갱신
	{
		if (m_stopped) // 타이머가 멈춘 상태인 경우
		{
			m_deltaTIme = 0.0; // 델타 타임을 0으로 설정
			return; // 함수 종료
		}

		__int64 currTime; // 현재 시간을 저장할 변수
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime); // 고정 주파수 카운터를 사용하여 현재 시간 획득
		m_currTime = currTime; // 멤버 변수에 현재 시간 저장

		m_deltaTIme = (m_currTime - m_prevTime) * m_secondsPerCount; // 델타 타임 계산

		m_prevTime = m_currTime; // 다음 프레임을 위해 현재 시간을 이전 시간으로 업데이트

		if (m_deltaTIme < 0.0) // 델타 타임이 음수일 경우 (예외 상황)
		{
			m_deltaTIme = 0.0; // 델타 타임을 0으로 설정
		}
	}
}


