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
            if (!m_buildingRaw || !m_hasBuilding) { ChangeCatType(CatType::None); return; } // �ǹ��� ������ ����

            // �ó��� ó���� �� �� ����̸� ������Ʈ
            if (auto building = dynamic_cast<Building*>(m_buildingRaw)) {
                if (m_cat == building->GetCatType()) return;  // ���� ����̶� ������
                else {
                    ChangeCatType(building->GetCatType());
                    ChangeCatTexture();
                }
            }
        }

        void Grid::LateUpdate(float deltaTime) {
            __super::LateUpdate(deltaTime);

            // ���� ����
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

        // ����̸� �ٲٱ�
        void Grid::ChangeCatType(CatType type) {

            UpdateSynergy(); // ���� �� ���� ����� �ó��� ������Ʈ

            if (m_cat == type) return;

            CatType preType = m_cat; // ���� ����� ����

            m_resourceSystem->AddSynergyBonus(-m_synergyBonus); // ���� �ó��� ����

            m_cat = type; // �� ����� �Ҵ�
            UpdateSynergy(); // �� ����̿� ���� ���ʽ� ���Ҵ�

        }

        // ����� ���ʽ��� �ֺ� Ÿ�� ����� ������ ���ؼ� ������ֱ�
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

            // ��ȭ�� ���
            Resource diff = newBonus - m_synergyBonus;

            // ���ҽ� �Ŵ����� ��ȭ�� �ݿ�
            m_resourceSystem->AddSynergyBonus(diff);

            // m_synergyBonus�� �׻� ���� �� Grid�� ��ü �ó��� �� ����
            m_synergyBonus = newBonus;
        }

        bool Grid::CreateTextureObj(SceneBase& scene)
        {
            // �׸����� ��ġ�� ũ�Ⱚ ��������
            // �ݶ��̴� ũ�Ⱚ
            auto tr = this->GetComponent<JDComponent::Transform>();
            if (!tr) { cout << "[Grid::CreateTextureObj] Transform ������Ʈ ����!!" << endl; return false; }
            auto boxCol = this->GetComponent<JDComponent::BoxCollider>();
            if (!boxCol) { cout << "[Grid::CreateTextureObj] BoxCollider ������Ʈ ����!!" << endl; return false; }
            
            Vec2 bottom_Left{ (tr->GetWorldPosition().x - boxCol->GetQuarterSize().x), tr->GetWorldPosition().y - boxCol->GetQuarterSize().y };
            Vec2 bottom_Right{ (tr->GetWorldPosition().x + boxCol->GetQuarterSize().x), tr->GetWorldPosition().y - boxCol->GetQuarterSize().y };
            /////
            // ����� �ؽ���
            m_catTextureObj = scene.CreateGameObject<GameObject>((L"GameUI_Cat"+this->GetName()));
            m_catTextureObj->SetTag(JDGameObject::GameObject::Tag::UI);
            m_catTextureObj->AddComponent<Editor_Clickable>();
            m_catTextureObj->AddComponent<JDComponent::TextureRenderer>("ART_Stiker01", RenderLayerInfo{ SortingLayer::Grid, 1 });

            // TODO : �׸��� ����ؼ� ���ϴ����� ���߱�, + ������
            m_catTextureObj->GetComponent<JDComponent::Transform>()->SetPosition(bottom_Right + Vec2{ 4.8f,-13.f });
            m_catTextureObj->GetComponent<JDComponent::TextureRenderer>()->SetSize({ boxCol->GetQuarterSize().x, boxCol->GetQuarterSize().y });
            m_catTextureObj->SetActive(false);

            if (!m_catTextureObj) return false;

            /////
            // �ǹ� ���� �ؽ�ó
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

            // TODO : �׸��� ����ؼ� ���ϴ����� ���߱�, + ������
            m_levelTextureObj->GetComponent<JDComponent::Transform>()->SetPosition(bottom_Left + Vec2{-16.6f,-17.1f});
            m_levelTextureObj->GetComponent<JDComponent::TextureRenderer>()->SetSize({ boxCol->GetQuarterSize().x, boxCol->GetQuarterSize().y });
            m_levelTextureObj->GetComponent<JDComponent::TextRenderer>()->SetText(L"1");
            m_levelTextureObj->SetActive(false);

            if (!m_levelTextureObj) return false;

            /////
            // ����ȿ�� ȭ��ǥ
            m_synergyEffectTextureObj = scene.CreateGameObject<GameObject>((L"GameUI_SynergyEffect" + this->GetName()));
            m_synergyEffectTextureObj->SetTag(JDGameObject::GameObject::Tag::UI);
            m_synergyEffectTextureObj->AddComponent<Editor_Clickable>();
            m_synergyEffectTextureObj->AddComponent<JDComponent::TextureRenderer>("ART_Up5", RenderLayerInfo{ SortingLayer::Grid, 3 });

            // TODO : �׸��� ����ؼ� ���ϴ����� ���߱�, + ������
            m_synergyEffectTextureObj->GetComponent<JDComponent::Transform>()->SetPosition(bottom_Right+ Vec2{14.7f, -14.3f});
            m_synergyEffectTextureObj->GetComponent<JDComponent::TextureRenderer>()->SetSize({ boxCol->GetQuarterSize().x + 0.5f, boxCol->GetQuarterSize().y + 0.5f });
            m_synergyEffectTextureObj->SetActive(false);

            if (!m_synergyEffectTextureObj) return false;

            /////
            // �׸��� ���� �ؽ���
            m_selectEffectTextureObj = scene.CreateGameObject<GameObject>((L"GameUI_SelectEffect" + this->GetName()));
            m_selectEffectTextureObj->SetTag(JDGameObject::GameObject::Tag::UI);
            m_selectEffectTextureObj->AddComponent<Editor_Clickable>();
            m_selectEffectTextureObj->AddComponent<JDComponent::TextureRenderer>("Tile_Stroke01", RenderLayerInfo{ SortingLayer::Grid, 4 });

            // TODO : �׸��� ����ؼ� ���ϴ����� ���߱�, + ������
            m_selectEffectTextureObj->GetComponent<JDComponent::Transform>()->SetPosition(tr->GetWorldPosition());
            m_selectEffectTextureObj->GetComponent<JDComponent::TextureRenderer>()->SetSize({ boxCol->GetSize().x * 1.03f, boxCol->GetSize().y * 1.03f });
            m_selectEffectTextureObj->SetActive(false);
            
            if (!m_selectEffectTextureObj) return false;

            /////
            // �׸��� Ȯ�� ���� �ؽ���
            m_expendEffectTextureObj;
            m_expendEffectTextureObj = scene.CreateGameObject<GameObject>((L"GameUI_ExpendEffect" + this->GetName()));
            m_expendEffectTextureObj->SetTag(JDGameObject::GameObject::Tag::UI);
            m_expendEffectTextureObj->AddComponent<Editor_Clickable>();
            m_expendEffectTextureObj->AddComponent<JDComponent::TextureRenderer>("Tile_Stroke01", RenderLayerInfo{ SortingLayer::Grid, 5 });

            // TODO : �׸��� ����ؼ� ���ϴ����� ���߱�, + ������
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
            if (!m_catTextureObj || !m_buildingRaw) return; // �ؽ�ó Obj �̼��� or ���� �̼�����

            auto* building = dynamic_cast<Building*>(m_buildingRaw);
            
            if (!building) { cout << "[Error] ������ �ƴմϴ�." << endl; return; }

            switch (building->GetCatType()) {
            case CatType::Felis: m_catTextureObj->GetComponent<JDComponent::TextureRenderer>()->SetTextureName("ART_Stiker01"); break;
            case CatType::Navi: m_catTextureObj->GetComponent<JDComponent::TextureRenderer>()->SetTextureName("ART_Stiker02"); break;
            case CatType::Kone: m_catTextureObj->GetComponent<JDComponent::TextureRenderer>()->SetTextureName("ART_Stiker03"); break;
            }
        }

        void Grid::ChangeSynergyEffectTexture()
        {
            if (!m_synergyEffectTextureObj || !m_buildingRaw) return; // �ؽ�ó Obj �̼��� or ���� �̼�����

            auto* building = dynamic_cast<Building*>(m_buildingRaw);

            if (!building) { cout << "[Error] ������ �ƴմϴ�." << endl; return; }

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