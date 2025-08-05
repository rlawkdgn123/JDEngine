#pragma once
#include <d2d1_3.h>
#include <wrl/client.h>
#include <algorithm> // for std::min, std::max

class CameraFader {
public:
    CameraFader()
        : m_state(State::None)
        , m_duration(0.f)
        , m_elapsed(0.f)
        , m_startAlpha(0.f)
        , m_targetAlpha(0.f)
        , m_currentAlpha(0.f)
    {
    }

    // ���������� (���� ���� �� to)
    void FadeIn(float duration, float to) {
        StartFade(duration, m_currentAlpha, to);
    }
    // �������� (���� ���� �� to)
    void FadeOut(float duration, float to) {
        StartFade(duration, m_currentAlpha, to);
    }

    // ���� ���� ���̵�: from��to
    void Fade(float duration, float from, float to) {
        StartFade(duration, from, to);
    }

    // �� ������(�� ����) ȣ��
    void Update(float dt) {
        if (m_state == State::None)
            return;
        m_elapsed += dt;
        if (m_elapsed >= m_duration) {
            m_elapsed = m_duration;
            m_state = State::None;
        }
        float t = (m_duration > 0.f) ? (m_elapsed / m_duration) : 1.f;
        // ���� ����
        m_currentAlpha = m_startAlpha + (m_targetAlpha - m_startAlpha) * t;
    }

    // ȭ�� ��ü�� ���� �簢�� ���� �׸���
    void Render(ID2D1DeviceContext7* ctx, D2D1_SIZE_F screenSize) {
        if (m_state == State::None && m_currentAlpha <= 0.f)
            return;
        if (m_currentAlpha <= 0.f)
            return;

        // �귯�� ����/������Ʈ
        static Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
        D2D1_COLOR_F color = D2D1::ColorF(D2D1::ColorF::Black, m_currentAlpha);
        if (!brush) {
            ctx->CreateSolidColorBrush(color, &brush);
        }
        else {
            brush->SetColor(color);
        }

        // ��ü ȭ�� ����
        D2D1_RECT_F rect = D2D1::RectF(0.f, 0.f, screenSize.width, screenSize.height);
        ctx->FillRectangle(rect, brush.Get());
    }

    bool IsBusy() const {
        return m_state != State::None;
    }

private:
    enum class State { None, Fading };
    State m_state;
    float m_duration;
    float m_elapsed;
    float m_startAlpha;
    float m_targetAlpha;
    float m_currentAlpha;

    // C++14�� clamp
    static float Clamp(float v, float lo, float hi) {
        // Windows�� min/max ��ũ�� �浹�� ���ϱ� ���� ���� ����
        return (v < lo) ? lo : (v > hi ? hi : v);
    }

    void StartFade(float duration, float from, float to) {
        m_duration = duration;
        m_elapsed = 0.f;
        m_startAlpha = Clamp(from, 0.f, 1.f);
        m_targetAlpha = Clamp(to, 0.f, 1.f);
        m_currentAlpha = m_startAlpha;
        m_state = State::Fading;
    }
};
