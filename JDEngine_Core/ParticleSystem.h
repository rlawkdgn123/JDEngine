#pragma once
#include "pch.h"
#include "framework.h"
#include "MathUtil.h"
#include "MathType.h"
#include <random>
#include <d2d1.h>
#include <d2d1_1.h>
using namespace JDGlobal::Math;

struct Particle {
    Vector2F pos;
    Vector2F vel;
    float     life;      // 남은 수명 (초)
    float     size;      // 크기 (픽셀)
    float     alpha;     // 투명도 (0~1)
    D2D1::ColorF color;  // 파티클 색상

    float imageScale;
    float rotation;
    float rotationSpeed;

    Particle()
        : pos{ 0,0 }
        , vel{ 0,0 }
        , life{ 0 }
        , size{ 0 }
        , alpha{ 0 }
        , color{ 0.f,0.f,0.f,0.f }  // 원하는 초기값
        , imageScale{ 1.0f }
        , rotation{ 0.f }
        , rotationSpeed{ 0.0f }
    {}
};

static std::mt19937_64 rng{ std::random_device{}() };

// lo ≤ x < hi 범위의 랜덤 실수 생성 함수
static float RandomFloat(float lo, float hi) {
    std::uniform_real_distribution<float> dist(lo, hi);
    return dist(rng);
}

class ParticleSystem {
public:
    std::vector<Particle> particles;
    float emissionRate = 20;      // 초당 생성 개수
    float emitAccumulator = 0;    // 생성 누적 시간
    ID2D1SolidColorBrush* brush = nullptr;   // 브러시

    ID2D1Bitmap* particleBitmap = nullptr;
    // 자동 방출에 사용할 기본 색상
    D2D1::ColorF defaultColor = D2D1::ColorF(1.f, 1.f, 1.f, 1.f);

    ParticleSystem(ID2D1DeviceContext7* ctx) {
        // 초기 브러시(흰색, 완전 불투명)
        ctx->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 1), &brush);

        particleBitmap = AssetManager::Instance().GetTexture("blossom");
    }

    // 수정: color 파라미터 추가, 세미콜론 빠진 부분 보강
    void SpawnParticle(const Vector2F& pos, D2D1::ColorF color, float scale = 1.0f) {
        Particle p;
        p.pos = pos;
        float angle = RandomFloat(0.f, 2.f * 3.14159265f);
        float speed = RandomFloat(20.f, 80.f);
        p.vel = { std::cos(angle) * speed, std::sin(angle) * speed };
        p.life = RandomFloat(0.5f, 1.f);
        p.size = RandomFloat(10.f, 12.f);
        p.alpha = 1.f;
        p.color = color;
        p.imageScale = scale;
        particles.push_back(p);
    }
    
    void SpawnFallingParticle(const Vector2F& pos,
        D2D1::ColorF color,
        float scale = 1.0f,
        float maxLife = 4.0f)
    {
        Particle p;
        p.pos = pos;
        // 좌우 drift: -20~+20 픽셀/sec, 아래 속도: 50~80 픽셀/sec
        p.vel = { RandomFloat(-80.f, -20.f), RandomFloat(50.f, 80.f) };
        p.life = maxLife;
        p.size = RandomFloat(10.f, 12.f);
        p.alpha = 1.f;
        p.color = color;
        p.imageScale = scale;
        p.rotation = RandomFloat(0.f, 360.f);
        p.rotationSpeed = RandomFloat(-30.f, 30.f);
        particles.push_back(p);
    }
    // dt 동안 자동 방출 + 기존 파티클 업데이트
    void Update(float dt, const Vector2F& emitterPos) {
        // 1) 자동 방출 (defaultColor 사용)
        emitAccumulator += dt * emissionRate;
        int count = static_cast<int>(emitAccumulator);
        emitAccumulator -= count;
        for (int i = 0; i < count; ++i) {
            SpawnParticle(emitterPos, defaultColor);
        }

        // 2) 기존 파티클 업데이트
        for (auto it = particles.begin(); it != particles.end();) {
            it->life -= dt;
            it->pos += it->vel * dt;
            it->alpha = (it->life > 0.f) ? it->life : 0.f;
            it->size *= it->alpha;
            if (it->life <= 0.f) it = particles.erase(it);
            else                 ++it;
        }
    }

    void UpdateFalling(float dt, float screenWidth, float screenHeight) {
        for (auto it = particles.begin(); it != particles.end();) {
            it->life -= dt;
            it->pos += it->vel * dt;
            //회전
            it->rotation += it->rotationSpeed * dt;
            // life 비율로 alpha 조정
            it->alpha = (it->life > 0.f) ? (it->life / 4.0f) : 0.f;

            // 화면 아래 + life 다 소진 시 삭제
            if (it->life <= 0.f || it->pos.y > screenHeight + 50.f)
                it = particles.erase(it);
            else
                ++it;
        }
    }

    // 지정 위치에서 원하는 색으로 한 번에 방출
    void Emit(const Vector2F& pos, int count, D2D1::ColorF color, float scale = 1.0f) {
        for (int i = 0; i < count; ++i) {
            SpawnParticle(pos, color, scale);
        }
    }

    void SetParticleLife(float life) { Particle p; p.life = life; }

    // 렌더링: 각 파티클의 color와 alpha 적용
    void Render(ID2D1DeviceContext7* ctx) {
        //ctx->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_ADD);
        ctx->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);

        D2D1_MATRIX_3X2_F oldTM;
        ctx->GetTransform(&oldTM);

        for (auto& p : particles) {
            if (particleBitmap) {
                //회전
                D2D1_POINT_2F center = { p.pos.x, p.pos.y };
                auto rot = D2D1::Matrix3x2F::Rotation(p.rotation, center);
                ctx->SetTransform(rot * oldTM);
                // 그릴 위치와 크기 결정
                float half = p.size * p.imageScale * 0.5f;
                D2D1_RECT_F dest = {
                  p.pos.x - half, p.pos.y - half,
                  p.pos.x + half, p.pos.y + half
                };
                // 알파만 조절
                ctx->DrawBitmap(
                    particleBitmap,
                    dest,
                    p.alpha,                                   // opacity
                    D2D1_INTERPOLATION_MODE_LINEAR,
                    D2D1_RECT_F{ 0,0,
                      (float)particleBitmap->GetPixelSize().width,
                      (float)particleBitmap->GetPixelSize().height }
                );
            }
            else {
                // 텍스처 없으면 기존 방식
                brush->SetColor(D2D1::ColorF(p.color.r, p.color.g, p.color.b, p.alpha));
                D2D1_ELLIPSE e = D2D1::Ellipse({ p.pos.x, p.pos.y }, p.size, p.size);
                ctx->FillEllipse(e, brush);
            }

            ctx->SetTransform(oldTM);

        }
        //for (auto& p : particles) {
        //    // p.color의 RGB + p.alpha 이용
        //    brush->SetColor(D2D1::ColorF(p.color.r, p.color.g, p.color.b, p.alpha));
        //    D2D1_ELLIPSE e = D2D1::Ellipse(
        //        D2D1::Point2F(p.pos.x, p.pos.y),
        //        p.size, p.size
        //    );
        //    ctx->FillEllipse(e, brush);
        //}
        ctx->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);
    }
};
