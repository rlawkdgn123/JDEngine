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

    // ����������
    void FadeIn(float duration) {
        m_state = State::FadingIn;
        m_last = State::FadingIn;
        m_duration = duration;
        m_elapsed = 0.f;
    }
    // ��������
    void FadeOut(float duration) {
        m_state = State::FadingOut;
        m_last = State::FadingOut;
        m_duration = duration;
        m_elapsed = 0.f;
    }

    // �� ������(�� ����) ȣ��
    void Update(float dt) {
        if (m_state == State::None) return;
        m_elapsed += dt;
        if (m_elapsed >= m_duration) {
            m_elapsed = m_duration;
            m_state = State::None;
        }
    }

    // ȭ�� ��ü�� ���� �簢���� ���� �׸���
    void Render(ID2D1DeviceContext7* ctx, D2D1_SIZE_F screenSize) {
        // ���� �� ���� ���̵����� �ʾҴٸ� �ƹ��͵� ���� ����
        if (m_last == State::None)
            return;

        // ���� ���� 0��1
        float t = (m_duration > 0.f) ? (m_elapsed / m_duration) : 1.f;
        // ���� ���
        float alpha = (m_last == State::FadingIn)
            ? (1.f - t)  // FadeIn: 1��0
            : (t);       // FadeOut: 0��1

        // ������ ����(��==0)�̸� �׸� �ʿ� ����
        if (alpha <= 0.f)
            return;

        // ���� �귯�� ����/������Ʈ
        static Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
        D2D1_COLOR_F c = D2D1::ColorF(D2D1::ColorF::Black, alpha);
        if (!brush) {
            ctx->CreateSolidColorBrush(c, &brush);
        }
        else {
            brush->SetColor(c);
        }

        // ȭ�� ��ü ����
        D2D1_RECT_F r = D2D1::RectF(0.f, 0.f, screenSize.width, screenSize.height);
        ctx->FillRectangle(r, brush.Get());
    }

    bool IsBusy() const {
        return m_state != State::None;
    }

private:
    enum class State { None, FadingIn, FadingOut };
    State m_state;   // ���� ���� ���� ���̵�
    State m_last;    // ������ �����ߴ� ���̵� Ÿ��
    float m_duration;
    float m_elapsed;
};
