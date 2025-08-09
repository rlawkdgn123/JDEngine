#include "pch.h"
#include "framework.h"
using namespace JDComponent;
using ResourceSystem = JDGameSystem::ResourceSystem;
using namespace std;
namespace JDGameObject {
    namespace Content {
        void Building::Awake()
        {
            __super::Awake();
            m_resourceSystem = &ResourceSystem::Instance();
        }

        void Building::Start()
        {
            __super::Start();
            // ��ġ �ڿ��� ����
            m_resourceSystem->AddTotalResource(-stats.m_upgradeCost[m_curLevel]);
            wcout << this->GetName() + L" �Ǽ� �� �ڿ�(Wood) �Ҹ� : " + to_wstring(stats.m_upgradeCost[m_curLevel].m_wood) << endl;

            // �ʴ� �ڿ� ���/�Ҹ� ���ϱ�
            m_resourceSystem->AddResourcePerSec(stats.m_resourceGenPerSec[m_curLevel]);
            m_resourceSystem->AddResourcePerSec(stats.m_resourceSubPerSec[m_curLevel]);
        }
        void Building::Update(float deltaTime)
        {
            __super::Update(deltaTime);
        }
        void Building::ChangeCat(CatType catType)
        {
            if (m_cat == catType) return;

            auto bonus = JDGlobal::Contents::CatResourceBonus();

            switch (m_cat) {
            case CatType::Felis:
                m_resourceSystem->AddResourceBonus(-bonus.FelisBonus);
                break;
            case CatType::Navi:
                m_resourceSystem->AddResourceBonus(-bonus.NaviBonus);
                break;
            case CatType::Kone:
                m_resourceSystem->AddResourceBonus(-bonus.KoneBonus);
                break;
            default:
                break;
            }
            
            switch (catType) {
            case CatType::Felis:
                m_resourceSystem->AddResourceBonus(bonus.FelisBonus);
                break;
            case CatType::Navi:
                m_resourceSystem->AddResourceBonus(bonus.NaviBonus);
                break;
            case CatType::Kone:
                m_resourceSystem->AddResourceBonus(bonus.KoneBonus);
                break;
            default:
                break;
            }

            m_cat = catType;
        }

        // �ش� �ڵ忡���� return false : ����Ѵٴ� ��
        bool Building::LevelUp()
        {
            if (m_curLevel >= JDGlobal::Contents::MAX_GAME_LEVEL - 1) { cout << "���� �ִ�ġ!!" << endl; return false; }
            
            if (m_resourceSystem->GetTotalResource().m_food < stats.m_upgradeCost[m_curLevel + 1].m_food) { cout << "���� �� ����! ���׷��̵� �ڿ��� ���ڶ��ϴ�! - food" << endl; return false; }
            else if (m_resourceSystem->GetTotalResource().m_wood < stats.m_upgradeCost[m_curLevel + 1].m_wood) { cout << "���� �� ����! ���׷��̵� �ڿ��� ���ڶ��ϴ�! - wood" << endl; return false; }
            else if (m_resourceSystem->GetTotalResource().m_mineral < stats.m_upgradeCost[m_curLevel + 1].m_mineral) { cout << "���� �� ����! ���׷��̵� �ڿ��� ���ڶ��ϴ�! - mineral" << endl; return false; }

            // ������ �ڿ� �Ҹ�
           m_resourceSystem->AddTotalResource(-stats.m_upgradeCost[m_curLevel + 1]);

            wcout << this->GetName()
                << L" - Level Up!! " << (m_curLevel + 1)
                << L" -> " << (m_curLevel + 2) << endl;

            // �ʴ� �ڿ� ���/�Ҹ� ���ϱ�
            m_resourceSystem->AddResourcePerSec(-stats.m_resourceGenPerSec[m_curLevel]);
            m_resourceSystem->AddResourcePerSec(-stats.m_resourceSubPerSec[m_curLevel]);
            ++m_curLevel;
            m_resourceSystem->AddResourcePerSec(stats.m_resourceGenPerSec[m_curLevel]);
            m_resourceSystem->AddResourcePerSec(stats.m_resourceSubPerSec[m_curLevel]);
            
            return true;
        }

        // �ش� �ڵ忡���� return false : �ı��Ѵٴ� ��
        bool Building::LevelDown()
        {
            if (m_curLevel == 0) { cout << "�̹� ���� ����!! �ı��մϴ�!!" << endl; return false; } // ����ó���� ��ư �ʿ���

            // �ʴ� �ڿ� ����� ���ϱ�
            m_resourceSystem->AddResourcePerSec(-stats.m_resourceGenPerSec[m_curLevel]);
            m_resourceSystem->AddResourcePerSec(-stats.m_resourceSubPerSec[m_curLevel]);
            --m_curLevel;
            // �ʴ� �ڿ� ����� ���ϱ�
            m_resourceSystem->AddResourcePerSec(stats.m_resourceGenPerSec[m_curLevel]);
            m_resourceSystem->AddResourcePerSec(stats.m_resourceSubPerSec[m_curLevel]);

            return true;
        }
    }
}