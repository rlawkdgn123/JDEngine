#pragma once
#include "pch.h"
#include "framework.h"
#include "SceneBase.h"
#include "ParticleSystem.h"

namespace JDScene {

    using Image = JDGameObject::Content::Image;
    using Text = JDGameObject::Content::Text;
    using Button = JDGameObject::Content::Button;
    using Slider = JDGameObject::Content::Slider;

	class TitleScene : public SceneBase
	{
    public:

        ////////////////////////////////////////////////////////////////////////////////

        TitleScene(SceneType type, std::string id) : SceneBase(type, id) {}

        ~TitleScene() override {}

        void OnEnter() override;

        void OnLeave() override;

        void Update(float deltaTime) override;

        void FixedUpdate(float fixedDeltaTime) override;

        void LateUpdate(float deltaTime) override;

        void Render(float deltaTime) override;

        ////////////////////////////////////////////////////////////////////////////////

        void CreateTitleScene();                // 타이틀 씬 생성 ( 게임오브젝트 & UI )
        void FinalizeTitleScene();              // 타이틀 씬 정리

        void InitSound();                       // 사운드 초기화
        void InitParticle();                    // 파티클 초기화

        void ClickUpdate();                     // 클릭 업데이트
        void ParticleUpdate(float deltaTime);   // 파티클 업데이트

        void RenderTitleScene(float deltaTime); // 타이틀 씬 렌더
        void RenderParticle();                  // 파티클 렌더

        ////////////////////////////////////////////////////////////////////////////////

    private:
        ////////////////////////////////////////////////////////////////////////////////
        // 파티클

        //FMOD::Channel* bgmChannel = nullptr;
        FMOD::Channel* sfxChannel = nullptr;

        std::unique_ptr<ParticleSystem> m_mouseParticles;
        std::unique_ptr<ParticleSystem> m_sakuraParticles;
        std::unique_ptr<ParticleSystem> m_dustParticles;
        std::unique_ptr<ParticleSystem> m_dust2Particles;
        std::unique_ptr<ParticleSystem> m_sparkleParticles;
        Vector2F                        m_emitterPos;

        Vector2F sakuraMin{ 900.0f, 0.0f };
        Vector2F sakuraMax{ 1920.0f, 500.0f };

        float m_sakuraTimer = 0.f;
        const float kSakuraIntervalMin = 0.5f;  // 최소 0.5초
        const float kSakuraIntervalMax = 1.5f;  // 최대 1.5초

        float m_sakuraEmissionRate = 1.5f;//초당 생성 갯수
        float m_sakuraEmitAccumulator = 0.0f;

        void RenderCursor(Vector2F mouseClientPos,
            float scale = 1.0f, float alpha = 1.0f);

        Vector2F mouseClientPos;
        D2D1_SIZE_U  g_cursorPx = {};
        Vector2F g_hotspot = { 0.0f, 0.0f };
        ID2D1Bitmap* g_cursorBmp = AssetManager::Instance().GetTexture("mouse");
        ID2D1Bitmap* g_cursorDownBmp = AssetManager::Instance().GetTexture("click");
        ////////////////////////////////////////////////////////////////////////////////
        // 옵션창

        bool isOpenOption = false;

        Image* m_optionUI = nullptr;
        Image* m_optionVolume = nullptr;
        Image* m_optionControl = nullptr;
        Image* m_optionCredit = nullptr;

        // 옵션 닫기 버튼
        Button* m_closeOption = nullptr;

        // 옵션 선택 실제 버튼
        Button* m_selectVolume = nullptr;
        Button* m_selectControl = nullptr;
        Button* m_selectCredit = nullptr;

        // 옵션 선택 더미 텍스트
        Text* m_selectVolumeDummyText = nullptr;
        Text* m_selectControlDummyText = nullptr;
        Text* m_selectCreditDummyText = nullptr;

        // 볼륨 선택 슬라이더
        Slider* m_masterSlider = nullptr;
        Slider* m_bgmSlider = nullptr;
        Slider* m_sfxSlider = nullptr;

        // 배속 키 텍스트
        Text* m_stopKeyText = nullptr;
        Text* m_playKeyText = nullptr;
        Text* m_speedKeyText = nullptr;
        
        ////////////////////////////////////////////////////////////////////////////////
	};
}



