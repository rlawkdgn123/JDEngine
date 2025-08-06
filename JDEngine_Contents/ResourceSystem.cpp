#include "pch.h"
#include "ResourceSystem.h"

using namespace JDGlobal::Contents;

namespace JDGameSystem {
    void ResourceSystem::SetMaxPopulation(const int& value)
    {
        if (value < 0) {
            m_maxPopulation = 0;
            m_curPopulation = 0;  // ���µ� ��� �ı��ϴ� ó��
            std::cout << "�ִ� �α��� 0���� �����Ǿ� ������ ��� �ı��Ǿ����ϴ�." << std::endl;
        }
        else if (value > MAX_GAME_POPULATION) {
            m_maxPopulation = MAX_GAME_POPULATION;
        }
        else {
            m_maxPopulation = value;
            //if (m_curPopulation > m_maxPopulation) {
            //    m_curPopulation = m_maxPopulation;  // ���� �α��� �ִ� �α��� ������ ���� & ���� ��� �ı�
            //}
        }
    }

    void ResourceSystem::AddMaxPopulation(const int& value)
    {
        if (m_maxPopulation + value < 0) {
            m_maxPopulation = 0;
            //m_curPopulation = 0; // ���� ��� �ı� ó�� �߰� ����
            //std::cout << "�ִ� �α��� 0���� �����Ǿ� ������ ��� �ı��Ǿ����ϴ�." << std::endl;
            std::cout << "�ִ� �α��� 0���� �����Ǿ�����, ���� �α��� �ʰ��Ǿ����ϴ�." << std::endl;
        }
        else if (m_maxPopulation + value > MAX_GAME_POPULATION) {
            m_maxPopulation = MAX_GAME_POPULATION;
            std::cout << "�ִ� �α� ũ�� ����" << std::endl;
        }
        else {
            m_maxPopulation += value;
        }

        // ���� �α��� �ִ� �α��� ������ ���� & ���� ��� �ı�
        /*if (m_curPopulation > m_maxPopulation) {
            m_curPopulation = m_maxPopulation;
        }*/
    }
    void ResourceSystem::SetCurPopulation(const int& value)
    {
        if (value < 0) {
            m_curPopulation = 0;  // ���� ��� �ı�
            std::cout << "���� �α��� 0���� �����Ǿ� ������ ��� �ı��Ǿ����ϴ�." << std::endl;
        }
        else if (value > m_maxPopulation) {
            std::cout << "�ִ� �α��� ����, ����մϴ�." << std::endl;
            return;
            //m_curPopulation = m_maxPopulation;
        }
        else {
            m_curPopulation = value;
        }
    }

    void ResourceSystem::AddCurPopulation(const int& value)
    {
        if (m_maxPopulation + value < 0) {
            m_curPopulation = 0; // ���� ��� �ı� ó�� �߰� ����
            std::cout << "�ִ� �α��� 0���� �����Ǿ� ������ ��� �ı��Ǿ����ϴ�." << std::endl;
        }
        else if (m_maxPopulation + value > MAX_GAME_POPULATION) {
            std::cout << "�ִ� �α��� ����, ����մϴ�." << std::endl;
            return;
           /* m_curPopulation = m_maxPopulation;
            std::cout << "�ִ� �α� ũ�� ����" << std::endl;*/
        }
        else {
            m_curPopulation += value;
        }

        // ���� �α��� �ִ� �α��� ������ ���� & ���� ��� �ı�
        if (m_curPopulation > m_maxPopulation) {
            m_curPopulation = m_maxPopulation;
        }
    }
    
}
