#include "pch.h"
#include "CombatSystem.h"
#include <random>

namespace JDGameSystem {

	void CombatSystem::ResolveBattle(ArmySystem& player, const ArmySystem& enemy)
	{
		// 총 공격력.
		float P_my = player.CalculateTotalPower();
		float P_enemy = enemy.CalculateTotalPower();

		if (P_enemy <= 0.0f) { // 0 이하 나누기 방지.
			std::cout << "[CombatSystem] Invalid enemy TotalPower." << std::endl;
			return;
		}

		// 우세도 계산.
		float R = P_my / P_enemy;

		// 손실률 계산. 
		float L;

		if (R <= 1.0f)		L = 1.0f;
		else if (R > 10.0f) L = 0.29f;
		else L = 0.772f * std::expf(-0.098f * R);

		//  손실된 병사 수 계산 및 반영.
		int N_my = player.GetTotalUnits();
		int N_loss = static_cast<int>(N_my * L);

		std::cout << "[CombatSystem] 현재 병사 수: " << N_my << std::endl;
		std::cout << "[CombatSystem] 보정 전 손실 병사 수: " << N_loss << std::endl;

		// 랜덤 수치 추가.
		static std::random_device rd;
		static std::mt19937 gen(rd());
		static std::uniform_int_distribution<int> dist(-3, 3);
		int randVal = dist(gen);

		N_loss = JDMath::Clamp(N_loss + randVal, 0, N_my);

		std::cout << "[CombatSystem] 보정 후 손실 병사 수: " << N_loss << std::endl;

		UnitCounts counts = player.GetUnitCounts();

		int W_n = counts[SordierType::Novice];
		//int W_e = counts[SordierType::Expert];

		int w_n_loss = std::min(W_n, N_loss);
		int w_e_loss = std::max(0, N_loss - W_n);

		counts[SordierType::Novice] -= w_n_loss;
		counts[SordierType::Expert] -= w_e_loss;

		// 손실 출력 (디버깅용)
		//int prevTotalPower = player.CalculateTotalPower();

		////player.SetUnitCounts(counts);

		//int diffTotalPower = prevTotalPower - player.CalculateTotalPower();

		//std::cout << "[CombatSystem] 손실 병사 수: Novice - " << w_n_loss << ", Expert - " << w_e_loss << std::endl;
		//std::cout << "[CombatSystem] 전투력 손실: " << diffTotalPower << std::endl;
	}

	bool CombatSystem::ResolveBattle(const UnitCounts& playerUnits, int playerPower,
		                             const UnitCounts& enemyUnits, int enemyPower,
		                             UnitCounts& outPlayerResult, UnitCounts& outEnemyResult) {
		// 총 공격력.
		float P_my = static_cast<float>(playerPower);
		float P_enemy = static_cast<float>(enemyPower);

		if (P_enemy <= 0.0f) { // 0 이하 나누기 방지.
			outPlayerResult = playerUnits;
			std::cout << "[CombatSystem] Invalid enemy TotalPower." << P_enemy <<std::endl;
			return false;
		}

		if (P_my <= 0.0f) { // 0 이하 나누기 방지.
			outEnemyResult = enemyUnits;
			std::cout << "[CombatSystem] Invalid player TotalPower." << std::endl;
			return false;
		}

		// 우세도 계산.
		float R_my = P_my / P_enemy;
		float R_enemy = P_enemy / P_my;

		// 손실률 계산. 
		float L_my;
		float L_enemy;

		if (R_my <= 1.0f)		L_my = 1.0f;
		else if (R_my > 10.0f) L_my = 0.29f;
		else L_my = 0.772f * std::expf(-0.098f * R_my);

		if (R_enemy <= 1.0f)		L_enemy = 1.0f;
		else if (R_enemy > 10.0f) L_enemy = 0.29f;
		else L_enemy = 0.772f * std::expf(-0.098f * R_enemy);

		//  손실된 병사 수 계산 및 반영.
		int N_my = playerUnits.Total();
		int N_loss_my = static_cast<int>(N_my * L_my);

		int N_enemy = enemyUnits.Total();
		int N_loss_enemy = static_cast<int>(N_enemy * L_enemy);

		// 랜덤 수치 추가. 이건 아군만 적용.
		static std::random_device rd;
		static std::mt19937 gen(rd());
		static std::uniform_int_distribution<int> dist(-3, 3);
		int randVal = dist(gen);
		N_loss_my = JDMath::Clamp(N_loss_my + randVal, 0, N_my);

		// 결과.
		int W_n_my = playerUnits[SordierType::Novice];
		int w_n_loss_my = std::min(W_n_my, N_loss_my);
		int w_e_loss_my = std::max(0, N_loss_my - W_n_my);

		// 죽은 병사만큼 현재 인구 되돌리기.
		ArmySystem::BringbackPopulation(w_n_loss_my + w_e_loss_my);

		outPlayerResult[SordierType::Novice] = std::max(0, playerUnits[SordierType::Novice] - w_n_loss_my);
		outPlayerResult[SordierType::Expert] = std::max(0, playerUnits[SordierType::Expert] - w_e_loss_my);

		int W_n_enemy = enemyUnits[SordierType::Novice];
		int w_n_loss_enemy = std::min(W_n_enemy, N_loss_enemy);
		int w_e_loss_enemy = std::max(0, N_loss_enemy - W_n_enemy);

		outEnemyResult[SordierType::Novice] = std::max(0, enemyUnits[SordierType::Novice] - w_n_loss_enemy);
		outEnemyResult[SordierType::Expert] = std::max(0, enemyUnits[SordierType::Expert] - w_e_loss_enemy);

		if (R_my < 1.0f) { 
			std::cout << "[CombatSystem] 졌음." << std::endl;
			return false; 
		}
		else { 
			std::cout << "[CombatSystem] 이김." << std::endl;
			return true; 
		}
	}
}