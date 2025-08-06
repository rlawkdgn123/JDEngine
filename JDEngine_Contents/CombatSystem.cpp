#include "pch.h"
#include "CombatSystem.h"
#include <random>

namespace JDGameSystem {

	void CombatSystem::ResolveBattle(ArmySystem& player, const ArmySystem& enemy)
	{
		// �� ���ݷ�.
		float P_my = player.CalculateTotalPower();
		float P_enemy = enemy.CalculateTotalPower();

		if (P_enemy <= 0.0f) { // 0 ���� ������ ����.
			std::cout << "[CombatSystem] Invalid enemy TotalPower." << std::endl;
			return;
		}

		// �켼�� ���.
		float R = P_my / P_enemy;

		// �սǷ� ���. 
		float L;

		if (R <= 1.0f)		L = 1.0f;
		else if (R > 10.0f) L = 0.29f;
		else L = 0.772f * std::expf(-0.098f * R);

		//  �սǵ� ���� �� ��� �� �ݿ�.
		int N_my = player.GetTotalUnits();
		int N_loss = static_cast<int>(N_my * L);

		std::cout << "[CombatSystem] ���� ���� ��: " << N_my << std::endl;
		std::cout << "[CombatSystem] ���� �� �ս� ���� ��: " << N_loss << std::endl;

		// ���� ��ġ �߰�.
		static std::random_device rd;
		static std::mt19937 gen(rd());
		static std::uniform_int_distribution<int> dist(-3, 3);
		int randVal = dist(gen);

		N_loss = JDMath::Clamp(N_loss + randVal, 0, N_my);

		std::cout << "[CombatSystem] ���� �� �ս� ���� ��: " << N_loss << std::endl;

		UnitCounts counts = player.GetUnitCounts();

		int W_n = counts[UnitType::Novice];
		//int W_e = counts[UnitType::Expert];

		int w_n_loss = std::min(W_n, N_loss);
		int w_e_loss = std::max(0, N_loss - W_n);

		counts[UnitType::Novice] -= w_n_loss;
		counts[UnitType::Expert] -= w_e_loss;

		UnitCounts{ 10, 20 };

		// �ս� ��� (������)
		int prevTotalPower = player.CalculateTotalPower();

		player.SetUnitCounts(counts);

		int diffTotalPower = prevTotalPower - player.CalculateTotalPower();

		std::cout << "[CombatSystem] �ս� ���� ��: Novice - " << w_n_loss << ", Expert - " << w_e_loss << std::endl;
		std::cout << "[CombatSystem] ������ �ս�: " << diffTotalPower << std::endl;
	}
}