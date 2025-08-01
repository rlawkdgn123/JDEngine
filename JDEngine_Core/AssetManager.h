#pragma once

#include "json.hpp"  // 또는 <nlohmann/json.hpp> - 설치 방식에 따라 다름
using json = nlohmann::json;

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
    bool LoadTexture(const std::string& name, const std::wstring& filepath);
    ID2D1Bitmap* GetTexture(const std::string& name) const;

    bool LoadAnimationRender(const std::string& name, const std::wstring& jsonPath);

    const AnimationRenderClip* GetAnimationRender(const std::string& name) const;
    const std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID2D1Bitmap>>& GetTextures() const {
        return m_textures;
    }
private:
    AssetManager() = default;
    ~AssetManager() = default;
private:
    Microsoft::WRL::ComPtr<IWICImagingFactory> m_wicFactory;
    ID2D1RenderTarget* m_renderTarget = nullptr;

    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID2D1Bitmap>> m_textures;
    std::unordered_map<std::string, AnimationRenderClip> m_AnimationRenders;
};