#include "pch.h"
#include "framework.h"
#include "ResourceManager.h"


bool ResourceManager::Initialize(ID2D1RenderTarget* renderTarget)
{
    m_renderTarget = renderTarget;

    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE)
    {
        std::wcout << L"[ERROR] CoInitializeEx failed: " << std::hex << hr << std::endl;
        return false;
    }

    hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&m_wicFactory)
    );
    return SUCCEEDED(hr);
}

bool ResourceManager::LoadTexture(const std::string& name, const std::wstring& filepath)
{
    Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
    HRESULT hr = m_wicFactory->CreateDecoderFromFilename(
        filepath.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
    if (FAILED(hr)) return false;

    Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr)) return false;

    Microsoft::WRL::ComPtr<IWICFormatConverter> converter;
    hr = m_wicFactory->CreateFormatConverter(&converter);
    if (FAILED(hr)) return false;

    hr = converter->Initialize(
        frame.Get(), GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom);
    if (FAILED(hr)) return false;

    Microsoft::WRL::ComPtr<ID2D1Bitmap> bitmap;
    hr = m_renderTarget->CreateBitmapFromWicBitmap(converter.Get(), nullptr, &bitmap);
    if (FAILED(hr)) return false;

    m_textures[name] = bitmap;
    return true;
}

ID2D1Bitmap* ResourceManager::GetTexture(const std::string& name) const
{
    auto it = m_textures.find(name);
    if (it != m_textures.end())
    {
        return it->second.Get();
    }
    return nullptr;
}

bool ResourceManager::LoadAnimation(const std::string& name, const std::wstring& jsonPath)
{
    std::ifstream ifs(jsonPath);
    if (!ifs.is_open()) {
        std::cout << "[ERROR] JSON 파일 열기 실패: " << std::string(jsonPath.begin(), jsonPath.end()) << std::endl;
        return false;
    }

    json j;
    ifs >> j;

    AnimationClip clip;

    for (auto& item : j["frames"].items()) {
        //std::string key = item.key();     // ← 키 접근
        auto& val = item.value();         // ← 값 접근

        auto frame = val["frame"];
        AnimationFrame anim;
        anim.srcRect = D2D1::RectF(
            static_cast<float>(frame["x"]),
            static_cast<float>(frame["y"]),
            static_cast<float>(frame["x"]) + static_cast<float>(frame["w"]),
            static_cast<float>(frame["y"]) + static_cast<float>(frame["h"])
        );
        anim.duration = val["duration"].get<float>() / 1000.0f;

        clip.frames.push_back(anim);
        //std::cout << "[DEBUG] clip.frames.size(): " << clip.frames.size() << std::endl;

    }

    m_animations[name] = clip;
    return true;
}

const AnimationClip* ResourceManager::GetAnimation(const std::string& name) const
{
    auto it = m_animations.find(name);
    if (it != m_animations.end()) {
        return &it->second;
    }
    return nullptr;
}


