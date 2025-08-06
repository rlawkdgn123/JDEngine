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
    float     life;      // ���� ���� (��)
    float     size;      // ũ�� (�ȼ�)
    float     alpha;     // ���� (0~1)
};

static std::mt19937_64 rng{ std::random_device{}() };

// lo �� x < hi ������ ���� �Ǽ� ���� �Լ�
static float RandomFloat(float lo, float hi) {
    std::uniform_real_distribution<float> dist(lo, hi);
    return dist(rng);
}

class ParticleSystem {
public:
    std::vector<Particle> particles;
    float emissionRate = 20;      // �ʴ� ���� ����
    float emitAccumulator = 0;    // ���� ���� �ð�
    ID2D1SolidColorBrush* brush = nullptr;   // ���� �귯��(���, ���߿� �������� ����)


    ParticleSystem(ID2D1DeviceContext7* ctx) {
        // �⺻ ��� �귯�� �ϳ� ����
        ctx->CreateSolidColorBrush(D2D1::ColorF(1, 0, 0, 1), &brush);
    }

    void SpawnParticle(const Vector2F& pos) {
        Particle p;
        p.pos = pos;
        float angle = RandomFloat(0.f, 2.f * 3.14159265f);
        float speed = RandomFloat(20.f, 80.f);
        p.vel = { std::cos(angle) * speed, std::sin(angle) * speed };
        p.life = RandomFloat(0.5f, 1.f);
        p.size = RandomFloat(10.f, 12.f);
        p.alpha = 1.f;
        particles.push_back(p);
    }

    void Update(float dt, const Vector2F& emitterPos) {
        // �ڵ� ����
        emitAccumulator += dt * emissionRate;
        int count = static_cast<int>(emitAccumulator);
        emitAccumulator -= count;
        for (int i = 0; i < count; ++i) {
            SpawnParticle(emitterPos);
        }

        // ���� ��ƼŬ ������Ʈ
        for (auto it = particles.begin(); it != particles.end();) {
            it->life -= dt;
            it->pos += it->vel * dt;
            it->alpha = (it->life > 0.f) ? it->life : 0.f;
            it->size *= it->alpha;
            if (it->life <= 0.f) it = particles.erase(it);
            else                 ++it;
        }
    }

    void Emit(const Vector2F& pos, int count) {
        for (int i = 0; i < count; ++i) {
            SpawnParticle(pos);
        }
    }

    void Render(ID2D1DeviceContext7* ctx) {
        ctx->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_ADD);
        for (auto& p : particles) {
            brush->SetColor(D2D1::ColorF(1, 1, 0, p.alpha));
            D2D1_ELLIPSE e = D2D1::Ellipse(
                D2D1::Point2F(p.pos.x, p.pos.y),
                p.size, p.size
            );
            ctx->FillEllipse(e, brush);
        }
        ctx->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);
    }
};
