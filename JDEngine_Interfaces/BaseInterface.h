#pragma once
#include "pch.h"

namespace Interface {

    /// <summary>
    /// JDWndBase : ������ ���ν��� ������ �������̽�
    /// </summary>
    class JDWndBase {
    public:
        JDWndBase() = default;
        virtual ~JDWndBase() = default;

        // ������ ����
        bool Create(const wchar_t* className, const wchar_t* windowName, int width, int height);

        // ������ ����
        void Destroy();

        // ������ �ڵ� ��ȯ
        void* GetHandle() const { return m_hWnd; }

    protected:
        // �ܺ� �Լ����� �� Ŭ������ private/protected ��� ���� ���
        friend LRESULT CALLBACK JDWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

        // ������ ���ν��� (�⺻ ����: false ��ȯ)
        virtual bool OnWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) { return false; }

        // ������ ������ ������ (�⺻ ������ ���� ��� ����)
        virtual void OnResize(int width, int height) { /* �⺻ ���� ���� */ }

        // ������ ���� ó�� - ���� ���� �Լ� (�߻�ȭ)
        virtual void OnClose() = 0;

        HWND m_hWnd = nullptr;  // ������ �ڵ�
        int m_width = 0;        // ������ ���γ���
        int m_height = 0;       // ������ ���γ���

        // ���� �� �̵� ������ ����
        JDWndBase(const JDWndBase&) = delete;
        JDWndBase& operator=(const JDWndBase&) = delete;
        JDWndBase(JDWndBase&&) = delete;
        JDWndBase& operator=(JDWndBase&&) = delete;
    };


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
