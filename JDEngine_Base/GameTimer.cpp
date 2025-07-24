#include "pch.h"
#include "GameTimer.h"
/// <summary>
/// QueryPerformanceFrequency : 1�ʴ� ī��Ʈ ��(ticks per second)�� ��ȯ
/// mSecondsPerCount�� ���� �� tick�� �� ������ ��ȯ�ϴ� ������ ��
/// 
/// QueryPerformanceCounter : ���ػ� Ÿ�̸� ��� ����
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
		if (m_stopped) // ������ ���
		{
			return (float)(((m_stopTime - m_pausedTime) - m_baseTime) * m_secondsPerCount); // ������ �ð� - �����ִ� �ð� * ����ð�
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

	void GameTimer::Tick() // ƽī��Ʈ ����
	{
		if (m_stopped) // Ÿ�̸Ӱ� ���� ������ ���
		{
			m_deltaTIme = 0.0; // ��Ÿ Ÿ���� 0���� ����
			return; // �Լ� ����
		}

		__int64 currTime; // ���� �ð��� ������ ����
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime); // ���� ���ļ� ī���͸� ����Ͽ� ���� �ð� ȹ��
		m_currTime = currTime; // ��� ������ ���� �ð� ����

		m_deltaTIme = (m_currTime - m_prevTime) * m_secondsPerCount; // ��Ÿ Ÿ�� ���

		m_prevTime = m_currTime; // ���� �������� ���� ���� �ð��� ���� �ð����� ������Ʈ

		if (m_deltaTIme < 0.0) // ��Ÿ Ÿ���� ������ ��� (���� ��Ȳ)
		{
			m_deltaTIme = 0.0; // ��Ÿ Ÿ���� 0���� ����
		}
	}
}


