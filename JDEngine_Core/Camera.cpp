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
    if (factor <= 0.0f) return;

    Vector2F logicalScreenCenter = { screenCenter.x, screenCenter.y };

    Vector2F worldBeforeZoom = ScreenToWorldPoint(logicalScreenCenter);

    m_zoom *= factor;
    m_zoom = JDMath::Clamp(m_zoom, 0.1f, 10.0f);

    Vector2F worldAfterZoom = ScreenToWorldPoint(logicalScreenCenter);

    m_position.x += (worldBeforeZoom.x - worldAfterZoom.x);
    m_position.y += (worldBeforeZoom.y - worldAfterZoom.y);

    //if (factor <= 0.0f)
    //    return;

    //// 1. 현재 ViewMatrix의 역행렬 구하기
    //D2D1_MATRIX_3X2_F inverseView = GetViewMatrix();
    //if (!::D2D1InvertMatrix(&inverseView))
    //    return;

    //D2D1_POINT_2F worldBeforeZoom = TransformPoint(inverseView, screenCenter);

    //// 2. 줌 조절
    //m_zoom *= factor;
    //if (m_zoom < 0.1f) m_zoom = 0.1f;
    //if (m_zoom > 10.f) m_zoom = 10.f;

    //// 3. 새로운 ViewMatrix의 역행렬 구하기
    //D2D1_MATRIX_3X2_F newInverseView = GetViewMatrix();
    //if (!::D2D1InvertMatrix(&newInverseView))
    //    return;

    //D2D1_POINT_2F worldAfterZoom = TransformPoint(newInverseView, screenCenter);

    //std::cout << screenCenter.x << " " << screenCenter.y << std::endl;
    //// 4. 위치 보정
    //m_position.x += (worldBeforeZoom.x - worldAfterZoom.x);
    //m_position.y += -(worldBeforeZoom.y - worldAfterZoom.y);
}

D2D1_MATRIX_3X2_F Camera::GetViewMatrix() const
{
    // 뷰 행렬은 월드를 카메라 시점으로 옮기는 변환으로, Scale * Rotation * Translation(inverse) 순서가 표준적입니다.

    // 1. 줌 (카메라에 가까워지면 세상이 커보임)
    auto S = D2D1::Matrix3x2F::Scale(m_zoom, m_zoom);

    // 2. 회전 (카메라가 회전하면 세상은 반대로 회전함)
    auto R = D2D1::Matrix3x2F::Rotation(-m_rotation);

    // 3. 이동 (카메라가 이동하면 세상은 반대로 이동함)
    float shakeX = 0.f, shakeY = 0.f;
    if (m_shakeTime < m_shakeDuration) {
        shakeX = ((rand() % 200 - 100) / 100.f) * m_shakeIntensity;
        shakeY = ((rand() % 200 - 100) / 100.f) * m_shakeIntensity;
    }
    auto T = D2D1::Matrix3x2F::Translation(-m_position.x + shakeX, -m_position.y + shakeY);

    // 최종 뷰 행렬 = S * R * T
    // Y축 반전과 화면 중앙 이동은 이제 Renderer가 담당하므로 여기서는 하지 않습니다.
    //return S * R * T;
    return T * R * S;

    // LEGACY
    //// 기본 단위 행렬
    //D2D1_MATRIX_3X2_F mat = D2D1::Matrix3x2F::Identity();

    //// 1. 카메라 위치 반대로 이동 (월드 → 카메라 좌표)
    //float shakeX = 0.f, shakeY = 0.f;
    //if (m_shakeTime < m_shakeDuration)
    //{
    //    shakeX = ((rand() % 200 - 100) / 100.f) * m_shakeIntensity;
    //    shakeY = ((rand() % 200 - 100) / 100.f) * m_shakeIntensity;
    //}
    //mat = mat * D2D1::Matrix3x2F::Translation(-m_position.x + shakeX, -m_position.y + shakeY);

    //// 2. 회전 (Unity 방식: 반시계방향이 양수)
    //mat = mat * D2D1::Matrix3x2F::Rotation(m_rotation);

    //// 3. 줌 적용 (Y축 반전 없이)
    //mat = mat * D2D1::Matrix3x2F::Scale(m_zoom, m_zoom);

    //// 4. 화면 중심으로 이동
    //mat = mat * D2D1::Matrix3x2F::Translation(m_screenWidth * 0.5f, m_screenHeight * 0.5f);

    //// 5. 마지막에 Y축 반전 (Unity → Direct2D 좌표계 변환)
    //mat = mat * D2D1::Matrix3x2F::Scale(1.0f, -1.0f,
    //    D2D1::Point2F(m_screenWidth * 0.5f, m_screenHeight * 0.5f));

    //return mat;
    
    // LEGACY : 좌표계 섞여 있어서 다시 만듦. 백승주
    /*
    //// 기본 단위 행렬
    //D2D1_MATRIX_3X2_F mat = D2D1::Matrix3x2F::Identity();

    //// 1. 스케일 (줌 + Y축 반전)
    //mat = mat * D2D1::Matrix3x2F::Scale(m_zoom, -m_zoom);

    //// 2. 회전 (반시계 방향, Unity와 동일)
    //mat = mat * D2D1::Matrix3x2F::Rotation(m_rotation);

    //// 3. 흔들림 적용
    //float shakeX = 0.f, shakeY = 0.f;
    //if (m_shakeTime < m_shakeDuration)
    //{
    //    shakeX = ((rand() % 200 - 100) / 100.f) * m_shakeIntensity;
    //    shakeY = ((rand() % 200 - 100) / 100.f) * m_shakeIntensity;
    //}

    //// 4. 카메라 위치 반대로 이동
    //mat = mat * D2D1::Matrix3x2F::Translation(-m_position.x + shakeX, -m_position.y + shakeY);

    //// 5. 화면 중심으로 이동 (중심 원점 보정)
    //mat = mat * D2D1::Matrix3x2F::Translation(m_screenWidth * 0.5f, m_screenHeight * 0.5f);

    //return mat;
    */
}

Vector2F Camera::ScreenToWorldPoint(const Vector2F& screenPoint) const
{
    // 렌더링 파이프라인의 완벽한 역순:
    // Screen -> Inverse Projection -> Inverse View -> World

    // 1. 프로젝션 행렬의 역행렬을 계산합니다.
    //    (D2DRenderer::RenderGameObject 에서 사용한 프로젝션 행렬과 동일해야 함)
    float screenW = GetScreenWidth();
    float screenH = GetScreenHeight();

    // 프로젝션: Y축 뒤집기 -> 화면 중앙으로 이동
    auto S_flipY = D2D1::Matrix3x2F::Scale(1.0f, -1.0f);
    auto T_center = D2D1::Matrix3x2F::Translation(screenW * 0.5f, screenH * 0.5f);
    D2D1_MATRIX_3X2_F projectionMatrix = S_flipY * T_center;

    // 역 프로젝션 계산
    D2D1InvertMatrix(&projectionMatrix);

    // 2. 뷰 행렬의 역행렬을 계산합니다.
    D2D1_MATRIX_3X2_F viewMatrix = GetViewMatrix();
    D2D1InvertMatrix(&viewMatrix);

    // 3. 최종 '화면 -> 월드' 변환 행렬
    //    역변환은 곱셈 순서도 반대입니다: inv(Proj) * inv(View)
    D2D1_MATRIX_3X2_F screenToWorldMatrix = projectionMatrix * viewMatrix;

    // 4. 스크린 좌표에 최종 역행렬을 곱합니다.
    D2D1_POINT_2F sp = { screenPoint.x, screenPoint.y };
    D2D1_POINT_2F worldPoint;
    worldPoint.x = sp.x * screenToWorldMatrix._11 + sp.y * screenToWorldMatrix._21 + screenToWorldMatrix._31;
    worldPoint.y = sp.x * screenToWorldMatrix._12 + sp.y * screenToWorldMatrix._22 + screenToWorldMatrix._32;

    return { worldPoint.x, worldPoint.y };

    //// 1. 스크린 중심 기준으로 변환
    //float centeredX = screenPoint.x - (m_screenWidth * 0.5f);
    //float centeredY = screenPoint.y - (m_screenHeight * 0.5f);

    //// 2. 줌 적용 (역변환)
    //float worldX = centeredX / m_zoom;
    //float worldY = centeredY / m_zoom;

    //// 3. 회전 적용 (역변환)
    //float cosR = cos(-m_rotation * (3.14159f / 180.0f));  // 역회전
    //float sinR = sin(-m_rotation * (3.14159f / 180.0f));

    //float rotatedX = worldX * cosR - worldY * sinR;
    //float rotatedY = worldX * sinR + worldY * cosR;

    //// 4. 카메라 위치 적용
    //return Vector2F(rotatedX + m_position.x, rotatedY + m_position.y);
}

Vector2F Camera::WorldToScreenPoint(const Vector2F& worldPoint) const
{
    // 렌더링 파이프라인과 완벽하게 동일한 순서:
    // World -> View -> Projection -> Screen

    D2D1_POINT_2F pt = { worldPoint.x, worldPoint.y };

    // 1. 뷰 행렬 적용
    D2D1_MATRIX_3X2_F viewMatrix = GetViewMatrix();

    // 2. 프로젝션 행렬 적용
    float screenW = GetScreenWidth();
    float screenH = GetScreenHeight();
    auto S_flipY = D2D1::Matrix3x2F::Scale(1.0f, -1.0f);
    auto T_center = D2D1::Matrix3x2F::Translation(screenW * 0.5f, screenH * 0.5f);
    D2D1_MATRIX_3X2_F projectionMatrix = S_flipY * T_center;

    // 3. 최종 변환 행렬
    D2D1_MATRIX_3X2_F worldToScreenMatrix = viewMatrix * projectionMatrix;

    // 4. 월드 좌표에 최종 행렬을 곱합니다.
    D2D1_POINT_2F finalScreenPoint;
    finalScreenPoint.x = pt.x * worldToScreenMatrix._11 + pt.y * worldToScreenMatrix._21 + worldToScreenMatrix._31;
    finalScreenPoint.y = pt.x * worldToScreenMatrix._12 + pt.y * worldToScreenMatrix._22 + worldToScreenMatrix._32;

    return { finalScreenPoint.x, finalScreenPoint.y };

    //// 1. 카메라 위치 기준으로 변환
    //float relativeX = worldPoint.x - m_position.x;
    //float relativeY = worldPoint.y - m_position.y;

    //// 2. 회전 적용
    //float cosR = cos(m_rotation * (3.14159f / 180.0f));
    //float sinR = sin(m_rotation * (3.14159f / 180.0f));

    //float rotatedX = relativeX * cosR - relativeY * sinR;
    //float rotatedY = relativeX * sinR + relativeY * cosR;

    //// 3. 줌 적용
    //float zoomedX = rotatedX * m_zoom;
    //float zoomedY = rotatedY * m_zoom;

    //// 4. 스크린 중심으로 이동
    //float screenX = zoomedX + (m_screenWidth * 0.5f);
    //float screenY = zoomedY + (m_screenHeight * 0.5f);

    //return Vector2F(screenX, screenY);
}
