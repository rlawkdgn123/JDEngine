#pragma once
#include "json.hpp"
//#include "../JDEngine_Common/json.hpp"

constexpr const char* csvFolderPath = "../Resource/Document/"; // 문서(CSV)관련 기본 위치

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
public:

    bool Initialize(ID2D1RenderTarget* renderTarget);
    
    // 텍스처 에셋
    void TextureSetUp();
    bool LoadTexture(const std::string& name, const std::wstring& filepath);
    ID2D1Bitmap* GetTexture(const std::string& name) const;
    const std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID2D1Bitmap>>&
    GetTextures() const { return m_textures; }

    // 애니메이션 에셋
    bool LoadAnimationRender(const std::string& name, const std::wstring& jsonPath);
    const AnimationRenderClip* GetAnimationRender(const std::string& name) const;
    const std::unordered_map<std::string, AnimationRenderClip>&
    GetAnimations() const { return m_AnimationRenders; }

private:
    AssetManager() = default;
    ~AssetManager() = default;
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;
private:
    // 애니메이션
    Microsoft::WRL::ComPtr<IWICImagingFactory> m_wicFactory;
    ID2D1RenderTarget* m_renderTarget = nullptr;

    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID2D1Bitmap>> m_textures;
    std::unordered_map<std::string, AnimationRenderClip> m_AnimationRenders;

};