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
        , m_textFormatName("MalgunGothic_16")       // 기본 포맷
    {
    }

    void TextRenderer::Render(ID2D1DeviceContext7* context, D2D1_MATRIX_3X2_F worldTransform)
    {
        // SetActive가 false면 리턴
        if (!m_Owner || !m_Owner->IsActive()) return;

        // 1. 월드 변환 행렬 설정
        context->SetTransform(worldTransform);

        // 2. 로컬 레이아웃 사각형 정의
        D2D1_RECT_F layoutRect = D2D1::RectF(
            -m_layoutSize.width * 0.5f,
            m_layoutSize.height * 0.5f,
            m_layoutSize.width * 0.5f,
            -m_layoutSize.height * 0.5f
        );

        // 3. 텍스트 포맷 가져오기 시도
        IDWriteTextFormat* textFormat = GetTextFormat();

        // 4. 텍스트를 그리기 위한 브러시 생성
        ComPtr<ID2D1SolidColorBrush> brush;
        HRESULT hr = context->CreateSolidColorBrush(m_color, &brush);

        // 5. 텍스트 그리기
        if (SUCCEEDED(hr)) {
            context->DrawTextW(
                m_text.c_str(),
                static_cast<UINT32>(m_text.length()),
                textFormat,
                layoutRect,
                brush.Get()
            );
        }
    }

    IDWriteTextFormat* TextRenderer::GetTextFormat() const
    {
        auto& formats = D2DRenderer::Instance().GetTextFormats();
        auto it = formats.find(m_textFormatName);

        if (it != formats.end()) {
            return it->second.Get();
        }

        // 폰트 로딩 실패를 확인하기 위해 콘솔에 로그를 남길 수 있습니다.
        // std::cout << "Failed to find font format: " << m_textFormatName << std::endl;

        return nullptr;
    }
}
