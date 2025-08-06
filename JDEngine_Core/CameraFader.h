#pragma once
#include <d2d1_3.h>
#include <wrl/client.h>
#include <algorithm>

class CameraFader {
public:
    CameraFader()
        : m_state(State::None)
        , m_duration(0.f)
        , m_elapsed(0.f)
        , m_startAlpha(0.f)
        , m_targetAlpha(0.f)
        , m_currentAlpha(0.f)
        , m_currentColor{ 0,0,0,1 }
    {
    }

    // ������toColor (from ���� ���� �� toAlpha)
    void FadeIn(float duration, float toAlpha, D2D1_COLOR_F toColor) {
        StartFade(duration, m_currentAlpha, toAlpha, m_currentColor, toColor);
    }
    // ����(�Ǵ� �����÷�)��toColor (from ���� ���� �� toAlpha)
    void FadeOut(float duration, float toAlpha, D2D1_COLOR_F toColor) {
        StartFade(duration, m_currentAlpha, toAlpha, m_currentColor, toColor);
    }

    // fromAlpha��toAlpha, fromColor��toColor
    void Fade(float duration,
        float fromAlpha, D2D1_COLOR_F fromColor,
        float toAlpha, D2D1_COLOR_F toColor)
    {
        StartFade(duration, fromAlpha, toAlpha, fromColor, toColor);
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
        // ���� ����
        m_currentColor.r = m_startColor.r + (m_targetColor.r - m_startColor.r) * t;
        m_currentColor.g = m_startColor.g + (m_targetColor.g - m_startColor.g) * t;
        m_currentColor.b = m_startColor.b + (m_targetColor.b - m_startColor.b) * t;
    }

    // ȭ�� ��ü�� �÷� �簢�� ���� �׸���
    void Render(ID2D1DeviceContext7* ctx, D2D1_SIZE_F screenSize) {
        if (m_state == State::None && m_currentAlpha <= 0.f)
            return;
        if (m_currentAlpha <= 0.f)
            return;

        // �귯�� ����/������Ʈ
        static Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
        D2D1_COLOR_F color = {
            m_currentColor.r,
            m_currentColor.g,
            m_currentColor.b,
            m_currentAlpha
        };
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

    // ���� ������
    float m_startAlpha;
    float m_targetAlpha;
    float m_currentAlpha;

    // ���� ������
    D2D1_COLOR_F m_startColor;
    D2D1_COLOR_F m_targetColor;
    D2D1_COLOR_F m_currentColor;
    // C++14�� clamp
    static float Clamp(float v, float lo, float hi) {
        return v < lo ? lo : (v > hi ? hi : v);
    }

    // FadeIn/FadeOut ȣ���
    void StartFade(float duration,
        float fromAlpha, float toAlpha,
        D2D1_COLOR_F fromColor, D2D1_COLOR_F toColor)
    {
        m_duration = duration;
        m_elapsed = 0.f;
        m_startAlpha = Clamp(fromAlpha, 0.f, 1.f);
        m_targetAlpha = Clamp(toAlpha, 0.f, 1.f);
        m_currentAlpha = m_startAlpha;

        m_startColor = fromColor;
        m_targetColor = toColor;
        m_currentColor = fromColor;

        m_state = State::Fading;
    }
};
