#include "pch.h"
#include "framework.h"
#include "SceneList.h"
#include "BoxCollider.h"
#include "CircleCollider.h"

using namespace std;
using namespace JDGameObject::Content;
using JDComponent::AnimationRender;

namespace JDScene {

    // TestScene
    void TestScene::OnEnter() {
        using namespace JDGameObject;
        using namespace JDComponent;

        //cout << "[TestScene] OnEnter()\n";

        CreateGameObject<Player>(L"Plyaer");

        std::shared_ptr<GameObject> testObject = std::make_shared<GameObject>();
        std::shared_ptr<GameObject>  birdObj = std::make_shared<GameObject>();

        const float startX = -500.0f;
        const float startY = 300.0f;
        const float spacingX = 100.0f;
        const float spacingY = -100.0f;


        for (int row = 0; row < m_totalRows; ++row) {
            for (int col = 0; col < m_totalCols; ++col) {
                // 1) 이름 생성
                std::wstring name = L"Box_" + std::to_wstring(row) + L"_" + std::to_wstring(col);

                // 2) 생성
                auto* box = CreateGameObject<Grid>(name.c_str());

                float x = startX + spacingX * col;
                float y = startY + spacingY * row;
                box->GetComponent<Transform>()->SetPosition({ x, y });
                box->AddComponent<Editor_Clickable>();

                int idx = row * m_totalCols + col;
                auto* collider = box->AddComponent<BoxCollider>(Vector2F{ 47,47 });
                collider->SetIndex(idx);
            }
        }

        auto* circObj = CreateGameObject<Player>(L"CircleObject");
        circObj->GetComponent<Transform>()->SetPosition({ 200.0f, 100.0f });
        circObj->AddComponent<Editor_Clickable>();
        circObj->AddComponent<CircleCollider>(50.0f);

        {//Torch
            auto* torch = CreateGameObject<Player>(L"BoxObject1");
            m_torch = torch;

            torch->SetTag(JDGlobal::Base::GameTag::Mover);
            torch->GetComponent<Transform>()->SetPosition({ -100.0f, 100.0f });
            torch->AddComponent<Editor_Clickable>();

            // 기본 텍스처
            m_torchTex = torch->AddComponent<TextureRenderer>("basic_Torch", RenderLayerInfo{ SortingLayer::BackGround, 1 });
            if (m_torchTex) m_torchTex->SetEnabled(true);

            // 애니메이션(예: "FrogCryAnim" 라는 키로 Asset에 등록되어 있다고 가정)
            m_torchAnim = torch->AddComponent<AnimationRender>("Torch", 1, RenderLayerInfo{ SortingLayer::BackGround, 1 });
            m_torchAnim->SetEnabled(false);

            // 콜라이더
            auto bitmap = static_cast<ID2D1Bitmap*>(AssetManager::Instance().GetTexture("basic_Torch"));
            if (bitmap) {
                auto size = bitmap->GetSize();
                torch->AddComponent<BoxCollider>(Vector2F{ size.width / 2.f, size.height / 2.f });
            }

            m_torchSfx = torch->AddComponent<SFX>("Torch", 0.8f);
            m_torchSfx->SetLoop(true);
            m_torchSfx->SetPlayOnMove(false);
            m_torchSfx->SetEnabled(false);
        }
        {//Frog
            auto* frog = CreateGameObject<Player>(L"BoxObject2");
            m_frog = frog;

            frog->SetTag(JDGlobal::Base::GameTag::Mover);
            frog->GetComponent<Transform>()->SetPosition({ 100.0f, 100.0f });
            frog->AddComponent<Editor_Clickable>();

            // 기본 텍스처
            m_frogTex = frog->AddComponent<TextureRenderer>("basic_Frog", RenderLayerInfo{ SortingLayer::BackGround, 1 });
            if (m_frogTex) m_frogTex->SetEnabled(true);

            // 애니메이션(예: "FrogCryAnim" 라는 키로 Asset에 등록되어 있다고 가정)
            m_frogAnim = frog->AddComponent<AnimationRender>("Frog1", 1, RenderLayerInfo{ SortingLayer::BackGround, 1 });
            m_frogAnim->SetEnabled(false);

            // 콜라이더
            auto bitmap = static_cast<ID2D1Bitmap*>(AssetManager::Instance().GetTexture("basic_Frog"));
            if (bitmap) {
                auto size = bitmap->GetSize();
                frog->AddComponent<BoxCollider>(Vector2F{ size.width / 2.f, size.height / 2.f });
            }

            frog->AddComponent<SFX>("FrogSound");
        }
        {//Rat
            auto* Rat = CreateGameObject<Player>(L"BoxObject2");
            m_rat = Rat;

            Rat->SetTag(JDGlobal::Base::GameTag::Mover);
            Rat->GetComponent<Transform>()->SetPosition({ 0.0f, 100.0f });
            Rat->AddComponent<Editor_Clickable>();

            // 기본 텍스처
            m_ratTex = Rat->AddComponent<TextureRenderer>("basic_Rat", RenderLayerInfo{ SortingLayer::BackGround, 1 });
            if (m_ratTex) m_ratTex->SetEnabled(true);

            // 애니메이션(예: "FrogCryAnim" 라는 키로 Asset에 등록되어 있다고 가정)
            m_ratAnim = Rat->AddComponent<AnimationRender>("Rat", 1, RenderLayerInfo{ SortingLayer::BackGround, 1 });
            m_ratAnim->SetEnabled(false);

            // 콜라이더
            auto bitmap = static_cast<ID2D1Bitmap*>(AssetManager::Instance().GetTexture("basic_Rat"));
            if (bitmap) {
                auto size = bitmap->GetSize();
                Rat->AddComponent<BoxCollider>(Vector2F{ size.width / 2.f, size.height / 2.f });
            }

            Rat->AddComponent<SFX>("RatHide");
        }

        int startCol = 1, startRow = 2;
        int regionW = 3, regionH = 3;

        for (int c = startCol; c < startCol + regionW; ++c) {
            for (int r = startRow; r < startRow + regionH; ++r) {
                int idx = c * m_totalRows + r;
                m_gameObjects[idx]->GetComponent<ColliderBase>()->SetOpen(true);
            }
        }
        
        // UI
        // GameStart 버튼
        auto gameStart = CreateUIObject<Button>(L"GameStart_Button");
        gameStart->SetTextureName("GAME_START_B");
        gameStart->SetSizeToOriginal();

        //파티클 초기화
        m_mouseParticles = std::make_unique<ParticleSystem>(
            D2DRenderer::Instance().GetD2DContext()
        );
        m_leafParticles = std::make_unique<ParticleSystem>(
            D2DRenderer::Instance().GetD2DContext()
        );
        m_smokeParticles = std::make_unique<ParticleSystem>(
            D2DRenderer::Instance().GetD2DContext()
        );
        m_footdustParticles = std::make_unique<ParticleSystem>(
            D2DRenderer::Instance().GetD2DContext()
        );
        m_waveParticles = std::make_unique<ParticleSystem>(
            D2DRenderer::Instance().GetD2DContext()
        );
    }

    void TestScene::OnLeave() {
        //cout << "[TestScene] OnLeave()\n";
    }

    void TestScene::Update(float deltaTime) {
        SceneBase::Update(deltaTime);

        auto cam = D2DRenderer::Instance().GetCamera();
        float W = (float)cam->GetScreenWidth();
        float H = (float)cam->GetScreenHeight();

        float speed = 100.f;  // 픽셀/초
        Vector2F delta{ speed * deltaTime, 0.f };

        {//개구리
            // Anim2 자동 복귀(락 없이)
            if (m_frogChanged && !m_frogLocked) {
                m_frogTimer -= deltaTime;
                if (m_frogTimer <= 0.0f) {
                    if (m_frogAnim) m_frogAnim->SetEnabled(false);
                    if (m_frogTex) { m_frogTex->SetEnabled(true); }
                    m_frogChanged = false;
                }
            }

            // Anim1 락 해제 시 복귀
            if (m_frogLocked) {
                m_frogLockTimer -= deltaTime;
                if (m_frogLockTimer <= 0.0f) {
                    if (m_frogAnim) m_frogAnim->SetEnabled(false);
                    if (m_frogTex)   m_frogTex->SetEnabled(true);
                    m_frogLocked = false;
                }
            }
        }

        {//쥐
            if (m_ratLocked) {
                m_ratLockTimer -= deltaTime;
                if (m_ratLockTimer <= 0.0f) {
                    if (m_ratAnim) m_ratAnim->SetEnabled(false);
                    if (m_ratTex)  m_ratTex->SetEnabled(true);
                    m_ratLocked = false;
                }
            }
        }

        //파티클

        //  나뭇잎
        if (m_leafParticles && m_emitleaf) {
            if (auto* tm = m_rat->GetComponent<JDComponent::Transform>()) {
                Vector2F p = tm->GetPosition();
                m_leafParticles->SpawnLeavesParticle(/*pos*/{ p.x, p.y - 130 },
                    /*count*/1,
                    /*scale*/1.0f,
                    /*maxLife*/4.0f);
            }
        }

        if (m_leafParticles)  m_leafParticles->UpdateLeaves(deltaTime, W, H);
        m_emitleaf = false;
        //  연기
        if (m_emitTorchSmoke && m_smokeParticles) {
            m_torchSmokeAcc += deltaTime;

            if (auto* tm = m_torch->GetComponent<JDComponent::Transform>()) {
                Vector2F p = tm->GetPosition();
                p.y -= 126.f; // 네가 쓰던 오프셋 유지(원하면 조절)
                while (m_torchSmokeAcc >= m_torchSmokePeriod) {
                    m_smokeParticles->SpawnSmokeParticle(p, 1, 1.6f, 2.2f);
                    m_torchSmokeAcc -= m_torchSmokePeriod;
                }
            }
        }
        else {
            m_torchSmokeAcc = 0.f; // 꺼지면 누적 리셋
        }

        if (m_smokeParticles) {
            m_smokeParticles->UpdateSmoke(deltaTime, W, H);
        }
        //  물파동 

        if (m_waveParticles && m_emitwave) {
            if (auto* tm = m_frog->GetComponent<JDComponent::Transform>()) {
                Vector2F p = tm->GetPosition();
                // 시작 스케일, 확산 속도, 수명, 기본 픽셀 크기는 취향대로 조절
                m_waveParticles->SpawnWaterWaveParticle(/*pos*/{ p.x, p.y - 100 },
                    /*startScale*/0.35f,
                    /*expandSpeed*/1.6f,
                    /*maxLife*/0.8f,
                    /*baseSizePx*/64.f);
            }
        }

        if (m_waveParticles)  m_waveParticles->UpdateWaterWave(deltaTime);
        m_emitwave = false;
        //////////////////////////////////////
        for (auto& objPtr : m_gameObjects) {

            auto* sfx = objPtr->GetComponent<JDComponent::SFX>();
            if (!sfx)
                continue;

            auto* tm = objPtr->GetComponent<Transform>();
            if (!tm)
                continue;

            if (objPtr->GetTag() != JDGlobal::Base::GameTag::Mover)
                continue;

            /*Vector2F oldPos = tm->GetPosition();
            Vector2F newPos{ oldPos.x + delta.x, oldPos.y + delta.y };
            tm->SetPosition(newPos);*/

            // 5) 디버그: SFX 가진 녀석인지, 이동 로그 출력
            //if (newPos.x != oldPos.x || newPos.y != oldPos.y)
            //{
            //    sfx->Play();

            //    // 디버그: 소리 재생 로그
            //    /*std::wcout
            //        << L"[DEBUG] SFX Play at mover: key="
            //        << L", from (" << oldPos.x << L"," << oldPos.y
            //        << L") to (" << newPos.x << L"," << newPos.y
            //        << L")\n";*/
            //}
        }

        bool left = InputManager::Instance().GetMouseState().leftClicked;
        bool right = InputManager::Instance().GetMouseState().rightClicked;
        auto mousePos = GetMouseWorldPos();

        for (auto& objPtr : m_gameObjects) {
            auto* collider = objPtr->GetComponent<JDComponent::ColliderBase>();
            if (!collider) continue;
            int id = collider->GetIndex();

            if (!collider->IsMouseOver(mousePos))
                continue;

            if (left) {
                collider->SetOpen(true);
                std::cout << "[DEBUG] left ID: " << id << std::endl;
            }
            else if (right && collider->IsOpen()) {
                std::cout << "[DEBUG] right ID: " << id << std::endl;
                // 우클릭: 열린 콜라이더만 동작
            }
        }

        ClickUpdate();
    }

    void TestScene::FixedUpdate(float fixedDeltaTime) {
        SceneBase::FixedUpdate(fixedDeltaTime);
        //cout << "[TestScene] FixedUpdate() - dt: " << fixedDeltaTime << "\n";
    }

    void TestScene::LateUpdate(float deltaTime) {
        SceneBase::LateUpdate(deltaTime);
        //cout << "[TestScene] LateUpdate() - dt: " << deltaTime << "\n";
    }

    void TestScene::Render(float deltaTime) {
        SceneBase::Render(deltaTime);
        auto camera = D2DRenderer::Instance().GetCamera();

        if (camera)
        {
            D2DRenderer::Instance().SetTransform(camera->GetViewMatrix());
        }
        else
        {
            D2DRenderer::Instance().SetTransform(D2D1::Matrix3x2F::Identity());
        }

        for (auto& obj : m_gameObjects)
        {
            D2DRenderer::Instance().RenderGameObject(*obj, deltaTime);
        }

        for (auto& uiObj : m_uiObjects)
        {
            D2DRenderer::Instance().RenderUIObject(*uiObj);
        }

        auto ctx = D2DRenderer::Instance().GetD2DContext();

        if (m_leafParticles) m_leafParticles->RenderLeaves(ctx);
        if (m_smokeParticles) m_smokeParticles->RenderSmoke(ctx);
        if (m_waveParticles) m_waveParticles->RenderWaterWave(ctx);

        //if (m_mouseParticles) m_mouseParticles->RenderGlow(ctx);
        
    }
    void TestScene::ClickUpdate()
    {
        auto camera = D2DRenderer::Instance().GetCamera();
        if (!camera) return;

        InputManager& input = InputManager::Instance();
        MouseState state = input.GetMouseState();

        if (ImGui::GetIO().WantCaptureMouse)
            return;

        if (state.leftClicked)
        {
            std::cout << "이거 실행됨?";

            // 1. 마우스 스크린 좌표
            float screenHeight = static_cast<float>(camera.get()->GetScreenHeight());

            Vector2F mousePos(
                static_cast<float>(state.pos.x),
                static_cast<float>(state.pos.y)
            );

            //디버그용 출력문
            std::cout << "마우스: (" << mousePos.x << ", " << mousePos.y << ")" << std::endl;
            std::cout << "화면 높이: " << screenHeight << std::endl;


            // 클릭 여부
            bool clicked = false;

            ////////////////////////////////////////////////////////////////////////////////
            // 1. UI 클릭 검사 ( 스크린 좌표계 사용 )
            ////////////////////////////////////////////////////////////////////////////////

            for (int i = static_cast<int>(m_uiObjects.size()) - 1; i >= 0; --i)
            {
                auto& uiObj = m_uiObjects[i];
                if (!uiObj) continue;

                auto clickable = uiObj->GetComponent<Editor_Clickable>();
                if (clickable && clickable->IsHit(mousePos))
                {
                    SetSelectedObject(uiObj.get());
                    clicked = true;
                    std::cout << " UI 오브젝트 클릭 함!!!!! ";
                    break;
                }
            }

            ////////////////////////////////////////////////////////////////////////////////
            // 2. 게임오브젝트 클릭 검사 ( 월드 좌표계 사용 )
            ////////////////////////////////////////////////////////////////////////////////

            if (!clicked)
            {
                // 게임오브젝트용으로만 Unity 좌표 변환
                //Vector2F unityMousePos(mousePos.x, screenHeight - mousePos.y);
                // 스크린 좌표를 월드 좌표로 변환
                //Vector2F worldMousePos = camera->ScreenToWorldPoint(unityMousePos);
                Vector2F worldMousePos = camera->ScreenToWorldPoint(mousePos);

                //디버그용 출력문
                //std::cout << "Unity 마우스: (" << unityMousePos.x << ", " << unityMousePos.y << ")" << std::endl;
                std::cout << "월드 마우스: (" << worldMousePos.x << ", " << worldMousePos.y << ")" << std::endl;

                if (m_frog)
                {
                    if (auto* frogCol = m_frog->GetComponent<JDComponent::BoxCollider>())
                    {
                        if (frogCol->IsMouseOver(worldMousePos))
                        {
                            if (m_frogLocked) return; // 락 중엔 무시

                            SetSelectedObject(m_frog);
                            clicked = true;

                            std::uniform_real_distribution<float> d01(0.0f, 1.0f);
                            bool useAnim1 = (d01(rng) < 0.2f);     // 20% 확률로 Frog1
                            const char* clipName = useAnim1 ? "Frog1" : "Frog2";

                            // 텍스처 끄고 애니로 전환
                            if (m_frogTex)  m_frogTex->SetEnabled(false);
                            if (m_frogAnim)
                            {
                                m_frogAnim->SetClipName(clipName); // 클립만 교체
                                m_frogAnim->SetEnabled(true);
                                m_frogAnim->PlayOnceFromStart();
                            }

                            if (auto* sfx = m_frog->GetComponent<JDComponent::SFX>()) {
                                sfx->SetClipName(useAnim1 ? "FrogPop" : "FrogSound");  // ← 키만 교체
                                sfx->Play();                                           // ← 재생
                            }

                            if (useAnim1)
                            {//FlogPop
                                //  Anim1 사운드 재생
                                if (auto* sfx = m_frog->GetComponent<JDComponent::SFX>()) {
                                    sfx->Play(); // 필요하면 Play(volume, pitch) 등으로 확장
                                }
                                // Anim1일 때만 락 3~5초
                                std::uniform_real_distribution<float> dLock(3.0f, 5.0f);
                                m_frogLockTimer = dLock(rng);
                                m_frogLocked = true;
                            }
                            else
                            {
                                //  Anim2 사운드 재생
                                if (auto* sfx = m_frog->GetComponent<JDComponent::SFX>()) {
                                    sfx->Play(); // 필요하면 Play(volume, pitch) 등으로 확장
                                }
                                // Anim2는 락 없이 자동 복귀(클립 총길이, 없으면 1초)
                                float dur = 1.0f;
                                
                                if (auto* clip = AssetManager::Instance().GetAnimationRender("Frog2")) {
                                    dur = 0.0f;
                                    for (const auto& f : clip->frames) dur += f.duration;
                                    if (dur <= 0.0f) dur = 1.0f;
                                }
                                m_frogTimer = dur;
                                m_frogChanged = true; // Update에서 복귀 처리
                            }

                            m_emitwave = true;
                        }
                    }
                }

                if (m_rat)
                {
                    if (auto* ratCol = m_rat->GetComponent<JDComponent::BoxCollider>())
                    {
                        if (ratCol->IsMouseOver(worldMousePos))
                        {
                            if (m_ratLocked) return; // 락 중엔 무시

                            SetSelectedObject(m_rat);
                            clicked = true;
                                               
                            if (m_ratTex)  m_ratTex->SetEnabled(false);
                            if (m_ratAnim)
                            {
                                m_ratAnim->SetClipName("Rat");   // 클립 하나라면 생략해도 OK
                                m_ratAnim->SetEnabled(true);
                                m_ratAnim->PlayOnceFromStart();
                            }

                            // 사운드 재생(항상 동일)
                            if (auto* sfx = m_rat->GetComponent<JDComponent::SFX>()) {
                                sfx->Play();
                            }

                            std::uniform_real_distribution<float> dLock(2.0f, 3.0f); // 예: 2~3초
                            m_ratLockTimer = dLock(rng);
                            m_ratLocked = true;

                            m_emitleaf = true;
                        }
                    }
                }

                if (m_torch)
                {
                    if (auto* torchCol = m_torch->GetComponent<JDComponent::BoxCollider>())
                    {
                        if (torchCol->IsMouseOver(worldMousePos))
                        {
                            SetSelectedObject(m_torch);
                            clicked = true;

                            if (m_torchAnim && m_torchAnim->GetEnabled())
                            {
                                m_emitTorchSmoke = false;
                                // 애니 끄고 텍스처 복귀
                                m_torchAnim->SetEnabled(false);
                                if (m_torchTex) m_torchTex->SetEnabled(true);

                                // 사운드 끔
                                if (m_torchSfx) { m_torchSfx->Stop(); m_torchSfx->SetEnabled(false); }
                            }
                            else
                            {
                                m_emitTorchSmoke = true;
                                // 텍스처 숨기고 애니 시작(2프레임부터 루프)
                                if (m_torchTex)  m_torchTex->SetEnabled(false);
                                if (m_torchAnim) {
                                    m_torchAnim->SetEnabled(true);
                                    m_torchAnim->PlayOnceThenLoopRange(1, UINT_MAX);
                                }

                                // 사운드 루프 재생
                                if (m_torchSfx) {
                                    m_torchSfx->SetEnabled(true);
                                    m_torchSfx->SetLoop(true); // 혹시나 초기화 누락 대비
                                    if (!m_torchSfx->IsPlaying()) m_torchSfx->Play();
                                }
                            }

                            auto* sfx = m_torch->GetComponent<JDComponent::SFX>();
                            std::cout << "[Torch] sfx=" << sfx
                                << " enabled=" << (sfx ? sfx->GetEnabled() : false)
                                << " playing=" << (sfx ? sfx->IsPlaying() : false) << "\n";
                        }
                    }
                }
                if (!clicked)
                {
                    for (int i = static_cast<int>(m_gameObjects.size()) - 1; i >= 0; --i)
                    {
                        auto& obj = m_gameObjects[i];
                        if (!obj) continue;

                        // 클릭러블이 아닌, 실제 콜라이더 기준 검사로 바꾸고 싶다면 이렇게:
                        // if (auto* col = obj->GetComponent<JDComponent::ColliderBase>())
                        // {
                        //     if (col->IsMouseOver(worldMousePos)) { ... }
                        // }

                        auto clickable = obj->GetComponent<Editor_Clickable>();
                        if (clickable && clickable->IsHit(worldMousePos))
                        {
                            SetSelectedObject(obj.get());
                            clicked = true;
                            std::cout << " 게임 오브젝트 클릭 함!!!!! ";
                            break;
                        }
                    }
                }
            }

            if (!clicked)
            {
                SetSelectedObject(nullptr);
            }
        }
    }
}
