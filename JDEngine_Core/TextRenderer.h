#pragma once
#include "pch.h"
#include "framework.h"
#include "Component.h"
#include "D2DRenderer.h"

namespace JDComponent {

    using RenderLayerInfo = JDGlobal::Base::RenderLayerInfo;
    using SortingLayer = JDGlobal::Base::SortingLayer;

    template<typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    /// @brief 월드 공간에 텍스트를 렌더링하는 컴포넌트입니다.
    /// GameObject의 Transform(위치, 회전, 스케일)을 따릅니다.
    class TextRenderer : public Component {
    public:
        // 생성자: 텍스트, 레이아웃 크기, 렌더링 레이어 정보를 받습니다.
        TextRenderer(
            const std::wstring& text = L"Text",
            const D2D1_SIZE_F& layoutSize = { 100.f, 100.f },
            const RenderLayerInfo& layerInfo = { SortingLayer::None, 0 }
        );

    //private:
    //    TextRenderer() = delete; // 기본 생성자 비활성화

    public:
        // 렌더링 함수: 외부에서 월드 변환 행렬을 받아 텍스트를 그립니다.
        void Render(ID2D1DeviceContext7* context, D2D1_MATRIX_3X2_F worldTransform);

        // --- 프로퍼티 Getter/Setter ---

        void                SetText(const std::wstring& text) { m_text = text; }
        const std::wstring& GetText() const { return m_text; }

        void               SetColor(const D2D1_COLOR_F& color) { m_color = color; }
        D2D1_COLOR_F       GetColor() const { return m_color; }

        void               SetTextFormatName(const std::string& name) { m_textFormatName = name; }
        const std::string& GetTextFormatName() const { return m_textFormatName; }

        void               SetLayoutSize(const D2D1_SIZE_F& newSize) { m_layoutSize = newSize; }
        D2D1_SIZE_F        GetLayoutSize() const { return m_layoutSize; }

        const RenderLayerInfo& GetLayerInfo() const { return m_layerInfo; }
        void                   SetLayerInfo(const RenderLayerInfo& info) { m_layerInfo = info; }

        // D2DRenderer에 캐시된 TextFormat을 가져오는 헬퍼 함수
        IDWriteTextFormat* GetTextFormat() const;

        // Component 가상 함수들 (빈 구현)
        void Update(float dt) override {}
        void OnEvent(const std::string& ev) override {}
        void OnEnable() override {}
        void OnDisable() override {}

    private:
        std::wstring    m_text;
        D2D1_COLOR_F    m_color;
        std::string     m_textFormatName;
        D2D1_SIZE_F     m_layoutSize;      // 텍스트가 그려질 영역의 크기
        RenderLayerInfo m_layerInfo;       // 렌더링 순서 정보
    };
}