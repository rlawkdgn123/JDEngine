#pragma once
#include "pch.h"
#include "framework.h"
#include "TextRenderer.h"
#include "D2DRenderer.h"
#include "GameObjectBase.h"

namespace JDComponent {

    TextRenderer::TextRenderer(
        const std::wstring& text,
        const D2D1_SIZE_F& layoutSize,
        const RenderLayerInfo& layerInfo)
        : m_text(text)
        , m_layoutSize(layoutSize)
        , m_layerInfo(layerInfo)
        , m_color(D2D1::ColorF(D2D1::ColorF::White)) // 기본 색상: 흰색
        , m_textFormatName("MalgunGothic_16")         // 기본 포맷
    {
    }

    void TextRenderer::Render(ID2D1DeviceContext7* context, D2D1_MATRIX_3X2_F worldTransform)
    {
        // 텍스트가 비어있거나, 소유자(GameObject)가 비활성화 상태이면 그리지 않음
        if (m_text.empty() || !m_Owner || !m_Owner->IsActive()) {
            return;
        }

        // 텍스트 포맷 가져오기
        IDWriteTextFormat* textFormat = GetTextFormat();
        if (!textFormat) {
            // 포맷이 없으면 렌더링 불가
            return;
        }

        // 1. 월드 변환 행렬 설정
        // GameObject의 Transform 컴포넌트로부터 계산된 최종 행렬을 적용합니다.
        context->SetTransform(worldTransform);

        // 2. 로컬 레이아웃 사각형 정의
        // TextureRenderer와 동일하게, 중심 피벗을 기준으로 사각형을 설정합니다.
        // Y축 방향은 TextureRenderer의 규칙을 따릅니다 (Top > Bottom).
        D2D1_RECT_F layoutRect = D2D1::RectF(
            -m_layoutSize.width * 0.5f,  // left
            m_layoutSize.height * 0.5f, // top
            m_layoutSize.width * 0.5f,  // right
            -m_layoutSize.height * 0.5f  // bottom
        );

        // 3. 텍스트를 그리기 위한 브러시 생성
        ComPtr<ID2D1SolidColorBrush> brush;
        HRESULT hr = context->CreateSolidColorBrush(m_color, &brush);

        // 4. 텍스트 그리기
        if (SUCCEEDED(hr)) {
            context->DrawTextW(
                m_text.c_str(),
                static_cast<UINT32>(m_text.length()),
                textFormat,
                layoutRect, // 위에서 정의한 로컬 사각형
                brush.Get()
            );
        }
    }

    IDWriteTextFormat* TextRenderer::GetTextFormat() const
    {
        // D2DRenderer의 싱글톤 인스턴스에서 포맷 맵을 가져옵니다.
        auto& formats = D2DRenderer::Instance().GetTextFormats();
        auto it = formats.find(m_textFormatName);

        if (it != formats.end()) {
            return it->second.Get(); // 맵에 있으면 포맷 반환
        }

        // 맵에 없으면 nullptr 반환
        return nullptr;
    }
}