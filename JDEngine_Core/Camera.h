#pragma once
using Vector2F = JDMath::Vector2F;

class Camera
{
public:
    void SetPosition(float x, float y) { m_position = { x, y }; }
    void Move(float dx, float dy) {m_position.x += dx; m_position.y += dy; }

    void SetZoom(float zoom) { m_zoom = zoom; }
    float GetZoom() const { return m_zoom; }
    void Zoom(float factor, D2D1_POINT_2F screenCenter);

    void SetRotation(float angleInDegrees) { m_rotation = angleInDegrees; }
    void Rotate(float angleDelta) { m_rotation += angleDelta; }

    void Shake(float intensity, float duration);
    void Update(float dt);

    void SetScreenSize(float width, float height) noexcept
    {
        m_screenWidth = width;
        m_screenHeight = height;
    }

    float GetScreenWidth() const noexcept { return m_screenWidth;}
    float GetScreenHeight() const noexcept { return m_screenHeight;}

    D2D1_MATRIX_3X2_F GetViewMatrix() const;

    inline D2D1_POINT_2F TransformPoint(const D2D1_MATRIX_3X2_F& mat, const D2D1_POINT_2F& pt)
    {
        return D2D1::Point2F(
            pt.x * mat._11 + pt.y * mat._21 + mat._31,
            pt.x * mat._12 + pt.y * mat._22 + mat._32
        );
    }

    Vector2F ScreenToWorldPoint(const Vector2F& screenPoint) const;
    Vector2F WorldToScreenPoint(const Vector2F& worldPoint) const;

private:
    D2D1_POINT_2F m_position = { 0, 0 };
    float m_zoom = 1.0f;
    float m_rotation = 0.0f;

    float m_shakeTime = 0.f;
    float m_shakeDuration = 0.f;
    float m_shakeIntensity = 0.f;

    float m_screenWidth = 0.f;    // 렌더 타겟 너비(px)
    float m_screenHeight = 0.f;   // 렌더 타겟 높이(px)
};
