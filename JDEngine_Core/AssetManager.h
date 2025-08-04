
#pragma once
//#include "json.hpp"                   //�̰͵� �����ϳ� �ٸ� ������ �ֱ� ������ ����η� �ϴ� ���� ������ �� ����
#include "../JDEngine_Common/json.hpp"  // �Ǵ� <nlohmann/json.hpp> - ��ġ ��Ŀ� ���� �ٸ�
using json = nlohmann::json;

constexpr const char* documentPath = "../Resource/Document/"; // 문서(CSV)관련 기본 위치

struct AnimationRenderFrame {
    D2D1_RECT_F srcRect;
    float duration;
};

struct AnimationRenderClip {
    std::vector<AnimationRenderFrame> frames;
    bool loop = true;
};

class AssetManager
{
public:
    static AssetManager& Instance()
    {
        static AssetManager instance;
        return instance;
    }

    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    bool Initialize(ID2D1RenderTarget* renderTarget);
    
    // 애니메이션 Parsing
    bool LoadTexture(const std::string& name, const std::wstring& filePath);
    ID2D1Bitmap* GetTexture(const std::string& name) const;

    bool LoadAnimationRender(const std::string& name, const std::wstring& jsonPath);

    const AnimationRenderClip* GetAnimationRender(const std::string& name) const;
    const std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID2D1Bitmap>>& GetTextures() const {
        return m_textures;
    }

    // CSV Parsing
    bool LoadCSV(const std::string& name, const std::wstring& filePath);
    ID2D1Bitmap* GetCsv(const std::string& name) const;

    void ConvertAllCsvToWstring(); // m_documents 모든 객체 string -> wstring 변환

private:
    AssetManager() = default;
    ~AssetManager() = default;
private:
    // 애니메이션
    Microsoft::WRL::ComPtr<IWICImagingFactory> m_wicFactory;
    ID2D1RenderTarget* m_renderTarget = nullptr;

    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID2D1Bitmap>> m_textures;
    std::unordered_map<std::string, AnimationRenderClip> m_AnimationRenders;

    // CSV
    
    // string은 RAII가 이미 적용되어 있으므로, smart_ptr을 적용시킬 필요가 없다.
    std::unordered_map<std::string, std::string> m_csvRawData; // csv 파일
};