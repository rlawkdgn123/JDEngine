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

    // 1. ���� ViewMatrix�� ����� ���ϱ�
    D2D1_MATRIX_3X2_F inverseView = GetViewMatrix();
    if (!::D2D1InvertMatrix(&inverseView))
        return;

    D2D1_POINT_2F worldBeforeZoom = TransformPoint(inverseView, screenCenter);

    // 2. �� ����
    m_zoom *= factor;
    if (m_zoom < 0.1f) m_zoom = 0.1f;
    if (m_zoom > 10.f) m_zoom = 10.f;

    // 3. ���ο� ViewMatrix�� ����� ���ϱ�
    D2D1_MATRIX_3X2_F newInverseView = GetViewMatrix();
    if (!::D2D1InvertMatrix(&newInverseView))
        return;

    D2D1_POINT_2F worldAfterZoom = TransformPoint(newInverseView, screenCenter);

    std::cout << screenCenter.x << " " << screenCenter.y << std::endl;
    // 4. ��ġ ����
    m_position.x += (worldBeforeZoom.x - worldAfterZoom.x);
    m_position.y += -(worldBeforeZoom.y - worldAfterZoom.y);
}

D2D1_MATRIX_3X2_F Camera::GetViewMatrix() const
{
    
    // �⺻ ���� ���
    D2D1_MATRIX_3X2_F mat = D2D1::Matrix3x2F::Identity();

    // 1. ������ (�� + Y�� ����)
    mat = mat * D2D1::Matrix3x2F::Scale(m_zoom, -m_zoom);

    // 2. ȸ�� (�ݽð� ����, Unity�� ����)
    mat = mat * D2D1::Matrix3x2F::Rotation(m_rotation);

    // 3. ��鸲 ����
    float shakeX = 0.f, shakeY = 0.f;
    if (m_shakeTime < m_shakeDuration)
    {
        shakeX = ((rand() % 200 - 100) / 100.f) * m_shakeIntensity;
        shakeY = ((rand() % 200 - 100) / 100.f) * m_shakeIntensity;
    }

    // 4. ī�޶� ��ġ �ݴ�� �̵�
    mat = mat * D2D1::Matrix3x2F::Translation(-m_position.x + shakeX, -m_position.y + shakeY);

    // 5. ȭ�� �߽����� �̵� (�߽� ���� ����)
    mat = mat * D2D1::Matrix3x2F::Translation(m_screenWidth * 0.5f, m_screenHeight * 0.5f);

    return mat;
}
