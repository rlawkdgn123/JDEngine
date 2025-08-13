#include "pch.h"
#include "framework.h"
#include "ResourceSystem.h"
#include "House.h"
using namespace JDComponent;
using namespace std;
using ResourceSystem = JDGameSystem::ResourceSystem;
namespace JDGameObject {
    namespace Content {
        
        void House::Awake()
        {
            // ���ҽ� �ý��� ���� & �Ľ� �⺻�� �Ҵ�
            m_resourceSystem = &ResourceSystem::Instance();
            DataTableManager::Instance().GetHouseTable(m_stats);
            m_stats.PrintStats();
        }
        void House::Start()
        {
            cout << "�Ͽ콺 ������" << endl;
            m_resourceSystem->AddTotalResource(-m_stats.m_upgradeCost[m_curLevel]); // ���� �� �ʱ� ����� ����
            m_resourceSystem->AddMaxPopulation(m_stats.m_initPopulation[0]); // �ʱ� �α��� ������ (���� 0�� �α���)�� ����
            m_resourceSystem->AddCurPopulation(m_stats.m_initPopulation[0]);
            cout << m_stats.m_initPopulation[0] << endl;
            cout << m_resourceSystem->GetMaxPopulation() << endl;
            m_resourceSystem->AddResourcePerSec(m_stats.m_resourceSubPerSec[0]); // �ʴ� �ڿ� �Ҹ�(������)�� �����մϴ�.
        }
        void House::Update(float deltaTime)
        {
        }
        void House::LateUpdate(float deltaTime)
        {
        }
        void House::FixedUpdate(float fixedDeltaTime)
        {
        }
        void House::OnDestroy() // �ı� ��
        {
            if (m_curLevel != 0) wcout << this->GetName() << L" - [�߰� ����] �ı��� ��û�Ǿ����� 0������ �ƴմϴ�!!" << endl;

            // �ǹ� �ı� �� ���� ������ ������ �����ϰ�, �ִ� �α����� ����
            m_resourceSystem->AddResourcePerSec(-m_stats.m_resourceSubPerSec[m_curLevel]); // �ʴ� �Ҹ� �����ϱ�
            m_resourceSystem->AddMaxPopulation(-m_stats.m_initPopulation[m_curLevel]); // �α��� ���̱�}
            m_resourceSystem->AddCurPopulation(-m_stats.m_initPopulation[m_curLevel]);
        }

        bool House::LevelUp()
        {
            if (m_curLevel >= JDGlobal::Contents::MAX_GAME_LEVEL - 1) { // �迭 �ε��� �����÷ο� ����
                cout << "���� �ִ�ġ!!" << endl;
                return false;
            }
            
            const auto& nextUpgradeCost = m_stats.m_upgradeCost[m_curLevel + 1]; // ������ ���

            // �ڿ� ���� ���� üũ
            if (m_resourceSystem->GetTotalResource().m_food < nextUpgradeCost.m_food ||
                m_resourceSystem->GetTotalResource().m_wood < nextUpgradeCost.m_wood ||
                m_resourceSystem->GetTotalResource().m_mineral < nextUpgradeCost.m_mineral) {
                cout << "���� �� ����! ���׷��̵� �ڿ��� ���ڶ��ϴ�!" << endl;
                return false;
            }

            // ������ �ڿ� �Ҹ�
            m_resourceSystem->AddTotalResource(-nextUpgradeCost);

            wcout << this->GetName()
                << L" - Level Up!! " << m_curLevel
                << L" -> " << m_curLevel + 1 << endl;

            // ���� ������ ������ �����ϰ�, ���� ������ ������ �����մϴ�.
            m_resourceSystem->AddResourcePerSec(-m_stats.m_resourceSubPerSec[m_curLevel]);
            m_resourceSystem->AddResourcePerSec(m_stats.m_resourceSubPerSec[m_curLevel + 1]);

            // ���� ������ ���� ������ �α��� ���̸�ŭ �ִ� �α����� �����մϴ�.
            m_resourceSystem->AddMaxPopulation(m_stats.m_initPopulation[m_curLevel + 1] - m_stats.m_initPopulation[m_curLevel]);
            m_resourceSystem->AddCurPopulation(m_stats.m_initPopulation[m_curLevel + 1] - m_stats.m_initPopulation[m_curLevel]);

            ++m_curLevel;

            return true;
        }
        bool House::LevelDown()
        {
            if (m_curLevel == 0) { cout << "�̹� ���� ����!! �ı��մϴ�!!" << endl; return false; } // ����ó���� ��ư �ʿ���

            // ���� ������ ������ �����ϰ�, ���� ������ ������ ����
            m_resourceSystem->AddResourcePerSec(-m_stats.m_resourceSubPerSec[m_curLevel]);
            m_resourceSystem->AddResourcePerSec(m_stats.m_resourceSubPerSec[m_curLevel - 1]);

            // ���� ������ ���� ������ �α��� ���̸�ŭ �ִ� �α����� ����
            m_resourceSystem->AddMaxPopulation(m_stats.m_initPopulation[m_curLevel - 1] - m_stats.m_initPopulation[m_curLevel]);
            m_resourceSystem->AddCurPopulation(m_stats.m_initPopulation[m_curLevel - 1] - m_stats.m_initPopulation[m_curLevel]);

            --m_curLevel;
            return true;
        }
    }
}
