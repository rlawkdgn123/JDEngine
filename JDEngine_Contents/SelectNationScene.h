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

        SelectNationScene(SceneType type, std::string id) : SceneBase(type, id) {}

        ~SelectNationScene() override {}

        void OnEnter() override;

        void OnLeave() override;

        void Update(float deltaTime) override;

        void FixedUpdate(float fixedDeltaTime) override;

        void LateUpdate(float deltaTime) override;

        void Render(float deltaTime) override;

        ////////////////////////////////////////////////////////////////////////////////

        void ClickUpdate();

        // 호버된 캐릭터 클리어
        void ClearHoveredCharacter();

    private:
        FMOD::Channel* m_hoverSfxChannel = nullptr;

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
        CatType m_selectedNation = CatType::CatTypeMAX;

        // 초임 집사, 경험있는 집사 선택 버튼
        Button* m_newCatParentButton = nullptr;
        Button* m_experiencedCatParentButton = nullptr;
    };
}


