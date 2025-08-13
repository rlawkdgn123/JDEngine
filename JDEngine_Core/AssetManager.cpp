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

    if (!AssetManager::Instance().LoadTexture("basic_Torch", L"../Resource/Animation/ART_BG_Torch01.png")) {
        std::cout << "[ERROR] basic_Torch 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Torch", L"../Resource/Animation/ART_BG_TorchSprite01.png")) {
        std::cout << "[ERROR] basic_Torch 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadAnimationRender("Torch", L"../Resource/Animation/ART_BG_TorchSprite01.json")) {
        std::cout << "[ERROR] basic_Torch 애니메이션 로드 실패!" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("basic_Frog", L"../Resource/Animation/Frog_1.png")) {
        std::cout << "[ERROR] basic_Frog 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("cry_Frog", L"../Resource/Animation/Frog_2.png")) {
        std::cout << "[ERROR] cry_Frog 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Frog1", L"../Resource/Animation/ART_BG_FrogSprite01.png")) {
        std::cout << "[ERROR] Frog1 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadAnimationRender("Frog1", L"../Resource/Animation/ART_BG_FrogSprite01.json")) {
        std::cout << "[ERROR] Frog1 애니 로드 실패!" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Frog2", L"../Resource/Animation/ART_BG_FrogSprite02.png")) {
        std::cout << "[ERROR] Frog2 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadAnimationRender("Frog2", L"../Resource/Animation/ART_BG_FrogSprite02.json")) {
        std::cout << "[ERROR] Frog2 애니 로드 실패!" << std::endl;
    }


    if (!AssetManager::Instance().LoadTexture("basic_Rat", L"../Resource/Animation/ART_BG_MouseSprite01.png")) {
        std::cout << "[ERROR] basic_Rat 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Rat", L"../Resource/Animation/Rat_1-sheet.png")) {
        std::cout << "[ERROR] Rat 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadAnimationRender("Rat", L"../Resource/Animation/ART_BG_MouseSprite01.json")) {
        std::cout << "[ERROR] Rat 애니 로드 실패!" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("Hidden_Cat", L"../Resource/testcat.png")) {
        std::cout << "[ERROR] GrayBird 텍스처 로드 실패" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("ART_SpearSprite01", L"../Resource/Animation/ART_SpearSprite01.png")) {
        std::cout << "[ERROR] ART_SpearSprite01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadAnimationRender("ART_SpearSprite01", L"../Resource/Animation/ART_SpearSprite01.json")) {
        std::cout << "[ERROR] ART_SpearSprite01 애니 로드 실패!" << std::endl;
    }

#pragma region Asset_Exam
    if (!AssetManager::Instance().LoadTexture("Art_Opacity", L"../Resource/BATTLE/Art_Opacity.png"))
    {
        std::cout << "[ERROR] Art_Opacity 텍스처 로드 실패" << std::endl;
    }
    // 예시 파일 Exam
    ////////////////////////////////////////////////////////////////////////////////
    if (!AssetManager::Instance().LoadTexture("TITLE_Exam", L"../Resource/TITLE_Exam.png"))
    {
        std::cout << "[ERROR] TITLE_Exam 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Option_Exam01", L"../Resource/Option_Exam01.png"))
    {
        std::cout << "[ERROR] Option_Exam01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Option_Exam02", L"../Resource/Option_Exam02.png"))
    {
        std::cout << "[ERROR] Option_Exam02 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Option_Exam03", L"../Resource/Option_Exam03.png"))
    {
        std::cout << "[ERROR] Option_Exam03 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("SelectCharacter_Exam", L"../Resource/SelectCharacter_Exam.png"))
    {
        std::cout << "[ERROR] SelectCharacter_Exam 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("BATTLE_MAP_Exam", L"../Resource/BATTLE_MAP_Exam.png"))
    {
        std::cout << "[ERROR] BATTLE_MAP_Exam 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_BattleMap01_Example", L"../Resource/BATTLE/ART_BattleMap01_Example.png"))
    {
        std::cout << "[ERROR] ART_BattleMap01_Example 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("BATTLE_MAP_2_Exam", L"../Resource/BATTLE_MAP_2_Exam.png"))
    {
        std::cout << "[ERROR] BATTLE_MAP_2_Exam 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("BATTLE_MAP_3_Exam", L"../Resource/BATTLE_MAP_3_Exam.png"))
    {
        std::cout << "[ERROR] BATTLE_MAP_3_Exam 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("UI_Exam", L"../Resource/UI_Exam.png"))
    {
        std::cout << "[ERROR] UI_Exam 텍스처 로드 실패" << std::endl;
    }

#pragma endregion

#pragma region Asset_TITLE

    // TITLE
    ////////////////////////////////////////////////////////////////////////////////
    if (!AssetManager::Instance().LoadTexture("Test", L"../Resource/Texture/Test.png"))
    {
        std::cout << "[ERROR] Test 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Title01", L"../Resource/TITLE/ART_Title01.png"))
    {
        std::cout << "[ERROR] ART_Title01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Flower01", L"../Resource/TITLE/ART_Flower01.png"))
    {
        std::cout << "[ERROR] ART_Flower01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Flower02", L"../Resource/TITLE/ART_Flower02.png"))
    {
        std::cout << "[ERROR] ART_Flower02 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Flower03", L"../Resource/TITLE/ART_Flower03.png"))
    {
        std::cout << "[ERROR] ART_Flower03 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_GameStart01_Select", L"../Resource/TITLE/ART_GameStart01_Select.png"))
    {
        std::cout << "[ERROR] ART_GameStart01_Select 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_GameStart01_Select_mouseover", L"../Resource/TITLE/ART_GameStart01_Select_mouseover.png"))
    {
        std::cout << "[ERROR] ART_GameStart01_Select_mouseover 텍스처 로드 실패" << std::endl;
    }

#pragma endregion

#pragma region Asset_OPTION
    // OPTION
    ////////////////////////////////////////////////////////////////////////////////
    if (!AssetManager::Instance().LoadTexture("ART_Back01_mouseout", L"../Resource/OPTION/ART_Back01_mouseout.png"))
    {
        std::cout << "[ERROR] ART_Back01_mouseout 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Back01_mouseover", L"../Resource/OPTION/ART_Back01_mouseover.png"))
    {
        std::cout << "[ERROR] ART_Back01_mouseover 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Back02_mouseover02", L"../Resource/OPTION/ART_Back02_mouseover02.png"))
    {
        std::cout << "[ERROR] ART_Back02_mouseover02 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_BG01_OPACITY", L"../Resource/OPTION/ART_BG01_OPACITY.png"))
    {
        std::cout << "[ERROR] ART_BG01_OPACITY 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Art_Button_Mouseover", L"../Resource/OPTION/Art_Button_Mouseover.png"))
    {
        std::cout << "[ERROR] Art_Button_Mouseover 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Art_Button_Mouseover02", L"../Resource/OPTION/Art_Button_Mouseover02.png"))
    {
        std::cout << "[ERROR] Art_Button_Mouseover02 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Control01_mousedown", L"../Resource/OPTION/ART_Control01_mousedown.png"))
    {
        std::cout << "[ERROR] ART_Control01_mousedown 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Credit01_mousedown", L"../Resource/OPTION/ART_Credit01_mousedown.png"))
    {
        std::cout << "[ERROR] ART_Credit01_mousedown 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Volume01_mousedown", L"../Resource/OPTION/ART_Volume01_mousedown.png"))
    {
        std::cout << "[ERROR] ART_Volume01_mousedown 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_VolumeCat01", L"../Resource/OPTION/ART_VolumeCat01.png"))
    {
        std::cout << "[ERROR] ART_VolumeCat01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_VolumeCat02", L"../Resource/OPTION/ART_VolumeCat02.png"))
    {
        std::cout << "[ERROR] ART_VolumeCat02 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_VolumeSlider01", L"../Resource/OPTION/ART_VolumeSlider01.png"))
    {
        std::cout << "[ERROR] ART_VolumeSlider01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_VolumeSlider02", L"../Resource/OPTION/ART_VolumeSlider02.png"))
    {
        std::cout << "[ERROR] ART_VolumeSlider02 텍스처 로드 실패" << std::endl;
    }
#pragma endregion

#pragma region Asset_CHARACTER_SELECT
    // CHARACTER_SELECT
    ////////////////////////////////////////////////////////////////////////////////
    if (!AssetManager::Instance().LoadTexture("ART_Back02_mouseout", L"../Resource/CHARACTER SELECT/ART_Back02_mouseout.png"))
    {
        std::cout << "[ERROR] ART_Back02_mouseout 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Back02_mouseover", L"../Resource/CHARACTER SELECT/ART_Back02_mouseover.png"))
    {
        std::cout << "[ERROR] ART_Back02_mouseover 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Back02_mouseover02", L"../Resource/CHARACTER SELECT/ART_Back02_mouseover02.png"))
    {
        std::cout << "[ERROR] ART_Back02_mouseover02 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_CH_BACK", L"../Resource/CHARACTER SELECT/ART_CH_BACK.png"))
    {
        std::cout << "[ERROR] ART_CH_BACK 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_CHAT", L"../Resource/CHARACTER SELECT/ART_CHAT.png"))
    {
        std::cout << "[ERROR] ART_CHAT 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_CHAT_mouseout", L"../Resource/CHARACTER SELECT/ART_CHAT_mouseout.png"))
    {
        std::cout << "[ERROR] ART_CHAT_mouseout 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_CHAT_mouseover", L"../Resource/CHARACTER SELECT/ART_CHAT_mouseover.png"))
    {
        std::cout << "[ERROR] ART_CHAT_mouseover 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Q_1", L"../Resource/CHARACTER SELECT/ART_Q_1.png"))  // 기본
    {
        std::cout << "[ERROR] ART_Q_1 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Q_2", L"../Resource/CHARACTER SELECT/ART_Q_2.png"))  // 코네
    {
        std::cout << "[ERROR] ART_Q_2 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Q_3", L"../Resource/CHARACTER SELECT/ART_Q_3.png"))  // 펠리스
    {
        std::cout << "[ERROR] ART_Q_3 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Q_4", L"../Resource/CHARACTER SELECT/ART_Q_4.png"))  // 나비
    {
        std::cout << "[ERROR] ART_Q_4 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_SelectFelis01_mouseout", L"../Resource/CHARACTER SELECT/ART_SelectFelis01_mouseout.png"))
    {
        std::cout << "[ERROR] ART_SelectFelis01_mouseout 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_SelectFelis01_mouseover", L"../Resource/CHARACTER SELECT/ART_SelectFelis01_mouseover.png"))
    {
        std::cout << "[ERROR] ART_SelectFelis01_mouseover 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_SelectKone01_mouseout", L"../Resource/CHARACTER SELECT/ART_SelectKone01_mouseout.png"))
    {
        std::cout << "[ERROR] ART_SelectKone01_mouseout 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_SelectKone01_mouseover", L"../Resource/CHARACTER SELECT/ART_SelectKone01_mouseover.png"))
    {
        std::cout << "[ERROR] ART_SelectKone01_mouseover 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_SelectNavi01_mouseout", L"../Resource/CHARACTER SELECT/ART_SelectNavi01_mouseout.png"))
    {
        std::cout << "[ERROR] ART_SelectNavi01_mouseout 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_SelectNavi01_mouseover", L"../Resource/CHARACTER SELECT/ART_SelectNavi01_mouseover.png"))
    {
        std::cout << "[ERROR] ART_SelectNavi01_mouseover 텍스처 로드 실패" << std::endl;
    }
#pragma endregion

    // 고양이 애니메이션 NORWAY, RUSS
    ////////////////////////////////////////////////////////////////////////////////

    // 애니메이션 속도 정배율을 1.4x 가 마음에 든다고 했음.
    if (!AssetManager::Instance().LoadTexture("Norway", L"../Resource/Animation/character_norway_sprite_01.png"))
    {
        std::cout << "[ERROR] Norway 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadAnimationRender("Norway", L"../Resource/Animation/character_norway_sprite_01.json"))
    {
        std::cout << "[ERROR] Norway 애니메이션 로드 실패!" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("f1", L"../Resource/character/Norway/f1.png"))
    {
        std::cout << "[ERROR] f1 텍스처 로드 실패!" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("Russ", L"../Resource/Animation/character_russ_sprite_01.png"))
    {
        std::cout << "[ERROR] Russ 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadAnimationRender("Russ", L"../Resource/Animation/character_russ_sprite_01.json"))
    {
        std::cout << "[ERROR] Russ 애니메이션 로드 실패!" << std::endl;
    }

#pragma region character

    // character
    ////////////////////////////////////////////////////////////////////////////////

    ///// 견습 냥이, 수습 냥이
    if (!AssetManager::Instance().LoadTexture("ART_RecruitFelis01", L"../Resource/character/ART_RecruitFelis01.png"))
    {
        std::cout << "[ERROR] ART_RecruitFelis01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_RecruitFelis02", L"../Resource/character/ART_RecruitFelis02.png"))
    {
        std::cout << "[ERROR] ART_RecruitFelis02 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_RecruitKone01", L"../Resource/character/ART_RecruitKone01.png"))
    {
        std::cout << "[ERROR] ART_RecruitKone01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_RecruitKone02", L"../Resource/character/ART_RecruitKone02.png"))
    {
        std::cout << "[ERROR] ART_RecruitKone02 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_RecruitNavi01", L"../Resource/character/ART_RecruitNavi01.png"))
    {
        std::cout << "[ERROR] ART_RecruitNavi01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_RecruitNavi02", L"../Resource/character/ART_RecruitNavi02.png"))
    {
        std::cout << "[ERROR] ART_RecruitNavi02 텍스처 로드 실패" << std::endl;
    }

    ///// 펠리스
    if (!AssetManager::Instance().LoadAnimationRender("ART_FelisAdv_Sprite01", L"../Resource/character/Norway/adv/ART_FelisAdv_Sprite01.json"))
    {
        std::cout << "[ERROR] ART_FelisAdv_Sprite01 애니메이션 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_FelisAdv_Sprite01", L"../Resource/character/Norway/adv/ART_FelisAdv_Sprite01.png"))
    {
        std::cout << "[ERROR] ART_FelisAdv_Sprite01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Norway_adv_Frame 1", L"../Resource/character/Norway/adv/Frame 1.png"))
    {
        std::cout << "[ERROR] Frame 1 텍스처 로드 실패" << std::endl;
    }


    if (!AssetManager::Instance().LoadAnimationRender("ART_FelisEx_Sprite01", L"../Resource/character/Norway/ex/ART_FelisEx_Sprite01.json"))
    {
        std::cout << "[ERROR] ART_FelisEx_Sprite01 애니메이션 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_FelisEx_Sprite01", L"../Resource/character/Norway/ex/ART_FelisEx_Sprite01.png"))
    {
        std::cout << "[ERROR] ART_FelisEx_Sprite01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Norway_ex_Frame 1", L"../Resource/character/Norway/ex/Frame 1.png"))
    {
        std::cout << "[ERROR] Frame 1 텍스처 로드 실패" << std::endl;
    }

    if (!AssetManager::Instance().LoadAnimationRender("ART_FelisNov_Sprite01", L"../Resource/character/Norway/nov/ART_FelisNov_Sprite01.json"))
    {
        std::cout << "[ERROR] ART_FelisNov_Sprite01 애니메이션 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_FelisNov_Sprite01", L"../Resource/character/Norway/nov/ART_FelisNov_Sprite01.png"))
    {
        std::cout << "[ERROR] ART_FelisNov_Sprite01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Norway_nov_Frame 1", L"../Resource/character/Norway/nov/Frame 1.png"))
    {
        std::cout << "[ERROR] Frame 1 텍스처 로드 실패" << std::endl;
    }

    ///// 코네
    if (!AssetManager::Instance().LoadAnimationRender("ART_KoneAdv_Sprite01", L"../Resource/character/Russ/adv/ART_KoneAdv_Sprite01.json"))
    {
        std::cout << "[ERROR] ART_KoneAdv_Sprite01 애니메이션 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_KoneAdv_Sprite01", L"../Resource/character/Russ/adv/ART_KoneAdv_Sprite01.png"))
    {
        std::cout << "[ERROR] ART_KoneAdv_Sprite01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Russ_adv_Frame 1", L"../Resource/character/Russ/adv/Frame 1.png"))
    {
        std::cout << "[ERROR] Frame 1 텍스처 로드 실패" << std::endl;
    }


    if (!AssetManager::Instance().LoadAnimationRender("ART_KoneEx_Sprite01", L"../Resource/character/Russ/ex/ART_KoneEx_Sprite01.json"))
    {
        std::cout << "[ERROR] ART_KoneEx_Sprite01 애니메이션 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_KoneEx_Sprite01", L"../Resource/character/Russ/ex/ART_KoneEx_Sprite01.png"))
    {
        std::cout << "[ERROR] ART_KoneEx_Sprite01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Russ_ex_Frame 1", L"../Resource/character/Russ/ex/Frame 1.png"))
    {
        std::cout << "[ERROR] Frame 1 텍스처 로드 실패" << std::endl;
    }

    if (!AssetManager::Instance().LoadAnimationRender("ART_KoneNov_Sprite01", L"../Resource/character/Russ/nov/ART_KoneNov_Sprite01.json"))
    {
        std::cout << "[ERROR] ART_KoneNov_Sprite01 애니메이션 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_KoneNov_Sprite01", L"../Resource/character/Russ/nov/ART_KoneNov_Sprite01.png"))
    {
        std::cout << "[ERROR] ART_KoneNov_Sprite01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Russ_nov_Frame 1", L"../Resource/character/Russ/nov/Frame 1.png"))
    {
        std::cout << "[ERROR] Frame 1 텍스처 로드 실패" << std::endl;
    }


    ///// 나비
    if (!AssetManager::Instance().LoadAnimationRender("ART_NaviAdv_Sprite01", L"../Resource/character/Scottish/adv/ART_NaviAdv_Sprite01.json"))
    {
        std::cout << "[ERROR] ART_NaviAdv_Sprite01 애니메이션 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_NaviAdv_Sprite01", L"../Resource/character/Scottish/adv/ART_NaviAdv_Sprite01.png"))
    {
        std::cout << "[ERROR] ART_NaviAdv_Sprite01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Scottish_adv_Frame 1", L"../Resource/character/Scottish/adv/Frame 1.png"))
    {
        std::cout << "[ERROR] Frame 1 텍스처 로드 실패" << std::endl;
    }


    if (!AssetManager::Instance().LoadAnimationRender("ART_NaviEx_Sprite01", L"../Resource/character/Scottish/ex/ART_NaviEx_Sprite01.json"))
    {
        std::cout << "[ERROR] ART_NaviEx_Sprite01 애니메이션 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_NaviEx_Sprite01", L"../Resource/character/Scottish/ex/ART_NaviEx_Sprite01.png"))
    {
        std::cout << "[ERROR] ART_NaviEx_Sprite01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Scottish_ex_Frame 1", L"../Resource/character/Scottish/ex/Frame 1.png"))
    {
        std::cout << "[ERROR] Frame 1 텍스처 로드 실패" << std::endl;
    }

    if (!AssetManager::Instance().LoadAnimationRender("ART_NaviNov_Sprite01", L"../Resource/character/Scottish/nov/ART_NaviNov_Sprite01.json"))
    {
        std::cout << "[ERROR] ART_NaviNov_Sprite01 애니메이션 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_NaviNov_Sprite01", L"../Resource/character/Scottish/nov/ART_NaviNov_Sprite01.png"))
    {
        std::cout << "[ERROR] ART_NaviNov_Sprite01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Scottish_nov_Frame 1", L"../Resource/character/Scottish/nov/Frame 1.png"))
    {
        std::cout << "[ERROR] Frame 1 텍스처 로드 실패" << std::endl;
    }

#pragma endregion




#pragma region Asset_BATTLE

    // BATTLE
    ////////////////////////////////////////////////////////////////////////////////
    if (!AssetManager::Instance().LoadTexture("BATTLE_MAP(old)", L"../Resource/BATTLE/BATTLE_MAP(old).png"))
    {
        std::cout << "[ERROR] BATTLE_MAP(old) 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_BattleMap01", L"../Resource/BATTLE/ART_BattleMap01.png"))
    {
        std::cout << "[ERROR] ART_BattleMap01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_BattleMapLayer01", L"../Resource/BATTLE/ART_BattleMapLayer01.png"))
    {
        std::cout << "[ERROR] ART_BattleMapLayer01 텍스처 로드 실패" << std::endl;
    }


    if (!AssetManager::Instance().LoadTexture("ART_Away01", L"../Resource/BATTLE/ART_Away01.png"))
    {
        std::cout << "[ERROR] ART_Away01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_AwayGauge01", L"../Resource/BATTLE/ART_AwayGauge01.png"))
    {
        std::cout << "[ERROR] ART_AwayGauge01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Barracks01", L"../Resource/BATTLE/ART_Barracks01.png"))
    {
        std::cout << "[ERROR] ART_Barracks01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Barracks02", L"../Resource/BATTLE/ART_Barracks02.png"))
    {
        std::cout << "[ERROR] ART_Barracks02 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Barracks02_mouse_over", L"../Resource/BATTLE/ART_Barracks02_mouse_over.png"))
    {
        std::cout << "[ERROR] ART_Barracks02_mouse_over 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Barracks02_mouseout_", L"../Resource/BATTLE/ART_Barracks02_mouseout_.png"))
    {
        std::cout << "[ERROR] ART_Barracks02_mouseout_ 텍스처 로드 실패" << std::endl;
    }


    if (!AssetManager::Instance().LoadTexture("ART_BuffFelis", L"../Resource/BATTLE/ART_BuffFelis.png"))
    {
        std::cout << "[ERROR] ART_BuffFelis 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_BuffKone01", L"../Resource/BATTLE/ART_BuffKone01.png"))
    {
        std::cout << "[ERROR] ART_BuffKone01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_BuffNavi01", L"../Resource/BATTLE/ART_BuffNavi01.png"))
    {
        std::cout << "[ERROR] ART_BuffNavi01 텍스처 로드 실패" << std::endl;
    }


    if (!AssetManager::Instance().LoadTexture("ART_BuildCabin01", L"../Resource/BATTLE/ART_BuildCabin01.png"))
    {
        std::cout << "[ERROR] ART_BuildCabin01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_BuildFishing01", L"../Resource/BATTLE/ART_BuildFishing01.png"))
    {
        std::cout << "[ERROR] ART_BuildFishing01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_BuildLab01", L"../Resource/BATTLE/ART_BuildLab01.png"))
    {
        std::cout << "[ERROR] ART_BuildLab01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_BuildMine01", L"../Resource/BATTLE/ART_BuildMine01.png"))
    {
        std::cout << "[ERROR] ART_BuildMine01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_BuildLumbermill01", L"../Resource/BATTLE/ART_BuildLumbermill01.png"))
    {
        std::cout << "[ERROR] ART_BuildLumbermill01 텍스처 로드 실패" << std::endl;
    }


    if (!AssetManager::Instance().LoadTexture("ART_CatFelis01", L"../Resource/BATTLE/ART_CatFelis01.png"))
    {
        std::cout << "[ERROR] ART_CatFelis01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_CatKone01", L"../Resource/BATTLE/ART_CatKone01.png"))
    {
        std::cout << "[ERROR] ART_CatKone01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_CatNavi01", L"../Resource/BATTLE/ART_CatNavi01.png"))
    {
        std::cout << "[ERROR] ART_CatNavi01 텍스처 로드 실패" << std::endl;
    }


    if (!AssetManager::Instance().LoadTexture("ART_CostFood01", L"../Resource/BATTLE/ART_CostFood01.png"))
    {
        std::cout << "[ERROR] ART_CostFood01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_CostMineral01", L"../Resource/BATTLE/ART_CostMineral01.png"))
    {
        std::cout << "[ERROR] ART_CostMineral01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_CostPop01", L"../Resource/BATTLE/ART_CostPop01.png"))
    {
        std::cout << "[ERROR] ART_CostPop01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_CostWood01", L"../Resource/BATTLE/ART_CostWood01.png"))
    {
        std::cout << "[ERROR] ART_CostWood01 텍스처 로드 실패" << std::endl;
    }


    if (!AssetManager::Instance().LoadTexture("ART_Downgrade01", L"../Resource/BATTLE/ART_Downgrade01.png"))
    {
        std::cout << "[ERROR] ART_Downgrade01 텍스처 로드 실패" << std::endl;
    }


    if (!AssetManager::Instance().LoadTexture("ART_Fast01", L"../Resource/BATTLE/ART_Fast01.png"))
    {
        std::cout << "[ERROR] ART_Fast01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Fast01_ing", L"../Resource/BATTLE/ART_Fast01_ing.png"))
    {
        std::cout << "[ERROR] ART_Fast01_ing 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Pause01", L"../Resource/BATTLE/ART_Pause01.png"))
    {
        std::cout << "[ERROR] ART_Pause01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Pause01_ing", L"../Resource/BATTLE/ART_Pause01_ing.png"))
    {
        std::cout << "[ERROR] ART_Pause01_ing 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Play01", L"../Resource/BATTLE/ART_Play01.png"))
    {
        std::cout << "[ERROR] ART_Play01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Play01_ing", L"../Resource/BATTLE/ART_Play01_ing.png"))
    {
        std::cout << "[ERROR] ART_Play01_ing 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_SettingIcon01", L"../Resource/BATTLE/ART_SettingIcon01.png"))
    {
        std::cout << "[ERROR] ART_SettingIcon01 텍스처 로드 실패" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("ART_QuickPlay01", L"../Resource/BATTLE/ART_QuickPlay01.png"))
    {
        std::cout << "[ERROR] ART_QuickPlay01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_QuickPlay01_ing", L"../Resource/BATTLE/ART_QuickPlay01_ing.png"))
    {
        std::cout << "[ERROR] ART_QuickPlay01_ing 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_QuickPlay02_ing", L"../Resource/BATTLE/ART_QuickPlay02_ing.png"))
    {
        std::cout << "[ERROR] ART_QuickPlay02_ing 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_QuickPlay03_ing", L"../Resource/BATTLE/ART_QuickPlay03_ing.png"))
    {
        std::cout << "[ERROR] ART_QuickPlay03_ing 텍스처 로드 실패" << std::endl;
    }


    if (!AssetManager::Instance().LoadTexture("ART_Monster01", L"../Resource/BATTLE/ART_Monster01.png"))
    {
        std::cout << "[ERROR] ART_Monster01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Monster02", L"../Resource/BATTLE/ART_Monster02.png"))
    {
        std::cout << "[ERROR] ART_Monster02 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Monster03", L"../Resource/BATTLE/ART_Monster03.png"))
    {
        std::cout << "[ERROR] ART_Monster03 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Monster04", L"../Resource/BATTLE/ART_Monster04.png"))
    {
        std::cout << "[ERROR] ART_Monster04 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Monster05", L"../Resource/BATTLE/ART_Monster05.png"))
    {
        std::cout << "[ERROR] ART_Monster05 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Enemy_Mark", L"../Resource/BATTLE/ART_Enemy_Mark.png"))
    {
        std::cout << "[ERROR] ART_Monster05 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Player_Mark", L"../Resource/BATTLE/ART_Player_Mark.png"))
    {
        std::cout << "[ERROR] ART_Monster05 텍스처 로드 실패" << std::endl;
    }


    if (!AssetManager::Instance().LoadTexture("ART_RESFood01", L"../Resource/BATTLE/ART_RESFood01.png"))
    {
        std::cout << "[ERROR] ART_RESFood01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_RESMineral01", L"../Resource/BATTLE/ART_RESMineral01.png"))
    {
        std::cout << "[ERROR] ART_RESMineral01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_RESPop01", L"../Resource/BATTLE/ART_RESPop01.png"))
    {
        std::cout << "[ERROR] ART_RESPop01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_RESWood01", L"../Resource/BATTLE/ART_RESWood01.png"))
    {
        std::cout << "[ERROR] ART_RESWood01 텍스처 로드 실패" << std::endl;
    }


    if (!AssetManager::Instance().LoadTexture("ART_Stiker01", L"../Resource/BATTLE/ART_Stiker01.png"))
    {
        std::cout << "[ERROR] ART_Stiker01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Stiker02", L"../Resource/BATTLE/ART_Stiker02.png"))
    {
        std::cout << "[ERROR] ART_Stiker02 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Stiker03", L"../Resource/BATTLE/ART_Stiker03.png"))
    {
        std::cout << "[ERROR] ART_Stiker03 텍스처 로드 실패" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("ART_Rampart01", L"../Resource/BATTLE/ART_Rampart01.png"))
    {
        std::cout << "[ERROR] ART_Rampart01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_RecruitCat03", L"../Resource/BATTLE/ART_RecruitCat03.png"))
    {
        std::cout << "[ERROR] ART_RecruitCat03 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Select01", L"../Resource/BATTLE/ART_Select01.png"))
    {
        std::cout << "[ERROR] ART_Select01 텍스처 로드 실패" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("ART_TileCabin01", L"../Resource/BATTLE/ART_TileCabin01.png"))
    {
        std::cout << "[ERROR] ART_TileCabin01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_TileFishing01", L"../Resource/BATTLE/ART_TileFishing01.png"))
    {
        std::cout << "[ERROR] ART_TileFishing01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_TileLab01", L"../Resource/BATTLE/ART_TileLab01.png"))
    {
        std::cout << "[ERROR] ART_TileLab01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_TileLumbermill01", L"../Resource/BATTLE/ART_TileLumbermill01.png"))
    {
        std::cout << "[ERROR] ART_TileLumbermill01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_TileMine01", L"../Resource/BATTLE/ART_TileMine01.png"))
    {
        std::cout << "[ERROR] ART_TileMine01 텍스처 로드 실패" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("ART_UIBuilding01_Board", L"../Resource/BATTLE/ART_UIBuilding01_Board.png"))
    {
        std::cout << "[ERROR] ART_UIBuilding01_Board 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_UIBuilding01", L"../Resource/BATTLE/ART_UIBuilding01.png"))
    {
        std::cout << "[ERROR] ART_UIBuilding01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_UICharSelect01", L"../Resource/BATTLE/ART_UICharSelect01.png"))
    {
        std::cout << "[ERROR] ART_UICharSelect01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_UIRecruit01", L"../Resource/BATTLE/ART_UIRecruit01.png"))
    {
        std::cout << "[ERROR] ART_UIRecruit01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_UIWaveGauge01", L"../Resource/BATTLE/ART_UIWaveGauge01.png"))
    {
        std::cout << "[ERROR] ART_UIWaveGauge01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_UIDate01", L"../Resource/BATTLE/ART_UIDate01.png"))
    {
        std::cout << "[ERROR] ART_UIDate01 텍스처 로드 실패" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("ART_Upgrade01", L"../Resource/BATTLE/ART_Upgrade01.png"))
    {
        std::cout << "[ERROR] ART_Upgrade01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Upgrade02", L"../Resource/BATTLE/ART_Upgrade02.png"))
    {
        std::cout << "[ERROR] ART_Upgrade02 텍스처 로드 실패" << std::endl;
    }

    // 타일 1x1
    if (!AssetManager::Instance().LoadTexture("ART_Tile01", L"../Resource/BATTLE/ART_Tile01.png"))
    {
        std::cout << "[ERROR] ART_Tile01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Tile02", L"../Resource/BATTLE/ART_Tile02.png"))
    {
        std::cout << "[ERROR] ART_Tile02 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Tile03", L"../Resource/BATTLE/ART_Tile03.png"))
    {
        std::cout << "[ERROR] ART_Tile03 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Tile04", L"../Resource/BATTLE/ART_Tile04.png"))
    {
        std::cout << "[ERROR] ART_Tile04 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Tile05", L"../Resource/BATTLE/ART_Tile05.png"))
    {
        std::cout << "[ERROR] ART_Tile05 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Tile06", L"../Resource/BATTLE/ART_Tile06.png"))
    {
        std::cout << "[ERROR] ART_Tile06 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Tile07", L"../Resource/BATTLE/ART_Tile07.png"))
    {
        std::cout << "[ERROR] ART_Tile07 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Tile08", L"../Resource/BATTLE/ART_Tile08.png"))
    {
        std::cout << "[ERROR] ART_Tile08 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Tile09", L"../Resource/BATTLE/ART_Tile09.png"))
    {
        std::cout << "[ERROR] ART_Tile09 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Tile10", L"../Resource/BATTLE/ART_Tile10.png"))
    {
        std::cout << "[ERROR] ART_Tile10 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Tile11", L"../Resource/BATTLE/ART_Tile11.png"))
    {
        std::cout << "[ERROR] ART_Tile11 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Tile12", L"../Resource/BATTLE/ART_Tile12.png"))
    {
        std::cout << "[ERROR] ART_Tile12 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Tile13", L"../Resource/BATTLE/ART_Tile13.png"))
    {
        std::cout << "[ERROR] ART_Tile13 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Tile14", L"../Resource/BATTLE/ART_Tile14.png"))
    {
        std::cout << "[ERROR] ART_Tile14 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Tile15", L"../Resource/BATTLE/ART_Tile15.png"))
    {
        std::cout << "[ERROR] ART_Tile15 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Tile16", L"../Resource/BATTLE/ART_Tile16.png"))
    {
        std::cout << "[ERROR] ART_Tile16 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Tile17", L"../Resource/BATTLE/ART_Tile17.png"))
    {
        std::cout << "[ERROR] ART_Tile17 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Tile18", L"../Resource/BATTLE/ART_Tile18.png"))
    {
        std::cout << "[ERROR] ART_Tile18 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Tile19", L"../Resource/BATTLE/ART_Tile19.png"))
    {
        std::cout << "[ERROR] ART_Tile19 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Tile20", L"../Resource/BATTLE/ART_Tile20.png"))
    {
        std::cout << "[ERROR] ART_Tile20 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Tile21", L"../Resource/BATTLE/ART_Tile21.png"))
    {
        std::cout << "[ERROR] ART_Tile21 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Tile22", L"../Resource/BATTLE/ART_Tile22.png"))
    {
        std::cout << "[ERROR] ART_Tile22 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Tile23", L"../Resource/BATTLE/ART_Tile23.png"))
    {
        std::cout << "[ERROR] ART_Tile23 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Tile24", L"../Resource/BATTLE/ART_Tile24.png"))
    {
        std::cout << "[ERROR] ART_Tile24 텍스처 로드 실패" << std::endl;
    }


    if (!AssetManager::Instance().LoadTexture("PLAY", L"../Resource/BATTLE/PLAY.png"))
    {
        std::cout << "[ERROR] PLAY 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("FAST", L"../Resource/BATTLE/FAST.png"))
    {
        std::cout << "[ERROR] FAST 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("STOP", L"../Resource/BATTLE/STOP.png"))
    {
        std::cout << "[ERROR] STOP 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("SETTING_ICON", L"../Resource/BATTLE/SETTING_ICON.png"))
    {
        std::cout << "[ERROR] SETTING_ICON 텍스처 로드 실패" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("ART_Barracks_HP", L"../Resource/BATTLE/ART_Barracks_HP.png"))
    {
        std::cout << "[ERROR] ART_Barracks_HP 텍스처 로드 실패" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("ART_Information_Box_Expedition", L"../Resource/BATTLE/ART_Information_Box_Expedition.png"))
    {
        std::cout << "[ERROR] ART_Information_Box_Expedition 텍스처 로드 실패" << std::endl;
    }

    // 시너지 효과
    if (!AssetManager::Instance().LoadTexture("ART_Down5", L"../Resource/BATTLE/ART_Down5.png"))
    {
        std::cout << "[ERROR] ART_Down5 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Down10", L"../Resource/BATTLE/ART_Down10.png"))
    {
        std::cout << "[ERROR] ART_Down10 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Up5", L"../Resource/BATTLE/ART_Up5.png"))
    {
        std::cout << "[ERROR] ART_Up5 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_Up10", L"../Resource/BATTLE/ART_Up10.png"))
    {
        std::cout << "[ERROR] ART_Up10 텍스처 로드 실패" << std::endl;
    }

    // 그리드 테두리
    if (!AssetManager::Instance().LoadTexture("Tile_Stroke01", L"../Resource/BATTLE/Tile_Stroke01.png"))
    {
        std::cout << "[ERROR] Tile_Stroke01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Tile_Stroke02", L"../Resource/BATTLE/Tile_Stroke02.png"))
    {
        std::cout << "[ERROR] Tile_Stroke02 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Tile_Stroke03", L"../Resource/BATTLE/Tile_Stroke03.png"))
    {
        std::cout << "[ERROR] Tile_Stroke03 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Tile_Stroke04", L"../Resource/BATTLE/Tile_Stroke04.png"))
    {
        std::cout << "[ERROR] Tile_Stroke04 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Tile_Stroke05", L"../Resource/BATTLE/Tile_Stroke05.png"))
    {
        std::cout << "[ERROR] Tile_Stroke05 텍스처 로드 실패" << std::endl;
    }

    // 이름
    if (!AssetManager::Instance().LoadTexture("ART_Building_Level", L"../Resource/BATTLE/ART_Building_Level.png"))
    {
        std::cout << "[ERROR] ART_Building_Level 텍스처 로드 실패" << std::endl;
    }

    // 웨이브
    if (!AssetManager::Instance().LoadTexture("ART_Monster001", L"../Resource/BATTLE/ART_Monster001.png"))
    {
        std::cout << "[ERROR] ART_Monster001 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_BACK_TO_MENU02", L"../Resource/BATTLE/ART_BACK_TO_MENU02.png"))
    {
        std::cout << "[ERROR] ART_BACK_TO_MENU02 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_BACK_TO_MENU01", L"../Resource/BATTLE/ART_BACK_TO_MENU01.png"))
    {
        std::cout << "[ERROR] ART_BACK_TO_MENU01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_RETRY02", L"../Resource/BATTLE/ART_RETRY02.png"))
    {
        std::cout << "[ERROR] ART_RETRY02 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_RETRY01", L"../Resource/BATTLE/ART_RETRY01.png"))
    {
        std::cout << "[ERROR] ART_RETRY01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_BadEnd01", L"../Resource/BATTLE/ART_BadEnd01.png"))
    {
        std::cout << "[ERROR] ART_BadEnd01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_GoodEnd01", L"../Resource/BATTLE/ART_GoodEnd01.png"))
    {
        std::cout << "[ERROR] ART_GoodEnd01 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_NomalEnd01", L"../Resource/BATTLE/ART_NomalEnd01.png"))
    {
        std::cout << "[ERROR] ART_NomalEnd01 텍스처 로드 실패" << std::endl;
    }

#pragma endregion

    // 초급, 중급, 상급
    if (!AssetManager::Instance().LoadTexture("Art_Expedition_Button(Lv01)_Level", L"../Resource/BATTLE/Expedition/Art_Expedition_Button(Lv01)_Level.png"))    // 초급
    {
        std::cout << "[ERROR] Art_Expedition_Button(Lv01)_Level 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Art_Expedition_Button(Lv01)_Level_mouseover", L"../Resource/BATTLE/Expedition/Art_Expedition_Button(Lv01)_Level_mouseover.png"))
    {
        std::cout << "[ERROR] Art_Expedition_Button(Lv01)_Level_mouseover 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Art_Expedition_Button(Lv02)_Level", L"../Resource/BATTLE/Expedition/Art_Expedition_Button(Lv02)_Level.png"))    // 중급
    {
        std::cout << "[ERROR] Art_Expedition_Button(Lv02)_Level 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Art_Expedition_Button(Lv02)_Level_mouseover", L"../Resource/BATTLE/Expedition/Art_Expedition_Button(Lv02)_Level_mouseover.png"))
    {
        std::cout << "[ERROR] Art_Expedition_Button(Lv02)_Level_mouseover 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Art_Expedition_Button(Lv03)_Level", L"../Resource/BATTLE/Expedition/Art_Expedition_Button(Lv03)_Level.png"))    // 상급
    {
        std::cout << "[ERROR] Art_Expedition_Button(Lv03)_Level 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Art_Expedition_Button(Lv03)_Level_mouseover", L"../Resource/BATTLE/Expedition/Art_Expedition_Button(Lv03)_Level_mouseover.png"))
    {
        std::cout << "[ERROR] Art_Expedition_Button(Lv03)_Level_mouseover 텍스처 로드 실패" << std::endl;
    }


    // 병력보내기
    if (!AssetManager::Instance().LoadTexture("Art_Expedition_Button(Lv01)", L"../Resource/BATTLE/Expedition/Art_Expedition_Button(Lv01).png"))    // 초급
    {
        std::cout << "[ERROR] Art_Expedition_Button(Lv01) 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Art_Expedition_Button(Lv01)_mouseover", L"../Resource/BATTLE/Expedition/Art_Expedition_Button(Lv01)_mouseover.png"))
    {
        std::cout << "[ERROR] Art_Expedition_Button(Lv01)_mouseover 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Art_Expedition_Button(Lv02)", L"../Resource/BATTLE/Expedition/Art_Expedition_Button(Lv02).png"))    // 중급
    {
        std::cout << "[ERROR] Art_Expedition_Button(Lv02) 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Art_Expedition_Button(Lv02)_mouseover", L"../Resource/BATTLE/Expedition/Art_Expedition_Button(Lv02)_mouseover.png"))
    {
        std::cout << "[ERROR] Art_Expedition_Button(Lv02)_mouseover 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Art_Expedition_Button(Lv03)", L"../Resource/BATTLE/Expedition/Art_Expedition_Button(Lv03).png"))    // 상급
    {
        std::cout << "[ERROR] Art_Expedition_Button(Lv03)_Level 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Art_Expedition_Button(Lv03)_mouseover", L"../Resource/BATTLE/Expedition/Art_Expedition_Button(Lv03)_mouseover.png"))
    {
        std::cout << "[ERROR] Art_Expedition_Button(Lv03)_mouseover 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Art_Expedition_Button(Cannot)", L"../Resource/BATTLE/Expedition/Art_Expedition_Button(Cannot).png"))    // 원정 못감.
    {
        std::cout << "[ERROR] Art_Expedition_Button(Cannot) 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Art_Expedition_Button(Cannot)_mouseover", L"../Resource/BATTLE/Expedition/Art_Expedition_Button(Cannot)_mouseover.png"))
    {
        std::cout << "[ERROR] Art_Expedition_Button(Cannot)_mouseover 텍스처 로드 실패" << std::endl;
    }


    // 별
    if (!AssetManager::Instance().LoadTexture("Art_Expedition_Level_Empty", L"../Resource/BATTLE/Expedition/Art_Expedition_Level_Empty.png"))
    {
        std::cout << "[ERROR] Art_Expedition_Level_Empty 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("Art_Expedition_Level_Full", L"../Resource/BATTLE/Expedition/Art_Expedition_Level_Full.png"))
    {
        std::cout << "[ERROR] Art_Expedition_Level_Full 텍스처 로드 실패" << std::endl;
    }

    // 국가별 배치 고양이
    // 코네 국가
    if (!AssetManager::Instance().LoadTexture("ART_EmpCatFelis01_mouseout", L"../Resource/BATTLE/ART_EmpCatFelis01_mouseout.png"))
    {
        std::cout << "[ERROR] ART_EmpCatFelis01_mouseout 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_EmpCatKone01_mouseout", L"../Resource/BATTLE/ART_EmpCatKone01_mouseout.png"))
    {
        std::cout << "[ERROR] ART_EmpCatKone01_mouseout 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_EmpCatNavi01_mouseout", L"../Resource/BATTLE/ART_EmpCatNavi01_mouseout.png"))
    {
        std::cout << "[ERROR] ART_EmpCatNavi01_mouseout 텍스처 로드 실패" << std::endl;
    }

    // 펠리스 국가
    if (!AssetManager::Instance().LoadTexture("ART_RepCatFelis01_mouseout", L"../Resource/BATTLE/ART_RepCatFelis01_mouseout.png"))
    {
        std::cout << "[ERROR] ART_RepCatFelis01_mouseout 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_RepCatKone01_mouseout", L"../Resource/BATTLE/ART_RepCatKone01_mouseout.png"))
    {
        std::cout << "[ERROR] ART_RepCatKone01_mouseout 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_RepCatNavi01_mouseout", L"../Resource/BATTLE/ART_RepCatNavi01_mouseout.png"))
    {
        std::cout << "[ERROR] ART_RepCatNavi01_mouseout 텍스처 로드 실패" << std::endl;
    }

    // 나비 국가
    if (!AssetManager::Instance().LoadTexture("ART_UniteCatFelis01_mouseout", L"../Resource/BATTLE/ART_UniteCatFelis01_mouseout.png"))
    {
        std::cout << "[ERROR] ART_UniteCatFelis01_mouseout 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_UniteCatKone01_mouseout", L"../Resource/BATTLE/ART_UniteCatKone01_mouseout.png"))
    {
        std::cout << "[ERROR] ART_UniteCatKone01_mouseout 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("ART_UniteCatNavi01_mouseout", L"../Resource/BATTLE/ART_UniteCatNavi01_mouseout.png"))
    {
        std::cout << "[ERROR] ART_UniteCatNavi01_mouseout 텍스처 로드 실패" << std::endl;
    }

    
    // Patikle Resource
      ////////////////////////////////////////////////////////////////////////////////
    if (!AssetManager::Instance().LoadTexture("blossom", L"../Resource/Patikle/ART_Flower01.png"))
    {
        std::cout << "[ERROR] blossom 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("leaf", L"../Resource/Patikle/ART_LeefParticle01.png"))
    {
        std::cout << "[ERROR] leaf 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("wave", L"../Resource/Patikle/ART_PondParticle01.png"))
    {
        std::cout << "[ERROR] wave 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("smoke", L"../Resource/Patikle/smoke_particle.png"))
    {
        std::cout << "[ERROR] smoke 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("dust", L"../Resource/Dust.png"))
    {
        std::cout << "[ERROR] SETTING_ICON 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("dust2", L"../Resource/Dust2.png"))
    {
        std::cout << "[ERROR] SETTING_ICON 텍스처 로드 실패" << std::endl;
    }

    //if (!AssetManager::Instance().LoadTexture("mouse", L"../Resource/Mouse.png"))
    //{
    //    std::cout << "[ERROR] SETTING_ICON 텍스처 로드 실패" << std::endl;
    //}

    if (!AssetManager::Instance().LoadTexture("spakle", L"../Resource/Spakle.png"))
    {
        std::cout << "[ERROR] SETTING_ICON 텍스처 로드 실패" << std::endl;
    }

    // TEST Resource
    ////////////////////////////////////////////////////////////////////////////////

    if (!AssetManager::Instance().LoadTexture("house", L"../Resource/house.png"))
    {
        std::cout << "[ERROR] house 텍스처 로드 실패" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("house2", L"../Resource/house2.png"))
    {
        std::cout << "[ERROR] house2 텍스처 로드 실패" << std::endl;
    }

    if (!AssetManager::Instance().LoadTexture("Menu", L"../Resource/Menu.png"))
    {
        std::cout << "[ERROR] Menu 텍스처 로드 실패" << std::endl;
    }

    ////////////////////////////////////////////////////////////////////////////////

    if (!AssetManager::Instance().LoadTexture("GrayBird", L"../Resource/Animation/graybirdsheet.png"))
    {
        std::cout << "[ERROR] GrayBird 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadAnimationRender("GrayBird", L"../Resource/Animation/graybirdsheet.json"))
    {
        std::cout << "[ERROR] 애니메이션 로드 실패!" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("mouse", L"../Resource/Patikle/ART_Cursor01.png"))
    {
        std::cout << "[ERROR] mouse 텍스처 로드 실패" << std::endl;
    }
    if (!AssetManager::Instance().LoadTexture("click", L"../Resource/Patikle/ART_Cursor01_Click.png"))
    {
        std::cout << "[ERROR] click 텍스처 로드 실패" << std::endl;
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

