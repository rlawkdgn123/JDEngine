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
    D2D1::ColorF color;  // ��ƼŬ ����

    float imageScale;
    float rotation;
    float rotationSpeed;

    Particle()
        : pos{ 0,0 }
        , vel{ 0,0 }
        , life{ 0 }
        , size{ 0 }
        , alpha{ 0 }
        , color{ 0.f,0.f,0.f,0.f }  // ���ϴ� �ʱⰪ
        , imageScale{ 1.0f }
        , rotation{ 0.f }
        , rotationSpeed{ 0.0f }
    {}
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
    ID2D1SolidColorBrush* brush = nullptr;   // �귯��

    ID2D1Bitmap* particleBitmap = nullptr;
    // �ڵ� ���⿡ ����� �⺻ ����
    D2D1::ColorF defaultColor = D2D1::ColorF(1.f, 1.f, 1.f, 1.f);

    ParticleSystem(ID2D1DeviceContext7* ctx) {
        // �ʱ� �귯��(���, ���� ������)
        ctx->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 1), &brush);

        particleBitmap = AssetManager::Instance().GetTexture("blossom");
    }

    // ����: color �Ķ���� �߰�, �����ݷ� ���� �κ� ����
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
        // �¿� drift: -20~+20 �ȼ�/sec, �Ʒ� �ӵ�: 50~80 �ȼ�/sec
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
    // dt ���� �ڵ� ���� + ���� ��ƼŬ ������Ʈ
    void Update(float dt, const Vector2F& emitterPos) {
        // 1) �ڵ� ���� (defaultColor ���)
        emitAccumulator += dt * emissionRate;
        int count = static_cast<int>(emitAccumulator);
        emitAccumulator -= count;
        for (int i = 0; i < count; ++i) {
            SpawnParticle(emitterPos, defaultColor);
        }

        // 2) ���� ��ƼŬ ������Ʈ
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
            //ȸ��
            it->rotation += it->rotationSpeed * dt;
            // life ������ alpha ����
            it->alpha = (it->life > 0.f) ? (it->life / 4.0f) : 0.f;

            // ȭ�� �Ʒ� + life �� ���� �� ����
            if (it->life <= 0.f || it->pos.y > screenHeight + 50.f)
                it = particles.erase(it);
            else
                ++it;
        }
    }

    // ���� ��ġ���� ���ϴ� ������ �� ���� ����
    void Emit(const Vector2F& pos, int count, D2D1::ColorF color, float scale = 1.0f) {
        for (int i = 0; i < count; ++i) {
            SpawnParticle(pos, color, scale);
        }
    }

    void SetParticleLife(float life) { Particle p; p.life = life; }

    // ������: �� ��ƼŬ�� color�� alpha ����
    void Render(ID2D1DeviceContext7* ctx) {
        //ctx->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_ADD);
        ctx->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);

        D2D1_MATRIX_3X2_F oldTM;
        ctx->GetTransform(&oldTM);

        for (auto& p : particles) {
            if (particleBitmap) {
                //ȸ��
                D2D1_POINT_2F center = { p.pos.x, p.pos.y };
                auto rot = D2D1::Matrix3x2F::Rotation(p.rotation, center);
                ctx->SetTransform(rot * oldTM);
                // �׸� ��ġ�� ũ�� ����
                float half = p.size * p.imageScale * 0.5f;
                D2D1_RECT_F dest = {
                  p.pos.x - half, p.pos.y - half,
                  p.pos.x + half, p.pos.y + half
                };
                // ���ĸ� ����
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
                // �ؽ�ó ������ ���� ���
                brush->SetColor(D2D1::ColorF(p.color.r, p.color.g, p.color.b, p.alpha));
                D2D1_ELLIPSE e = D2D1::Ellipse({ p.pos.x, p.pos.y }, p.size, p.size);
                ctx->FillEllipse(e, brush);
            }

            ctx->SetTransform(oldTM);

        }
        //for (auto& p : particles) {
        //    // p.color�� RGB + p.alpha �̿�
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
