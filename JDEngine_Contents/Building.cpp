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
            m_resourceSystem->AddTotalResource(-m_stats.m_upgradeCost[m_curLevel]);
            wcout << this->GetName() + L" �Ǽ� �� �ڿ�(Wood) �Ҹ� : " + to_wstring(m_stats.m_upgradeCost[m_curLevel].m_wood) << endl;

            // ������ �� ����̰� �ִٸ�, ����� �ڿ� ���ʽ� �ֱ�
            ChangeCat(m_cat);
        }

        void Building::Update(float deltaTime)
        {
            __super::Update(deltaTime);
        }

        void Building::OnDestroy()
        {
            // ����̰� ��ġ�Ǿ� �ִٸ�, ���õ� ��� �ڿ� ȿ���� �����մϴ�.
            if (m_cat != CatType::None) {
                // �α� ȸ�� (����� ���� ��)
                m_resourceSystem->AddCurPopulation(1);

                // ����� ���ʽ� ����
                auto bonus = JDGlobal::Contents::CatResourceBonus();
                switch (m_cat) {
                case CatType::Felis: m_resourceSystem->AddResourceBonus(-bonus.FelisBonus); break;
                case CatType::Navi:  m_resourceSystem->AddResourceBonus(-bonus.NaviBonus);  break;
                case CatType::Kone:  m_resourceSystem->AddResourceBonus(-bonus.KoneBonus);  break;
                default: break;
                }

                // ���� ������ �ʴ� ����/�Ҹ� ����
                m_resourceSystem->AddResourcePerSec(-m_stats.m_resourceGenPerSec[m_curLevel]);
                m_resourceSystem->AddResourcePerSec(-m_stats.m_resourceSubPerSec[m_curLevel]);
            }
        }

        bool Building::ChangeCat(CatType newCat)
        {
            if (m_cat == newCat) return true; // ���� Ÿ���̸� �ƹ� �͵� �� ��

            auto bonus = JDGlobal::Contents::CatResourceBonus();
            CatType oldCat = m_cat;

            // 1) ���� ����� ���ʽ� ���� (�־��ٸ�)
            switch (oldCat) {
            case CatType::Felis: m_resourceSystem->AddResourceBonus(-bonus.FelisBonus); break;
            case CatType::Navi:  m_resourceSystem->AddResourceBonus(-bonus.NaviBonus);  break;
            case CatType::Kone:  m_resourceSystem->AddResourceBonus(-bonus.KoneBonus);  break;
            default: break;
            }

            // ���� ����̰� �־��ٸ� �ʴ� �ڿ��� ���� �� �α� ȸ��
            if (oldCat != CatType::None) {
                m_resourceSystem->AddResourcePerSec(-m_stats.m_resourceGenPerSec[m_curLevel]);
                m_resourceSystem->AddResourcePerSec(-m_stats.m_resourceSubPerSec[m_curLevel]);
                m_resourceSystem->AddCurPopulation(1);
            }

            // ���ο� ����̰� �ִٸ�
            if (newCat != CatType::None) {
                if (m_resourceSystem->GetCurPopulation() <= 0) {
                    // �α� ���� �� ���� ���ʽ� ����
                    switch (oldCat) {
                    case CatType::Felis: m_resourceSystem->AddResourceBonus(bonus.FelisBonus); break;
                    case CatType::Navi:  m_resourceSystem->AddResourceBonus(bonus.NaviBonus);  break;
                    case CatType::Kone:  m_resourceSystem->AddResourceBonus(bonus.KoneBonus);  break;
                    default: break;
                    }
                    std::cout << "�α� �������� ����� ��ġ ����!" << std::endl;
                    return false;
                }

                // �ʴ� �ڿ��� ����
                m_resourceSystem->AddResourcePerSec(m_stats.m_resourceGenPerSec[m_curLevel]);
                m_resourceSystem->AddResourcePerSec(m_stats.m_resourceSubPerSec[m_curLevel]);
                m_resourceSystem->AddCurPopulation(-1);

                // �� ����� ���ʽ� ����
                switch (newCat) {
                case CatType::Felis: m_resourceSystem->AddResourceBonus(bonus.FelisBonus); break;
                case CatType::Navi:  m_resourceSystem->AddResourceBonus(bonus.NaviBonus);  break;
                case CatType::Kone:  m_resourceSystem->AddResourceBonus(bonus.KoneBonus);  break;
                default: break;
                }
            }

            // ����� ���
            auto CatTypeToString = [](CatType cat) -> const char* {
                switch (cat) {
                case CatType::None:  return "None";
                case CatType::Felis: return "Felis";
                case CatType::Navi:  return "Navi";
                case CatType::Kone:  return "Kone";
                default:             return "Unknown";
                }
                };
            std::cout << "Building cat type changed from " << CatTypeToString(oldCat)
                << " to " << CatTypeToString(newCat) << std::endl;

            // ���� ����
            m_cat = newCat;

            return true;
        }


        // �ش� �ڵ忡���� return false : ����Ѵٴ� ��
        bool Building::LevelUp()
        {
            if (m_curLevel >= JDGlobal::Contents::MAX_GAME_LEVEL - 1) { cout << "���� �ִ�ġ!!" << endl; return false; }

            if (m_resourceSystem->GetTotalResource().m_food < m_stats.m_upgradeCost[m_curLevel + 1].m_food) { cout << "���� �� ����! ���׷��̵� �ڿ��� ���ڶ��ϴ�! - food" << endl; return false; }
            else if (m_resourceSystem->GetTotalResource().m_wood < m_stats.m_upgradeCost[m_curLevel + 1].m_wood) { cout << "���� �� ����! ���׷��̵� �ڿ��� ���ڶ��ϴ�! - wood" << endl; return false; }
            else if (m_resourceSystem->GetTotalResource().m_mineral < m_stats.m_upgradeCost[m_curLevel + 1].m_mineral) { cout << "���� �� ����! ���׷��̵� �ڿ��� ���ڶ��ϴ�! - mineral" << endl; return false; }

            // ������ �ڿ� �Ҹ�
            m_resourceSystem->AddTotalResource(-m_stats.m_upgradeCost[m_curLevel + 1]);

            wcout << this->GetName()
                << L" - Level Up!! " << (m_curLevel + 1)
                << L" -> " << (m_curLevel + 2) << endl;

            // �ʴ� �ڿ� ���/�Ҹ� ���ϱ�
            // ����̰� ��ġ�Ǿ� ���� ���� �ڿ����� �����˴ϴ�.
            if (m_cat != CatType::None) {
                m_resourceSystem->AddResourcePerSec(m_stats.m_resourceGenPerSec[m_curLevel + 1] - m_stats.m_resourceGenPerSec[m_curLevel]);
                m_resourceSystem->AddResourcePerSec(m_stats.m_resourceSubPerSec[m_curLevel + 1] - m_stats.m_resourceSubPerSec[m_curLevel]);
            }

            ++m_curLevel;
            return true;
        }

        // �ش� �ڵ忡���� return false : �ı��Ѵٴ� ��
        bool Building::LevelDown()
        {
            if (m_curLevel == 0) { cout << "�̹� ���� ����!! �ı��մϴ�!!" << endl; return false; } // ����ó���� ��ư �ʿ���

            // �ʴ� �ڿ� ����� ���ϱ�
            m_resourceSystem->AddResourcePerSec(-m_stats.m_resourceGenPerSec[m_curLevel]);
            m_resourceSystem->AddResourcePerSec(-m_stats.m_resourceSubPerSec[m_curLevel]);
            --m_curLevel;

            // �ʴ� �ڿ� ����� ���ϱ�
            m_resourceSystem->AddResourcePerSec(m_stats.m_resourceGenPerSec[m_curLevel]);
            m_resourceSystem->AddResourcePerSec(m_stats.m_resourceSubPerSec[m_curLevel]);

            return true;
        }

        void Building::PrintCat()
        {
            switch (m_cat) {
            case CatType::Felis: cout << "Felis" << endl; break;
            case CatType::Navi:  cout << "Navi" << endl; break;
            case CatType::Kone:  cout << "Kone" << endl; break;
            case CatType::None:  cout << "None" << endl; break;
            default:              cout << "Default" << endl; break;
            }
        }
    }
}
