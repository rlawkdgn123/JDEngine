#pragma once
#include "pch.h"

namespace Interface {

    /// <summary>
    /// Timer.h
    /// </summary>
    class GameTimer {
    public:
        virtual ~GameTimer() = default;

        // ������� �帥 ��ü �ð�(�� ����).
        virtual float TotalTime() const = 0;

        // �� �������� ó���Ǵ� �� �ɸ��� �ð�.
        virtual float DeltaTime() const = 0;

        // ���� ���������� �и��� ������ ��ȯ (�߰��� �޼���).
        virtual float DeltaTimeMS() const = 0;

        // Ÿ�̸� ���� (���� ���� �ʱ�ȭ)
        virtual void Reset() = 0;

        // Ÿ�̸� ����
        virtual void Start() = 0;

        // Ÿ�̸� �Ͻ� ����
        virtual void Stop() = 0;

        // �����Ӹ��� ȣ��Ǿ� DeltaTime�� ���
        virtual void Tick() = 0;
    };
}
