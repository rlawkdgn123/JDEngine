#include "pch.h"
#include "framework.h"
#include "House.h"
#include "Building.h"
#include "ResourceSystem.h"
#include "SceneBase.h"
#include "BoxCollider.h"
#include "Grid.h"

using namespace std;
using Vec2 = JDComponent::D2DTM::Transform::Vec2;
namespace JDGameObject {
    namespace Content {

        void Grid::Awake()
        {
            __super::Awake();
            m_resourceSystem = &ResourceSystem::Instance();
        }

        void Grid::Start() {
            __super::Start();
        }

        void Grid::Update(float deltaTime) {
            if (!m_buildingRaw || !m_hasBuilding) { ChangeCatType(CatType::None); return; } // 건물이 없으면 리턴

            // 시너지 처리를 한 후 고양이를 업데이트
            if (auto building = dynamic_cast<Building*>(m_buildingRaw)) {
                if (m_cat == building->GetCatType()) return;  // 이전 고양이랑 같으면
                else {
                    ChangeCatType(building->GetCatType());
                    ChangeCatTexture();
                }
            }
        }

        void Grid::LateUpdate(float deltaTime) {
            __super::LateUpdate(deltaTime);

            // 레벨 연동
            if (m_levelTextureObj && m_levelTextureObj->IsActive() && m_buildingRaw) {
                if (auto* textRenderer = m_levelTextureObj->GetComponent<JDComponent::TextRenderer>()) {
                    int level = -1;

                    if (auto* house = dynamic_cast<House*>(m_buildingRaw)) {
                        level = house->GetLevel();
                    }
                    else if (auto* building = dynamic_cast<Building*>(m_buildingRaw)) {
                        level = building->GetLevel();
                    }

                    if (level >= 0) {
                        textRenderer->SetText(to_wstring(level+1));
                    }
                    /*else if (level+1 == JDGlobal::Contents::MAX_GAME_LEVEL) {
                        textRenderer->SetText(L"MAX");
                    }*/
                }
            }
        }
        void Grid::FixedUpdate(float fixedDeltaTime) {}
        void Grid::SetBuilding(JDGameObject::GameObject* building)
        {
            m_buildingRaw = dynamic_cast<JDGameObject::GameObject*>(building);
        }

        // 고양이를 바꾸기
        void Grid::ChangeCatType(CatType type) {

            UpdateSynergy(); // 로직 전 기존 고양이 시너지 업데이트

            if (m_cat == type) return;

            CatType preType = m_cat; // 기존 고양이 변수

            m_resourceSystem->AddSynergyBonus(-m_synergyBonus); // 기존 시너지 제거

            m_cat = type; // 새 고양이 할당
            UpdateSynergy(); // 새 고양이에 대한 보너스 재할당

        }

        // 고양이 보너스를 주변 타일 고양이 개수와 합해서 계산해주기
        void Grid::UpdateSynergy()
        {
            Resource newBonus{ 0, 0, 0 };
            SynergyBonus synergyData;

            for (Grid* other : others) {
                if (!other || !other->HasBuilding()) continue;

                auto building = dynamic_cast<Building*>(other->GetBuilding());
                if (!building) continue;

                auto catType = building->GetCatType();

                switch (m_cat) {
                case CatType::Felis:
                    if (catType == CatType::Kone) newBonus += synergyData.FelisBonus;
                    else if (catType == CatType::Navi) newBonus += synergyData.FelisPenalty;
                    break;
                case CatType::Navi:
                    if (catType == CatType::Felis) newBonus += synergyData.NaviBonus;
                    else if (catType == CatType::Kone) newBonus += synergyData.NaviPenalty;
                    break;
                case CatType::Kone:
                    if (catType == CatType::Navi) newBonus += synergyData.KoneBonus;
                    else if (catType == CatType::Felis) newBonus += synergyData.KonePenalty;
                    break;
                default:
                    break;
                }
            }

            // 변화량 계산
            Resource diff = newBonus - m_synergyBonus;

            // 리소스 매니저에 변화량 반영
            m_resourceSystem->AddSynergyBonus(diff);

            // m_synergyBonus는 항상 현재 이 Grid의 전체 시너지 값 유지
            m_synergyBonus = newBonus;
        }

        bool Grid::CreateTextureObj(SceneBase& scene)
        {
            // 그리드의 위치와 크기값 가져오기
            // 콜라이더 크기값
            auto tr = this->GetComponent<JDComponent::Transform>();
            if (!tr) { cout << "[Grid::CreateTextureObj] Transform 컴포넌트 없음!!" << endl; return false; }
            auto boxCol = this->GetComponent<JDComponent::BoxCollider>();
            if (!boxCol) { cout << "[Grid::CreateTextureObj] BoxCollider 컴포넌트 없음!!" << endl; return false; }
            
            Vec2 bottom_Left{ (tr->GetWorldPosition().x - boxCol->GetQuarterSize().x), tr->GetWorldPosition().y - boxCol->GetQuarterSize().y };
            Vec2 bottom_Right{ (tr->GetWorldPosition().x + boxCol->GetQuarterSize().x), tr->GetWorldPosition().y - boxCol->GetQuarterSize().y };
            /////
            // 고양이 텍스쳐
            m_catTextureObj = scene.CreateGameObject<GameObject>((L"GameUI_Cat"+this->GetName()));
            m_catTextureObj->SetTag(JDGameObject::GameObject::Tag::UI);
            m_catTextureObj->AddComponent<Editor_Clickable>();
            m_catTextureObj->AddComponent<JDComponent::TextureRenderer>("ART_Stiker01", RenderLayerInfo{ SortingLayer::Grid, 1 });

            // TODO : 그리드 계산해서 우하단으로 맞추기, + 사이즈
            m_catTextureObj->GetComponent<JDComponent::Transform>()->SetPosition(bottom_Right + Vec2{ 4.8f,-13.f });
            m_catTextureObj->GetComponent<JDComponent::TextureRenderer>()->SetSize({ boxCol->GetQuarterSize().x, boxCol->GetQuarterSize().y });
            m_catTextureObj->SetActive(false);

            if (!m_catTextureObj) return false;

            /////
            // 건물 레벨 텍스처
            m_levelTextureObj = scene.CreateGameObject<GameObject>((L"GameUI_LevelTexture" + this->GetName()));
            m_levelTextureObj->SetTag(JDGameObject::GameObject::Tag::UI);
            m_levelTextureObj->AddComponent<Editor_Clickable>();
            m_levelTextureObj->AddComponent<JDComponent::TextureRenderer>("ART_Building_Level", RenderLayerInfo{ SortingLayer::Grid, 2 });
            m_levelTextureObj->AddComponent<JDComponent::TextRenderer>(
                L"1",
                D2D1::SizeF(100.f, 100.f),
                RenderLayerInfo{ SortingLayer::Grid, 2 }
            );
            m_levelTextureObj->GetComponent<JDComponent::TextRenderer>()->SetTextFormatName("Sebang_12");

            // TODO : 그리드 계산해서 우하단으로 맞추기, + 사이즈
            m_levelTextureObj->GetComponent<JDComponent::Transform>()->SetPosition(bottom_Left + Vec2{-16.6f,-17.1f});
            m_levelTextureObj->GetComponent<JDComponent::TextureRenderer>()->SetSize({ boxCol->GetQuarterSize().x, boxCol->GetQuarterSize().y });
            m_levelTextureObj->GetComponent<JDComponent::TextRenderer>()->SetText(L"1");
            m_levelTextureObj->SetActive(false);

            if (!m_levelTextureObj) return false;

            /////
            // 인접효과 화살표
            m_synergyEffectTextureObj = scene.CreateGameObject<GameObject>((L"GameUI_SynergyEffect" + this->GetName()));
            m_synergyEffectTextureObj->SetTag(JDGameObject::GameObject::Tag::UI);
            m_synergyEffectTextureObj->AddComponent<Editor_Clickable>();
            m_synergyEffectTextureObj->AddComponent<JDComponent::TextureRenderer>("ART_Up5", RenderLayerInfo{ SortingLayer::Grid, 3 });

            // TODO : 그리드 계산해서 우하단으로 맞추기, + 사이즈
            m_synergyEffectTextureObj->GetComponent<JDComponent::Transform>()->SetPosition(bottom_Right+ Vec2{14.7f, -14.3f});
            m_synergyEffectTextureObj->GetComponent<JDComponent::TextureRenderer>()->SetSize({ boxCol->GetQuarterSize().x + 0.5f, boxCol->GetQuarterSize().y + 0.5f });
            m_synergyEffectTextureObj->SetActive(false);

            if (!m_synergyEffectTextureObj) return false;

            /////
            // 그리드 선택 텍스쳐
            m_selectEffectTextureObj = scene.CreateGameObject<GameObject>((L"GameUI_SelectEffect" + this->GetName()));
            m_selectEffectTextureObj->SetTag(JDGameObject::GameObject::Tag::UI);
            m_selectEffectTextureObj->AddComponent<Editor_Clickable>();
            m_selectEffectTextureObj->AddComponent<JDComponent::TextureRenderer>("Tile_Stroke01", RenderLayerInfo{ SortingLayer::Grid, 4 });

            // TODO : 그리드 계산해서 우하단으로 맞추기, + 사이즈
            m_selectEffectTextureObj->GetComponent<JDComponent::Transform>()->SetPosition(tr->GetWorldPosition());
            m_selectEffectTextureObj->GetComponent<JDComponent::TextureRenderer>()->SetSize({ boxCol->GetSize().x * 1.03f, boxCol->GetSize().y * 1.03f });
            m_selectEffectTextureObj->SetActive(false);
            
            if (!m_selectEffectTextureObj) return false;

            /////
            // 그리드 확장 가능 텍스쳐
            m_expendEffectTextureObj;
            m_expendEffectTextureObj = scene.CreateGameObject<GameObject>((L"GameUI_ExpendEffect" + this->GetName()));
            m_expendEffectTextureObj->SetTag(JDGameObject::GameObject::Tag::UI);
            m_expendEffectTextureObj->AddComponent<Editor_Clickable>();
            m_expendEffectTextureObj->AddComponent<JDComponent::TextureRenderer>("Tile_Stroke01", RenderLayerInfo{ SortingLayer::Grid, 5 });

            // TODO : 그리드 계산해서 우하단으로 맞추기, + 사이즈
            m_expendEffectTextureObj->GetComponent<JDComponent::Transform>()->SetPosition(tr->GetWorldPosition());
            m_expendEffectTextureObj->GetComponent<JDComponent::TextureRenderer>()->SetSize({ boxCol->GetSize().x * 1.03f, boxCol->GetSize().y * 1.03f });
            m_expendEffectTextureObj->SetActive(false);

            if (!m_expendEffectTextureObj) return false;

            return true;
        }

        void Grid::DeleteTextureObj(SceneBase& scene)
        {
            scene.DestroyObject(m_catTextureObj);
            scene.DestroyObject(m_levelTextureObj);
            scene.DestroyObject(m_synergyEffectTextureObj);
            scene.DestroyObject(m_selectEffectTextureObj);
            scene.DestroyObject(m_expendEffectTextureObj);
        }

        void Grid::ChangeCatTexture()
        {
            if (!m_catTextureObj || !m_buildingRaw) return; // 텍스처 Obj 미소유 or 빌딩 미소유면

            auto* building = dynamic_cast<Building*>(m_buildingRaw);
            
            if (!building) { cout << "[Error] 빌딩이 아닙니다." << endl; return; }

            switch (building->GetCatType()) {
            case CatType::Felis: m_catTextureObj->GetComponent<JDComponent::TextureRenderer>()->SetTextureName("ART_Stiker01"); break;
            case CatType::Navi: m_catTextureObj->GetComponent<JDComponent::TextureRenderer>()->SetTextureName("ART_Stiker02"); break;
            case CatType::Kone: m_catTextureObj->GetComponent<JDComponent::TextureRenderer>()->SetTextureName("ART_Stiker03"); break;
            }
        }

        void Grid::ChangeSynergyEffectTexture()
        {
            if (!m_synergyEffectTextureObj || !m_buildingRaw) return; // 텍스처 Obj 미소유 or 빌딩 미소유면

            auto* building = dynamic_cast<Building*>(m_buildingRaw);

            if (!building) { cout << "[Error] 빌딩이 아닙니다." << endl; return; }

            switch (building->GetCatType()) {
            case CatType::Felis: m_catTextureObj->GetComponent<JDComponent::TextureRenderer>()->SetTextureName("ART_Stiker01"); break;
            case CatType::Navi: m_catTextureObj->GetComponent<JDComponent::TextureRenderer>()->SetTextureName("ART_Stiker02"); break;
            case CatType::Kone: m_catTextureObj->GetComponent<JDComponent::TextureRenderer>()->SetTextureName("ART_Stiker03"); break;
            }

            m_synergyEffectTextureObj->GetComponent<JDComponent::TextureRenderer>()->SetTextureName("ART_Down5");
            m_synergyEffectTextureObj->GetComponent<JDComponent::TextureRenderer>()->SetTextureName("ART_Down10");
            m_synergyEffectTextureObj->GetComponent<JDComponent::TextureRenderer>()->SetTextureName("ART_Up5");
            m_synergyEffectTextureObj->GetComponent<JDComponent::TextureRenderer>()->SetTextureName("ART_Up10");
        }

    }
}