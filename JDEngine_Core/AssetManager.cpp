#include "pch.h"
#include "framework.h"
#include "AssetManager.h"

using json = nlohmann::json;

bool AssetManager::Initialize(ID2D1RenderTarget* renderTarget)
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

void AssetManager::TextureSetUp() {
    if (!AssetManager::Instance().LoadTexture("Test", L"../Resource/Texture/Test.png")) {
        std::cout << "[ERROR] Test 텍스처 로드 실패" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("Title", L"../Resource/TITLE/TITLE.png")) {
        std::cout << "[ERROR] Title 텍스처 로드 실패" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("GAME_START_A", L"../Resource/TITLE/GAME_START_A.png")) {
        std::cout << "[ERROR] GAME_START_A 텍스처 로드 실패" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("GAME_START_B", L"../Resource/TITLE/GAME_START_B.png")) {
        std::cout << "[ERROR] GAMESTART_B 텍스처 로드 실패" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("SETTING_A", L"../Resource/TITLE/SETTING_A.png")) {
        std::cout << "[ERROR] SETTING_A 텍스처 로드 실패" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("SETTING_B", L"../Resource/TITLE/SETTING_B.png")) {
        std::cout << "[ERROR] SETTING_B 텍스처 로드 실패" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("QUITGAME_A", L"../Resource/TITLE/QUIT_GAME_A.png")) {
        std::cout << "[ERROR] QUITGAME_A 텍스처 로드 실패" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("QUITGAME_B", L"../Resource/TITLE/QUIT_GAME_B.png")) {
        std::cout << "[ERROR] QUITGAME_B 텍스처 로드 실패" << std::endl;
    }


    if (!AssetManager::Instance().LoadTexture("GrayBird", L"../Resource/Animation/graybirdsheet.png")) {
        std::cout << "[ERROR] GrayBird 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadAnimationRender("GrayBird", L"../Resource/Animation/graybirdsheet.json")) {
        std::cout << "[ERROR] 애니메이션 로드 실패!" << std::endl;
    }
}

bool AssetManager::LoadTexture(const std::string& name, const std::wstring& filePath)
{
    Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
    HRESULT hr = m_wicFactory->CreateDecoderFromFilename(
        filePath.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
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

    if (!converter) {
        std::cout << "[에러] converter is null\n";
        return false;
    }
    if (!m_renderTarget) {
        std::cout << "[에러] render target is null\n";
        return false;
    }

    Microsoft::WRL::ComPtr<ID2D1Bitmap> bitmap;
    hr = m_renderTarget->CreateBitmapFromWicBitmap(converter.Get(), nullptr, &bitmap);
    if (FAILED(hr)) {
        std::cout << "[에러] CreateBitmapFromWicBitmap 실패. HR = 0x" << std::hex << hr << std::endl;
        return false;
    }
    m_textures[name] = bitmap;
    return true;
}

ID2D1Bitmap* AssetManager::GetTexture(const std::string& name) const
{
    auto it = m_textures.find(name);
    if (it != m_textures.end())
    {
        return it->second.Get();
    }
    return nullptr;
}

bool AssetManager::LoadAnimationRender(const std::string& name, const std::wstring& jsonPath)
{
    std::ifstream ifs(jsonPath);
    if (!ifs.is_open()) {
        std::cout << "[ERROR] JSON 파일 열기 실패: " << std::string(jsonPath.begin(), jsonPath.end()) << std::endl;
        return false;
    }

    json j;
    ifs >> j;

    AnimationRenderClip clip;

    for (auto& item : j["frames"].items()) {
        //std::string key = item.key();     // ← 키 접근
        auto& val = item.value();         // ← 값 접근

        auto frame = val["frame"];
        AnimationRenderFrame anim;
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
    m_AnimationRenders[name] = clip;
    return true;
}

const AnimationRenderClip* AssetManager::GetAnimationRender(const std::string& name) const
{
    auto it = m_AnimationRenders.find(name);
    if (it != m_AnimationRenders.end()) {
        return &it->second;
    }
    return nullptr;
}

