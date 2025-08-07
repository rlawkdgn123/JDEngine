#include"pch.h"
#include"framework.h"
#include"JDGlobal.h"

namespace JDMath = JDGlobal::Math;

namespace JDGlobal {
	namespace Base {
	}
	namespace Core {
	}
	namespace Contents {

		void Resource::ResourcePrint() const{
			std::cout << "m_food : " << m_food << std::endl;
			std::cout << "m_wood : " << m_food << std::endl;
			std::cout << "m_mineral : " << m_food << std::endl;
		}

		void BuildingStats::PrintStats() {
			// ��� ���
			std::cout << "�ڿ�,�ʱ��ڿ�,"
				<< "���׷��̵���_1,���׷��̵���_2,���׷��̵���_3,"
				<< "�ڿ�ȹ��_1,�ڿ�ȹ��_2,�ڿ�ȹ��_3,"
				<< "�ڿ��Ҹ�_1,�ڿ��Ҹ�_2,�ڿ��Ҹ�_3\n";

			// Food �� ���
			std::cout << "Food : " << m_initResource.m_food << ",";
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_upgradeCost[i].m_food << ",";
			}
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_resourceGenPerSec[i].m_food << ",";
			}
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_resourceSubPerSec[i].m_food << (i == MAX_GAME_LEVEL - 1 ? "" : ",");
			}
			std::cout << "\n";

			// Wood �� ���
			std::cout << "Wood : " << m_initResource.m_wood << ",";
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_upgradeCost[i].m_wood << ",";
			}
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_resourceGenPerSec[i].m_wood << ",";
			}
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_resourceSubPerSec[i].m_wood << (i == MAX_GAME_LEVEL - 1 ? "" : ",");
			}
			std::cout << "\n";

			// Mineral �� ���
			std::cout << "Mineral : " << m_initResource.m_mineral << ",";
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_upgradeCost[i].m_mineral << ",";
			}
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_resourceGenPerSec[i].m_mineral << ",";
			}
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_resourceSubPerSec[i].m_mineral << (i == MAX_GAME_LEVEL - 1 ? "" : ",");
			}
			std::cout << "\n";
		}

		void HouseStats::PrintStats() {
			// ��� ���
			std::cout << "������,"
				<< "����_1,����_2,����_3,"
				<< "���׷��̵���_1,���׷��̵���_2,���׷��̵���_3,"
				<< "�ڿ��Ҹ�_1,�ڿ��Ҹ�_2,�ڿ��Ҹ�_3\n";

			// Population �� ��� (������ �α�)
			std::cout << "Population : ";
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_initPopulation[i] << ",";
			}
			// �α� ���� �ٸ� �����Ͱ� �����Ƿ� �������� ä��ϴ�.
			for (int i = 0; i < MAX_GAME_LEVEL * 2; ++i) {
				std::cout << ",";
			}
			std::cout << "\n";

			// Food �� ���
			std::cout << "Food : ";
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << " ,";
			}
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_upgradeCost[i].m_food << ",";
			}
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_resourceSubPerSec[i].m_food << (i == MAX_GAME_LEVEL - 1 ? "" : ",");
			}
			std::cout << "\n";

			// Wood �� ���
			std::cout << "Wood : ";
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << " ,";
			}
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_upgradeCost[i].m_wood << ",";
			}
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_resourceSubPerSec[i].m_wood << (i == MAX_GAME_LEVEL - 1 ? "" : ",");
			}
			std::cout << "\n";

			// Mineral �� ���
			std::cout << "Mineral : ";
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << " ,";
			}
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_upgradeCost[i].m_mineral << ",";
			}
			for (int i = 0; i < MAX_GAME_LEVEL; ++i) {
				std::cout << m_resourceSubPerSec[i].m_mineral << (i == MAX_GAME_LEVEL - 1 ? "" : ",");
			}
			std::cout << "\n";
		}

	}
}