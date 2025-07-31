// CameraFader.h
#pragma once
#include <d2d1_3.h>

class CameraFader {
public:
    CameraFader()
        : m_state(State::None)
        , m_last(State::None)
        , m_duration(0)
        , m_elapsed(0)
    {
    }

    // 검정→투명
    void FadeIn(float duration) {
        m_state = State::FadingIn;
        m_last = State::FadingIn;
        m_duration = duration;
        m_elapsed = 0.f;
    }
    // 투명→검정
    void FadeOut(float duration) {
        m_state = State::FadingOut;
        m_last = State::FadingOut;
        m_duration = duration;
        m_elapsed = 0.f;
    }

    // 매 프레임(초 단위) 호출
    void Update(float dt) {
        if (m_state == State::None) return;
        m_elapsed += dt;
        if (m_elapsed >= m_duration) {
            m_elapsed = m_duration;
            m_state = State::None;
        }
    }

    // 화면 전체에 검정 사각형을 덮어 그리기
    void Render(ID2D1DeviceContext7* ctx, D2D1_SIZE_F screenSize) {
        // 아직 한 번도 페이드하지 않았다면 아무것도 하지 않음
        if (m_last == State::None)
            return;

        // 진행 비율 0→1
        float t = (m_duration > 0.f) ? (m_elapsed / m_duration) : 1.f;
        // 알파 계산
        float alpha = (m_last == State::FadingIn)
            ? (1.f - t)  // FadeIn: 1→0
            : (t);       // FadeOut: 0→1

        // 완전히 투명(α==0)이면 그릴 필요 없음
        if (alpha <= 0.f)
            return;

        // 검정 브러시 생성/업데이트
        static Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
        D2D1_COLOR_F c = D2D1::ColorF(D2D1::ColorF::Black, alpha);
        if (!brush) {
            ctx->CreateSolidColorBrush(c, &brush);
        }
        else {
            brush->SetColor(c);
        }

        // 화면 전체 덮기
        D2D1_RECT_F r = D2D1::RectF(0.f, 0.f, screenSize.width, screenSize.height);
        ctx->FillRectangle(r, brush.Get());
    }

    bool IsBusy() const {
        return m_state != State::None;
    }

private:
    enum class State { None, FadingIn, FadingOut };
    State m_state;   // 현재 진행 중인 페이드
    State m_last;    // 마지막 실행했던 페이드 타입
    float m_duration;
    float m_elapsed;
};
