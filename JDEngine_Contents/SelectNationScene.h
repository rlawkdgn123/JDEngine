#pragma once
#include "pch.h"
#include "framework.h"
#include "SceneBase.h"

namespace JDScene {

    using CatType = JDGlobal::Contents::CatType;
    using Image = JDGameObject::Content::Image;
    using Text = JDGameObject::Content::Text;
    using Button = JDGameObject::Content::Button;

    class SelectNationScene : public SceneBase
    {
    public:

        ////////////////////////////////////////////////////////////////////////////////

        SelectNationScene(SceneType type, std::string id) : SceneBase(type, id) {}

        ~SelectNationScene() override {}

        void OnEnter() override;

        void OnLeave() override;

        void Update(float deltaTime) override;

        void FixedUpdate(float fixedDeltaTime) override;

        void LateUpdate(float deltaTime) override;

        void Render(float deltaTime) override;

        ////////////////////////////////////////////////////////////////////////////////

        void CreateSelectNationScene();     // 국가 선택 씬 생성
        void FinalizeSelectNationScene();   // 국가 선택 씬 정리

        void InitSound();                   // 사운드 초기화
        void InitParticle();                // 파티클 초기화

        void LogicUpdate();                     // 로직 업데이트
        void ClickUpdate();                     // 클릭 업데이트
        void ParticleUpdate(float deltaTime);   // 파티클 업데이트

        void RenderSelectNationScene(float deltaTime);      // 국가 선택 씬 렌더
        void RenderParticle();                              // 파티클 렌더


        void ClearHoveredCharacter();   // 호버된 캐릭터 클리어

        ////////////////////////////////////////////////////////////////////////////////

    private:

        ////////////////////////////////////////////////////////////////////////////////
        
        void UpdatePage();
        void CloseOpeningCut();

        bool isOpening = true;
        int pageCount = 0;
        Image* openingCut = nullptr;
        Button* prevButton = nullptr;
        Button* nextButton = nullptr;
        Button* skipButton = nullptr;

        ////////////////////////////////////////////////////////////////////////////////
        //FMOD::Channel* bgmChannel = nullptr;
        FMOD::Channel* sfxChannel = nullptr;
        // 나비, 펠리스, 코네
        bool m_hoveredCharacter[static_cast<int>(CatType::CatTypeMAX)] = { false, false, false };

        // 국가 설명 이미지, 텍스트
        Image* m_nationDescImage = nullptr;
        Text* m_nationDescText = nullptr;
        Text* m_nationEffectText = nullptr;

        // 선택된 국가 타이틀 텍스트를 담은 이미지
        Image* m_titleTextImage = nullptr;
        
        Button* m_naviImageButton = nullptr;        // 나비 이미지 버튼
        Button* m_felisImageButton = nullptr;       // 펠리스 이미지 버튼
        Button* m_koneImageButton = nullptr;        // 코네 이미지 버튼

        // 클릭한 고양이 국가 ( 기본값 )
        CatType m_selectedNation = CatType::None;

        // 초임 집사, 경험있는 집사 선택 버튼
        // Button* m_newCatParentButton = nullptr;      // (튜토리얼 로직 변경에 따라서 사용하지 않게되었음.)
        Button* m_experiencedCatParentButton = nullptr;
        //마우스커서
        void RenderCursor(Vector2F mouseClientPos,
            float scale = 1.0f, float alpha = 1.0f);

        Vector2F mouseClientPos;
        D2D1_SIZE_U  g_cursorPx = {};
        Vector2F g_hotspot = { 4.0f, 4.0f };
        ID2D1Bitmap* g_cursorBmp = AssetManager::Instance().GetTexture("mouse");
        ID2D1Bitmap* g_cursorDownBmp = AssetManager::Instance().GetTexture("click");
    };
}


