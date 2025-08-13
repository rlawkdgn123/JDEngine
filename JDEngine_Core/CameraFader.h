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

    // 검정→toColor (from 현재 알파 → toAlpha)
    void FadeIn(float duration, float toAlpha, D2D1_COLOR_F toColor) {
        StartFade(duration, m_currentAlpha, toAlpha, m_currentColor, toColor);
    }
    // 투명(또는 현재컬러)→toColor (from 현재 알파 → toAlpha)
    void FadeOut(float duration, float toAlpha, D2D1_COLOR_F toColor) {
        StartFade(duration, m_currentAlpha, toAlpha, m_currentColor, toColor);
    }

    // fromAlpha→toAlpha, fromColor→toColor
    void Fade(float duration,
        float fromAlpha, D2D1_COLOR_F fromColor,
        float toAlpha, D2D1_COLOR_F toColor)
    {
        StartFade(duration, fromAlpha, toAlpha, fromColor, toColor);
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
        // 색상도 보간
        m_currentColor.r = m_startColor.r + (m_targetColor.r - m_startColor.r) * t;
        m_currentColor.g = m_startColor.g + (m_targetColor.g - m_startColor.g) * t;
        m_currentColor.b = m_startColor.b + (m_targetColor.b - m_startColor.b) * t;
    }

    // 화면 전체에 컬러 사각형 덮어 그리기
    void Render(ID2D1DeviceContext7* ctx, D2D1_SIZE_F screenSize) {
        if (m_state == State::None && m_currentAlpha <= 0.f)
            return;
        if (m_currentAlpha <= 0.f)
            return;

        // 브러시 생성/업데이트
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

        // 전체 화면 덮기
        D2D1_RECT_F rect = D2D1::RectF(0.f, 0.f, screenSize.width, screenSize.height);
        ctx->FillRectangle(rect, brush.Get());
    }

    bool IsBusy() const {
        return m_state != State::None;
    }

    // 1) 스크린 좌표의 사각형만 페이드 (컨텍스트 트랜스폼 = Identity 가정)
    void RenderRect(ID2D1DeviceContext7* ctx, D2D1_RECT_F rect) {
        if ((m_state == State::None && m_currentAlpha <= 0.f) || m_currentAlpha <= 0.f) return;
        EnsureBrush(ctx);

        // 현재 컬러/알파 적용
        m_brush->SetColor(D2D1::ColorF(m_currentColor.r, m_currentColor.g, m_currentColor.b, m_currentAlpha));
        ctx->FillRectangle(rect, m_brush.Get());
    }

    // 2) (pos, size, pivot)로 영역 지정해서 페이드
    //    pos: 기준점(보통 오브젝트의 월드/스크린 위치), 
    //    size: 픽셀 크기, 
    //    pivot: [0..1], (0.5,0.5=중심) 
    void RenderRegionAt(ID2D1DeviceContext7* ctx,
        D2D1_POINT_2F pos, D2D1_SIZE_F size,
        D2D1_POINT_2F pivot = { 0.5f, 0.5f })
    {
        const float left = pos.x - size.width * pivot.x;
        const float top = pos.y - size.height * pivot.y;
        const float right = left + size.width;
        const float bottom = top + size.height;
        RenderRect(ctx, D2D1::RectF(left, top, right, bottom));
    }

    // 3) 로컬 사각형을 월드 행렬로 변환해서 페이드 (회전/스케일 반영)
    //    rectLocal: (0,0)-(w,h) 같은 로컬 영역
    //    world:     오브젝트 월드 행렬 (D2D1::Matrix3x2F)
    void RenderRectTransformed(ID2D1DeviceContext7* ctx,
        D2D1_RECT_F rectLocal,
        D2D1_MATRIX_3X2_F world)
    {
        if ((m_state == State::None && m_currentAlpha <= 0.f) || m_currentAlpha <= 0.f) return;
        EnsureBrush(ctx);

        // 컨텍스트에 월드 행렬 적용 → 로컬 좌표로 사각형 채우기
        D2D1_MATRIX_3X2_F prev;
        ctx->GetTransform(&prev);
        ctx->SetTransform(world * prev);

        m_brush->SetColor(D2D1::ColorF(m_currentColor.r, m_currentColor.g, m_currentColor.b, m_currentAlpha));
        ctx->FillRectangle(rectLocal, m_brush.Get());

        ctx->SetTransform(prev);
    }
private:
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_brush;

    void EnsureBrush(ID2D1DeviceContext7* ctx) {
        if (!m_brush) {
            ctx->CreateSolidColorBrush(
                D2D1::ColorF(m_currentColor.r, m_currentColor.g, m_currentColor.b, m_currentAlpha),
                &m_brush);
        }
    }

    enum class State { None, Fading };
    State m_state;
    float m_duration;
    float m_elapsed;

    // 알파 보간용
    float m_startAlpha;
    float m_targetAlpha;
    float m_currentAlpha;

    // 색상 보간용
    D2D1_COLOR_F m_startColor;
    D2D1_COLOR_F m_targetColor;
    D2D1_COLOR_F m_currentColor;
    // C++14용 clamp
    static float Clamp(float v, float lo, float hi) {
        return v < lo ? lo : (v > hi ? hi : v);
    }

    // FadeIn/FadeOut 호출용
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
