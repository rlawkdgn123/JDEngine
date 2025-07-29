#include "pch.h"
#include "framework.h"
#include "Camera.h"

void Camera::Shake(float intensity, float duration)
{
    m_shakeIntensity = intensity;
    m_shakeDuration = duration;
    m_shakeTime = 0.f;
}

void Camera::Update(float dt)
{
    if (m_shakeTime < m_shakeDuration)
        m_shakeTime += dt;
}

void Camera::Zoom(float factor, D2D1_POINT_2F screenCenter)
{
    
    if (factor <= 0.0f)
        return;

    // 1. 현재 ViewMatrix의 역행렬 구하기
    D2D1_MATRIX_3X2_F inverseView = GetViewMatrix();
    if (!::D2D1InvertMatrix(&inverseView))
        return;

    D2D1_POINT_2F worldBeforeZoom = TransformPoint(inverseView, screenCenter);

    // 2. 줌 조절
    m_zoom *= factor;
    if (m_zoom < 0.1f) m_zoom = 0.1f;
    if (m_zoom > 10.f) m_zoom = 10.f;

    // 3. 새로운 ViewMatrix의 역행렬 구하기
    D2D1_MATRIX_3X2_F newInverseView = GetViewMatrix();
    if (!::D2D1InvertMatrix(&newInverseView))
        return;

    D2D1_POINT_2F worldAfterZoom = TransformPoint(newInverseView, screenCenter);

    std::cout << screenCenter.x << " " << screenCenter.y << std::endl;
    // 4. 위치 보정
    m_position.x += (worldBeforeZoom.x - worldAfterZoom.x);
    m_position.y += -(worldBeforeZoom.y - worldAfterZoom.y);
}

D2D1_MATRIX_3X2_F Camera::GetViewMatrix() const
{
    
    // 기본 단위 행렬
    D2D1_MATRIX_3X2_F mat = D2D1::Matrix3x2F::Identity();

    // 1. 스케일 (줌 + Y축 반전)
    mat = mat * D2D1::Matrix3x2F::Scale(m_zoom, -m_zoom);

    // 2. 회전 (반시계 방향, Unity와 동일)
    mat = mat * D2D1::Matrix3x2F::Rotation(m_rotation);

    // 3. 흔들림 적용
    float shakeX = 0.f, shakeY = 0.f;
    if (m_shakeTime < m_shakeDuration)
    {
        shakeX = ((rand() % 200 - 100) / 100.f) * m_shakeIntensity;
        shakeY = ((rand() % 200 - 100) / 100.f) * m_shakeIntensity;
    }

    // 4. 카메라 위치 반대로 이동
    mat = mat * D2D1::Matrix3x2F::Translation(-m_position.x + shakeX, -m_position.y + shakeY);

    // 5. 화면 중심으로 이동 (중심 원점 보정)
    mat = mat * D2D1::Matrix3x2F::Translation(m_screenWidth * 0.5f, m_screenHeight * 0.5f);

    return mat;
}
