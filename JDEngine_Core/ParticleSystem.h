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
    float     life;
    float     size;
    float     alpha;
    D2D1::ColorF color;

    float imageScale;
    float rotation;
    float rotationSpeed;

    Particle()
        : pos{ 0,0 }, vel{ 0,0 }, life{ 0 }, size{ 0 }, alpha{ 0 },
        color{ 0,0,0,0 }, imageScale{ 1.0f },
        rotation{ 0 }, rotationSpeed{ 0 }
    {
    }
};

static std::mt19937_64 rng{ std::random_device{}() };

static float RandomFloat(float lo, float hi) {
    std::uniform_real_distribution<float> dist(lo, hi);
    return dist(rng);
}

class ParticleSystem {
public:
    std::vector<Particle> particles;
    float emissionRate = 20;
    float emitAccumulator = 0;
    ID2D1SolidColorBrush* brush = nullptr;

    ID2D1Bitmap* particleBitmap = nullptr;
    ID2D1Bitmap* dustBitmap = nullptr;
    ID2D1Bitmap* dust2Bitmap = nullptr;
    ID2D1Bitmap* MouseBitmap = nullptr;
    ID2D1Bitmap* sparkleBitmap = nullptr;
    ID2D1Bitmap* leafBitmap = nullptr;
    ID2D1Bitmap* footdustBitmap = nullptr;
    ID2D1Bitmap* smokeBitmap = nullptr;
    ID2D1Bitmap* waveBitmap = nullptr;

    D2D1::ColorF defaultColor = D2D1::ColorF(1, 1, 1, 1);

    ParticleSystem(ID2D1DeviceContext7* ctx) {
        ctx->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 1), &brush);
        particleBitmap = AssetManager::Instance().GetTexture("blossom");
        dustBitmap = AssetManager::Instance().GetTexture("dust");
        dust2Bitmap = AssetManager::Instance().GetTexture("dust2");
        MouseBitmap = AssetManager::Instance().GetTexture("mouse");
        sparkleBitmap = AssetManager::Instance().GetTexture("spakle");
        leafBitmap = AssetManager::Instance().GetTexture("dust");//leaf
        footdustBitmap = AssetManager::Instance().GetTexture("dust");//footdust
        smokeBitmap = AssetManager::Instance().GetTexture("dust");//smoke
        waveBitmap = AssetManager::Instance().GetTexture("dust");//wave
    }

    void SpawnParticle(const Vector2F& pos, D2D1::ColorF color, float scale = 1.0f) {
        Particle p;
        p.pos = pos;
        float angle = RandomFloat(0, 2 * 3.14159265f);
        float speed = RandomFloat(20, 80);
        p.vel = { std::cos(angle) * speed, std::sin(angle) * speed };
        p.life = RandomFloat(0.5f, 1);
        p.size = RandomFloat(10, 12);
        p.alpha = 1;
        p.color = color;
        p.imageScale = scale;
        particles.push_back(p);
    }

    void SpawnFallingParticle(const Vector2F& pos, D2D1::ColorF color, float scale = 1.0f, float maxLife = 4.0f) {
        Particle p;
        p.pos = pos;
        p.vel = { RandomFloat(-80, -20), RandomFloat(50, 80) };
        p.life = maxLife;
        p.size = RandomFloat(10, 12);
        p.alpha = 1;
        p.color = color;
        p.imageScale = scale;
        p.rotation = RandomFloat(0, 360);
        p.rotationSpeed = RandomFloat(-30, 30);
        particles.push_back(p);
    }

    void SpawnDustParticle(const Vector2F& pos, D2D1::ColorF color, float scale = 5.0f, float maxLife = 2.0f, float spreadRadius = 20.0f) {
        Particle p;
        p.pos = pos;
        float angle = RandomFloat(0, 2 * 3.14159265f);
        float radius = RandomFloat(0, spreadRadius);
        p.pos.x += std::cos(angle) * radius;
        p.pos.y += std::sin(angle) * radius;
        p.vel = { RandomFloat(-50, 50), RandomFloat(-15, 15) };
        p.life = maxLife;
        p.size = RandomFloat(5, 7);
        p.imageScale = scale * 0.7f;
        p.alpha = 1;
        p.color = color;
        p.rotation = RandomFloat(0, 360);
        p.rotationSpeed = RandomFloat(-10, 10);
        particles.push_back(p);
    }

    void SpawnSparkleParticle(const Vector2F& center, int count = 5, float scale = 5.0f, float radius = 16.f, float maxLife = 0.5f)
    {
        for (int i = 0; i < count; ++i) {
            Particle p;
            // 1) ��ġ: ������ ���� �� ����
            float angle = RandomFloat(0, 2 * 3.14159265f);
            float dist = RandomFloat(0, radius);
            p.pos = { center.x + std::cos(angle) * dist,
                             center.y + std::sin(angle) * dist };
            // 2) �ӵ�: ���� ���ų� ���� ª�� ������
            p.vel = { RandomFloat(-10, 10), RandomFloat(-10, 10) };
            // 3) ����ũ�⡤����
            p.life = maxLife;
            p.size = RandomFloat(4, 8);
            p.imageScale = scale;
            p.alpha = 1.0f;
            // 4) �÷�: ����� ����Ŭ
            p.color = D2D1::ColorF(1.0f, 0.9f, 0.6f, 1.0f);
            // 5) ȸ�� ��� ��
            p.rotation = 0;
            p.rotationSpeed = 0;
            particles.push_back(p);
        }
    }

    void SpawnLeavesParticle(const Vector2F& pos, int count = 6, float scale = 1.0f, float maxLife = 4.0f)
    {
        for (int i = 0; i < count; ++i) {
            Particle p;
            p.pos = pos + Vector2F{ RandomFloat(-20.f, 20.f), RandomFloat(-10.f, 10.f) };
            // õõ�� �Ʒ��� + �ణ�� ��/�� �ӵ�
            p.vel = { RandomFloat(-30.f, 30.f), RandomFloat(30.f, 60.f) };
            p.life = maxLife;                 // UpdateLeaves���� �� �� �������� ���� ���
            p.size = RandomFloat(16.f, 24.f); // ������ ���� ũ��(px)
            p.alpha = 1.f;
            p.imageScale = scale;
            p.rotation = RandomFloat(0.f, 360.f);
            p.rotationSpeed = RandomFloat(-50.f, 50.f); // ȸ���ϸ� ��������
            particles.push_back(p);
        }
    }

    void SpawnSmokeParticle(const Vector2F& pos, int count = 6, float baseScale = 2.0f, float maxLife = 2.5f)
    {
        for (int i = 0; i < count; ++i) {
            Particle p;
            p.pos = pos + Vector2F{ RandomFloat(-6.f, 6.f), RandomFloat(-4.f, 4.f) };
            p.vel = { RandomFloat(-10.f, 10.f), RandomFloat(-30.f, -10.f) }; // ���� ��¦
            p.life = maxLife;
            p.size = RandomFloat(22.f, 30.f);
            p.alpha = 1.f;
            p.imageScale = RandomFloat(baseScale * 0.9f, baseScale * 1.1f);
            p.rotation = RandomFloat(0.f, 360.f);
            p.rotationSpeed = RandomFloat(-15.f, 15.f);
            particles.push_back(p);
        }
    }

    void SpawnFootDustParticle(const Vector2F& pos, int count = 8, float scale = 3.5f, float maxLife = 0.6f)
    {
        for (int i = 0; i < count; ++i) {
            Particle p;
            // �� ��ġ�� �߽����� ��¦ ���� ����
            p.pos = pos + Vector2F{ RandomFloat(-6.f, 6.f), RandomFloat(-3.f, 3.f) };
            // ������ ������ �ӵ� + ���� ���� ����(���鿡�� �Ǿ������)
            p.vel = { RandomFloat(-60.f, 60.f), RandomFloat(-10.f, 15.f) };
            p.life = maxLife;
            p.size = RandomFloat(10.f, 14.f);
            p.alpha = 1.f;
            p.imageScale = scale;
            p.rotation = RandomFloat(0.f, 360.f);
            p.rotationSpeed = RandomFloat(-20.f, 20.f);
            particles.push_back(p);
        }
    }

    void SpawnWaterWaveParticle(const Vector2F& pos, float startScale = 0.3f, float expandSpeed = 1.6f, float maxLife = 0.8f, float baseSizePx = 48.f)
    {
        Particle p;
        p.pos = pos;
        p.vel = { 0.f, 0.f };
        p.life = maxLife;
        p.size = baseSizePx;     // ���� ���� ũ��(px) ? ��Ʈ�� ������ Ȯ��
        p.alpha = 1.f;
        p.imageScale = startScale;
        p.rotation = 0.f;
        p.rotationSpeed = expandSpeed; //  Ȯ�� �ӵ�
        particles.push_back(p);
    }

    void Update(float dt, const Vector2F& emitterPos) {
        emitAccumulator += dt * emissionRate;
        int count = static_cast<int>(emitAccumulator);
        emitAccumulator -= count;

        for (int i = 0; i < count; ++i)
            SpawnParticle(emitterPos, defaultColor);

        for (auto it = particles.begin(); it != particles.end();) {
            it->life -= dt;
            it->pos += it->vel * dt;
            it->alpha = (it->life > 0) ? it->life : 0;
            it->size *= it->alpha;
            it = (it->life <= 0) ? particles.erase(it) : ++it;
        }
    }

    void UpdateFalling(float dt, float screenW, float screenH) {
        for (auto it = particles.begin(); it != particles.end();) {
            // 1) ���� ����
            it->life -= dt;

            // 2) ��ġ �� ȸ�� ������Ʈ
            it->pos += it->vel * dt;
            it->rotation += it->rotationSpeed * dt;

            // 3) life ��� ���� ��� (maxLife = 4�� ����)
            it->alpha = (it->life > 0.f) ? (it->life / 4.0f) : 0.f;

            // 4) ȭ�� �Ʒ��� ����ų� life ���� �� ����
            bool expired = (it->life <= 0.f) || (it->pos.y > screenH + 50.f);
            if (expired) {
                it = particles.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    void UpdateDust(float dt, float screenW, float screenH) {
        for (auto it = particles.begin(); it != particles.end();) {
            it->life -= dt;
            it->pos += it->vel * dt;
            it->rotation += it->rotationSpeed * dt;
            it->imageScale += dt * 1.0f;
            it->alpha = (it->life > 0) ? (it->life / 2.0f) : 0;
            bool offscreen = it->pos.x < -50 || it->pos.x > screenW + 50 || it->pos.y < -50 || it->pos.y > screenH + 50;
            if (it->life <= 0 || offscreen)
                it = particles.erase(it);
            else
                ++it;
        }
    }

    void UpdateSparkle(float dt) {
        for (auto it = particles.begin(); it != particles.end();) {
            it->life -= dt;
            it->pos += it->vel * dt;
            // ���Ĵ� life ������
            it->alpha = (it->life > 0) ? (it->life / 0.5f) : 0;
            if (it->life <= 0)
                it = particles.erase(it);
            else
                ++it;
        }
    }

    void UpdateLeaves(float dt, float screenW, float screenH)
    {
        const float MAX_LIFE = 4.0f; // SpawnLeaves���� �� �⺻���� ���� ���
        for (auto it = particles.begin(); it != particles.end();) {
            it->life -= dt;

            // �߷����� ���ݾ� �� ��������
            it->vel.y += 25.f * dt;

            // �¿� ��鸲(ȸ������ �������� ��Ȱ��)
            float sway = (std::sin)(it->rotation * 3.14159265f / 180.f) * 30.f; // px/s
            it->pos.x += (it->vel.x + sway) * dt;
            it->pos.y += it->vel.y * dt;

            // ȸ��
            it->rotation += it->rotationSpeed * dt;

            // ���̵�ƿ�(���� ����)
            float t = (std::max)(0.f, it->life / MAX_LIFE);
            it->alpha = t;

            // ȭ�� �Ʒ��� ������ų� ���� ���� �� ����
            bool expired = (it->life <= 0.f) || (it->pos.y > screenH + 50.f);
            if (expired) it = particles.erase(it);
            else ++it;
        }
    }

    void UpdateSmoke(float dt, float screenW, float screenH)
    {
        const float MAX_LIFE = 2.5f;
        for (auto it = particles.begin(); it != particles.end();) {
            it->life -= dt;

            // �η�: ���� �� �ε巴��
            it->vel.y -= 8.f * dt;
            // �¿� �帮��Ʈ
            it->vel.x += RandomFloat(-5.f, 5.f) * dt;

            it->pos += it->vel * dt;
            it->rotation += it->rotationSpeed * dt;

            // ����� ������ Ŀ����
            it->imageScale += dt * 0.6f;

            // ���Ĵ� ���� ����� ����
            float t = (std::max)(0.f, it->life / MAX_LIFE);
            it->alpha = t * 0.9f; // ��¦ �� ���ϰ�

            bool off =  it->life <= 0.f;
            if (off) it = particles.erase(it);
            else ++it;
        }
    }

    void UpdateFootDust(float dt, float screenW, float screenH)
    {
        const float MAX_LIFE = 0.6f;
        for (auto it = particles.begin(); it != particles.end();) {
            it->life -= dt;

            // ������ ������ ����
            it->vel.x *= (1.f - 6.f * dt); // ���� ���� ���ϰ�
            it->vel.y *= (1.f - 3.f * dt);

            it->pos += it->vel * dt;
            it->rotation += it->rotationSpeed * dt;

            // �ް��� �����ٰ� �����
            it->imageScale += dt * 1.2f;

            // ���̵� �ƿ�
            float t = (std::max)(0.f, it->life / MAX_LIFE);
            it->alpha = t;

            bool off = it->life <= 0.f || it->pos.x < -50 || it->pos.x > screenW + 50 || it->pos.y < -50 || it->pos.y > screenH + 50;
            if (off) it = particles.erase(it);
            else ++it;
        }
    }

    void UpdateWaterWave(float dt)
    {
        const float MAX_LIFE = 0.8f;
        for (auto it = particles.begin(); it != particles.end();) {
            it->life -= dt;

            // Ȯ��
            it->imageScale += it->rotationSpeed * dt;

            // ���̵�ƿ� (������ ������ ������ �������)
            float t = (std::max)(0.f, it->life / MAX_LIFE);
            it->alpha = t * t;

            if (it->life <= 0.f) it = particles.erase(it);
            else ++it;
        }
    }

    void Render(ID2D1DeviceContext7* ctx) {
        ctx->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);
        D2D1_MATRIX_3X2_F oldTM; ctx->GetTransform(&oldTM);

        for (auto& p : particles) {
            if (!particleBitmap) continue;
            auto rot = D2D1::Matrix3x2F::Rotation(p.rotation, { p.pos.x, p.pos.y });
            ctx->SetTransform(rot * oldTM);
            float half = p.size * p.imageScale * 0.5f;
            D2D1_RECT_F dest = { p.pos.x - half, p.pos.y - half, p.pos.x + half, p.pos.y + half };
            ctx->DrawBitmap(particleBitmap, dest, p.alpha, D2D1_INTERPOLATION_MODE_LINEAR,
                D2D1_RECT_F{ 0,0,(float)particleBitmap->GetPixelSize().width,(float)particleBitmap->GetPixelSize().height });
            ctx->SetTransform(oldTM);
        }
        ctx->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);
    }

    void RenderDust(ID2D1DeviceContext7* ctx) {
        if (!dustBitmap) return;
        ctx->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);
        D2D1_MATRIX_3X2_F oldTM; ctx->GetTransform(&oldTM);

        for (auto& p : particles) {
            float baseAlpha = p.alpha;
            const int layers = 4;
            for (int i = 0; i < layers; ++i) {
                float factor = 1.0f + i * 0.3f;
                float half = p.size * p.imageScale * factor * 0.5f;
                float spread = p.size * 0.2f * factor;
                float dx = RandomFloat(-spread, spread);
                float dy = RandomFloat(-spread, spread);
                float layerAlpha = baseAlpha * (1.0f - float(i) / layers);

                auto rot = D2D1::Matrix3x2F::Rotation(p.rotation, { p.pos.x, p.pos.y });
                ctx->SetTransform(rot * oldTM);
                D2D1_RECT_F dest = { p.pos.x - half + dx, p.pos.y - half + dy, p.pos.x + half + dx, p.pos.y + half + dy };
                ctx->DrawBitmap(dustBitmap, dest, layerAlpha, D2D1_INTERPOLATION_MODE_LINEAR,
                    D2D1_RECT_F{ 0,0,(float)dustBitmap->GetPixelSize().width,(float)dustBitmap->GetPixelSize().height });
            }
            ctx->SetTransform(oldTM);
        }
        ctx->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);
    }

    void RenderDust2(ID2D1DeviceContext7* ctx) {
        if (!dust2Bitmap) return;
        ctx->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);
        D2D1_MATRIX_3X2_F oldTM; ctx->GetTransform(&oldTM);

        for (auto& p : particles) {
            if (RandomFloat(0, 1) > 0.3f) continue;
            float baseAlpha = p.alpha * 0.6f;
            const int layers = 3;
            for (int i = 0; i < layers; ++i) {
                float factor = 1.0f + i * 0.4f;
                float half = p.size * p.imageScale * factor * 0.5f;
                float spread = p.size * 0.1f * factor;
                float dx = RandomFloat(-spread, spread);
                float dy = RandomFloat(-spread, spread);
                float layerAlpha = baseAlpha * (1.0f - float(i) / layers);

                auto rot = D2D1::Matrix3x2F::Rotation(p.rotation, { p.pos.x, p.pos.y });
                ctx->SetTransform(rot * oldTM);
                D2D1_RECT_F dest = { p.pos.x - half + dx, p.pos.y - half + dy, p.pos.x + half + dx, p.pos.y + half + dy };
                ctx->DrawBitmap(dust2Bitmap, dest, layerAlpha, D2D1_INTERPOLATION_MODE_LINEAR,
                    D2D1_RECT_F{ 0,0,(float)dust2Bitmap->GetPixelSize().width,(float)dust2Bitmap->GetPixelSize().height });
            }
            ctx->SetTransform(oldTM);
        }
        ctx->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);
    }

    void RenderMouse(ID2D1DeviceContext7* ctx) {
        if (!MouseBitmap) return;
        ctx->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);
        D2D1_MATRIX_3X2_F oldTM; ctx->GetTransform(&oldTM);

        for (auto& p : particles) {
            auto rot = D2D1::Matrix3x2F::Rotation(p.rotation, { p.pos.x, p.pos.y });
            ctx->SetTransform(rot * oldTM);
            float half = p.size * p.imageScale * 0.5f;
            D2D1_RECT_F dest = { p.pos.x - half, p.pos.y - half, p.pos.x + half, p.pos.y + half };
            ctx->DrawBitmap(MouseBitmap, dest, p.alpha, D2D1_INTERPOLATION_MODE_LINEAR,
                D2D1_RECT_F{ 0,0,(float)MouseBitmap->GetPixelSize().width,(float)MouseBitmap->GetPixelSize().height });
            ctx->SetTransform(oldTM);
        }
        ctx->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);
    }

    void RenderGlow(ID2D1DeviceContext7* ctx) {
        if (!MouseBitmap) return;
        ctx->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_ADD);
        D2D1_MATRIX_3X2_F oldTM; ctx->GetTransform(&oldTM);
        ctx->SetTransform(D2D1::Matrix3x2F::Identity());

        for (auto& p : particles) {
            auto rot = D2D1::Matrix3x2F::Rotation(p.rotation, { p.pos.x, p.pos.y });
            ctx->SetTransform(rot * oldTM);
            for (float factor : {1.0f, 1.5f, 2.0f}) {
                float half = p.size * p.imageScale * factor * 0.5f;
                D2D1_RECT_F dest = { p.pos.x - half, p.pos.y - half, p.pos.x + half, p.pos.y + half };
                float alpha = p.alpha * (1.0f - (factor - 1.0f));
                ctx->DrawBitmap(MouseBitmap, dest, alpha, D2D1_INTERPOLATION_MODE_LINEAR,
                    D2D1_RECT_F{ 0,0,(float)MouseBitmap->GetPixelSize().width,(float)MouseBitmap->GetPixelSize().height });
            }
            ctx->SetTransform(oldTM);
        }
        ctx->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);
    }

    void RenderSparkle(ID2D1DeviceContext7* ctx) {
        if (!sparkleBitmap) return;
        ctx->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_ADD);
        D2D1_MATRIX_3X2_F oldTM; ctx->GetTransform(&oldTM);
        for (auto& p : particles) {
            float half = p.size * p.imageScale * 0.5f;
            D2D1_RECT_F dest = {
                p.pos.x - half, p.pos.y - half,
                p.pos.x + half, p.pos.y + half
            };
            ctx->DrawBitmap(
                sparkleBitmap,
                dest,
                p.alpha,
                D2D1_INTERPOLATION_MODE_LINEAR
            );
        }

        ctx->SetTransform(oldTM);
        ctx->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);
    }

    void RenderLeaves(ID2D1DeviceContext7* ctx)
    {
        if (!leafBitmap) return;
        ctx->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);
        D2D1_MATRIX_3X2_F oldTM; ctx->GetTransform(&oldTM);

        for (auto& p : particles) {
            auto rot = D2D1::Matrix3x2F::Rotation(p.rotation, { p.pos.x, p.pos.y });
            ctx->SetTransform(rot * oldTM);

            float half = p.size * p.imageScale * 0.5f;
            D2D1_RECT_F dest = { p.pos.x - half, p.pos.y - half, p.pos.x + half, p.pos.y + half };

            ctx->DrawBitmap(leafBitmap, dest, p.alpha, D2D1_INTERPOLATION_MODE_LINEAR,
                D2D1_RECT_F{ 0,0,(float)leafBitmap->GetPixelSize().width,(float)leafBitmap->GetPixelSize().height });

            ctx->SetTransform(oldTM);
        }
        ctx->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);
    }

    void RenderSmoke(ID2D1DeviceContext7* ctx)
    {
        if (!smokeBitmap) return;
        ctx->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);
        D2D1_MATRIX_3X2_F oldTM; ctx->GetTransform(&oldTM);

        for (auto& p : particles) {
            auto rot = D2D1::Matrix3x2F::Rotation(p.rotation, { p.pos.x, p.pos.y });
            ctx->SetTransform(rot * oldTM);

            float half = p.size * p.imageScale * 0.5f;
            D2D1_RECT_F dest = { p.pos.x - half, p.pos.y - half, p.pos.x + half, p.pos.y + half };

            ctx->DrawBitmap(smokeBitmap, dest, p.alpha, D2D1_INTERPOLATION_MODE_LINEAR,
                D2D1_RECT_F{ 0,0,(float)smokeBitmap->GetPixelSize().width,(float)smokeBitmap->GetPixelSize().height });

            ctx->SetTransform(oldTM);
        }
        ctx->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);
    }

    void RenderFootDust(ID2D1DeviceContext7* ctx)
    {
        if (!footdustBitmap) return;
        ctx->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);
        D2D1_MATRIX_3X2_F oldTM; ctx->GetTransform(&oldTM);

        for (auto& p : particles) {
            auto rot = D2D1::Matrix3x2F::Rotation(p.rotation, { p.pos.x, p.pos.y });
            ctx->SetTransform(rot * oldTM);

            float half = p.size * p.imageScale * 0.5f;
            D2D1_RECT_F dest = { p.pos.x - half, p.pos.y - half, p.pos.x + half, p.pos.y + half };

            ctx->DrawBitmap(footdustBitmap, dest, p.alpha, D2D1_INTERPOLATION_MODE_LINEAR,
                D2D1_RECT_F{ 0,0,(float)footdustBitmap->GetPixelSize().width,(float)footdustBitmap->GetPixelSize().height });

            ctx->SetTransform(oldTM);
        }
        ctx->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);
    }

    void RenderWaterWave(ID2D1DeviceContext7* ctx)
    {
        if (!waveBitmap) return;
        // ���ĵ��� ��¦ �߱� ������ ��︲ ? ���ϸ� SOURCE_OVER�� �ٲ㵵 OK
        ctx->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_ADD);
        D2D1_MATRIX_3X2_F oldTM; ctx->GetTransform(&oldTM);

        for (auto& p : particles) {
            // ȸ���� ����
            ctx->SetTransform(oldTM);

            float half = p.size * p.imageScale * 0.5f;
            D2D1_RECT_F dest = { p.pos.x - half, p.pos.y - half, p.pos.x + half, p.pos.y + half };

            ctx->DrawBitmap(waveBitmap, dest, p.alpha, D2D1_INTERPOLATION_MODE_LINEAR,
                D2D1_RECT_F{ 0,0,(float)waveBitmap->GetPixelSize().width,(float)waveBitmap->GetPixelSize().height });
        }

        ctx->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);
    }
    void Emit(const Vector2F& pos, int count, D2D1::ColorF color, float scale = 1.0f) {
        for (int i = 0; i < count; ++i)
            SpawnParticle(pos, color, scale);
    }

    void SetParticleLife(float life) {
        Particle p; p.life = life;
    }
};
