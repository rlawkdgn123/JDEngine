#pragma once
//????????????????????????????????
#include <unordered_map>

#include "json.hpp"  // 또는 <nlohmann/json.hpp> - 설치 방식에 따라 다름
using json = nlohmann::json;

struct AnimationFrame {
    D2D1_RECT_F srcRect;
    float duration;
};

struct AnimationClip {
    std::vector<AnimationFrame> frames;
    bool loop = true;
};

class ResourceManager
{
public:
    bool Initialize(ID2D1RenderTarget* renderTarget);
    bool LoadTexture(const std::string& name, const std::wstring& filepath);
    ID2D1Bitmap* GetTexture(const std::string& name) const;
    bool LoadAnimation(const std::string& name, const std::wstring& jsonPath);
    const AnimationClip* GetAnimation(const std::string& name) const;
private:
    Microsoft::WRL::ComPtr<IWICImagingFactory> m_wicFactory;
    ID2D1RenderTarget* m_renderTarget = nullptr;

    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID2D1Bitmap>> m_textures;

    std::unordered_map<std::string, AnimationClip> m_animations;
};