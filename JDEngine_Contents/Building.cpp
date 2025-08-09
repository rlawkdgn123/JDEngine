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
            // ������ �� ����̰� �ִٸ�, ����� �ڿ� ���ʽ� �ֱ�
            ChangeCat(m_cat);
        }
        void Building::Update(float deltaTime)
        {
            __super::Update(deltaTime);
        }

        bool Building::ChangeCat(CatType newCat)
        {
            // ���� Ÿ���̸� �ƹ� �͵� ���� ����(����)
            if (m_cat == newCat) return true;

            auto bonus = JDGlobal::Contents::CatResourceBonus();
            CatType oldCat = m_cat;

            // 1) ���� ����� ���ʽ� ���� (�־��ٸ�)
            switch (oldCat) {
            case CatType::Felis: m_resourceSystem->AddResourceBonus(-bonus.FelisBonus); break;
            case CatType::Navi:  m_resourceSystem->AddResourceBonus(-bonus.NaviBonus);  break;
            case CatType::Kone:  m_resourceSystem->AddResourceBonus(-bonus.KoneBonus);  break;
            default: break;
            }

            // 2) oldCat -> newCat ���̺� ó��

            // (A) Cat -> None : �α�--, per-sec ����
            if (oldCat != CatType::None && newCat == CatType::None)
            {
                m_resourceSystem->AddCurPopulation(-1);

                // �ʴ� ����/�Ҹ� ���� (���� ���� ����)
                m_resourceSystem->AddResourcePerSec(-stats.m_resourceGenPerSec[m_curLevel]);
                m_resourceSystem->AddResourcePerSec(-stats.m_resourceSubPerSec[m_curLevel]);

                // �̹� oldBonus�� ���ŵ�(�� switch)
                m_cat = CatType::None;
                return true;
            }

            // (B) None -> Cat : �α� üũ �� �α�++, per-sec �߰�
            if (oldCat == CatType::None && newCat != CatType::None)
            {
                if (m_resourceSystem->GetCurPopulation() >= m_resourceSystem->GetMaxPopulation()) {
                    std::cout << "�α� �ʰ��� ����� ��ġ ����!" << std::endl;
                    // ����: ���� ����(=None) ����
                    return false;
                }

                m_resourceSystem->AddCurPopulation(1);

                // �ʴ� ����/�Ҹ� �߰� (���� ���� ����)
                m_resourceSystem->AddResourcePerSec(stats.m_resourceGenPerSec[m_curLevel]);
                m_resourceSystem->AddResourcePerSec(stats.m_resourceSubPerSec[m_curLevel]);
            }

            // (C) Cat1 -> Cat2 : �α�, per-sec�� �״�� �ΰ� ���ʽ��� ��ü
            // (�̹� ���� ���ʽ��� ���ŵ�. ���� �� ���ʽ� ����)

            switch (newCat) {
            case CatType::Felis: m_resourceSystem->AddResourceBonus(bonus.FelisBonus); break;
            case CatType::Navi:  m_resourceSystem->AddResourceBonus(bonus.NaviBonus);  break;
            case CatType::Kone:  m_resourceSystem->AddResourceBonus(bonus.KoneBonus);  break;
            default: break;
            }

            // ��� ���� ���� �ÿ��� ���� ����
            m_cat = newCat;
            return true;
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