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

    // 검정→투명 (현재 알파 → to)
    void FadeIn(float duration, float to) {
        StartFade(duration, m_currentAlpha, to);
    }
    // 투명→검정 (현재 알파 → to)
    void FadeOut(float duration, float to) {
        StartFade(duration, m_currentAlpha, to);
    }

    // 임의 구간 페이드: from→to
    void Fade(float duration, float from, float to) {
        StartFade(duration, from, to);
    }

    // 매 프레임(초 단위) 호출
    void Update(float dt) {
        if (m_state == State::None)
            return;
        m_elapsed += dt;
        if (m_elapsed >= m_duration) {
            m_elapsed = m_duration;
            m_state = State::None;
        }
        float t = (m_duration > 0.f) ? (m_elapsed / m_duration) : 1.f;
        // 선형 보간
        m_currentAlpha = m_startAlpha + (m_targetAlpha - m_startAlpha) * t;
    }

    // 화면 전체에 검정 사각형 덮어 그리기
    void Render(ID2D1DeviceContext7* ctx, D2D1_SIZE_F screenSize) {
        if (m_state == State::None && m_currentAlpha <= 0.f)
            return;
        if (m_currentAlpha <= 0.f)
            return;

        // 브러시 생성/업데이트
        static Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
        D2D1_COLOR_F color = D2D1::ColorF(D2D1::ColorF::Black, m_currentAlpha);
        if (!brush) {
            ctx->CreateSolidColorBrush(color, &brush);
        }
        else {
            brush->SetColor(color);
        }

        // 전체 화면 덮기
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

    // C++14용 clamp
    static float Clamp(float v, float lo, float hi) {
        // Windows의 min/max 매크로 충돌을 피하기 위해 직접 구현
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
