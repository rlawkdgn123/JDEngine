#pragma once  

#ifndef GAMETIMER_H  
#define GAMETIMER_H  

class GameTimer  
{  
public:  
	GameTimer();  

	float TotalTime()const;		// ������� �帥 ��ü �ð�(�� ����).
	float DeltaTime()const;		// �� �������� ó���Ǵ� �� �ɸ��� �ð�.

	float DeltaTimeMS()const;	// ���� ���������� �и��� ������ ��ȯ (�߰��� �޼���).

	void Reset();	// Ÿ�̸� ���� (���� ���� �ʱ�ȭ)
	void Start();	// Ÿ�̸� ����
	void Stop();	// Ÿ�̸� �Ͻ� ����
	void Tick();	// �����Ӹ��� ȣ��Ǿ� DeltaTime�� ���
private:  
	double m_secondsPerCount;   // CPU Ÿ�̸� �ֱ⸦ �� ������ ȯ���� ��
	double m_deltaTIme;			// �� ������ �� �ð� ���� (��)

	__int64 m_baseTime;			// Ÿ�̸� ���� ���� ����
	__int64 m_pausedTime;		// �Ͻ� ���� ���� ������ �ð� (�� �ð����� �����ϱ� ���� �ʿ�)
	__int64 m_stopTime;			// Ÿ�̸Ӱ� ���� ����
	__int64 m_prevTime;			// ���� �������� �ð� (Tick() ȣ�� ����)
	__int64 m_currTime;			// ���� �������� �ð�

	bool m_stopped = false;		// �Ͻ� ���� ���� �÷���.
};  

#endif // GAMETIMER_H
