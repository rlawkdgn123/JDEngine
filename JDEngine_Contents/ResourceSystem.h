#pragma once
#include "JDGlobal.h"
#include <limits>
#include <iostream>
#include "DataTableManager.h"

namespace JDGameSystem {

	class ResourceSystem
	{
	public:
		using Resource = JDGlobal::Contents::Resource;
		using StartResources = JDGlobal::Contents::StartResources;
		using CatType = JDGlobal::Contents::CatType;
		using NationBonus = JDGlobal::Contents::NationBonus;

		static ResourceSystem& Instance()
		{
			static ResourceSystem instance;
			return instance;
		}

	private:
		ResourceSystem() { DataTableManager::Instance().GetStartResourcesTable(m_totalResource, m_maxPopulation); }
		~ResourceSystem() = default;

		ResourceSystem(const ResourceSystem&) = delete;
		ResourceSystem& operator=(const ResourceSystem&) = delete;

		// -MAX_GAME_RESOURCE_BONUS ~ MAX_GAME_RESOURCE_BONUS 범위로 비율 보정
		Resource ClampBonus(const Resource& value) {
			return Resource{
				JDMath::Clamp<int>(value.m_food, -JDGlobal::Contents::MAX_GAME_RESOURCE_BONUS, JDGlobal::Contents::MAX_GAME_RESOURCE_BONUS),
				JDMath::Clamp<int>(value.m_wood, -JDGlobal::Contents::MAX_GAME_RESOURCE_BONUS, JDGlobal::Contents::MAX_GAME_RESOURCE_BONUS),
				JDMath::Clamp<int>(value.m_mineral, -JDGlobal::Contents::MAX_GAME_RESOURCE_BONUS, JDGlobal::Contents::MAX_GAME_RESOURCE_BONUS)
			};
		}

		// 자원량 상한 제한
		void ClampResourceToLimits(Resource& value) {
			value.m_food = JDMath::Clamp<int>(value.m_food, 0, JDGlobal::Contents::MAX_GAME_RESOURCE);
			value.m_wood = JDMath::Clamp<int>(value.m_wood, 0, JDGlobal::Contents::MAX_GAME_RESOURCE);
			value.m_mineral = JDMath::Clamp<int>(value.m_mineral, 0, JDGlobal::Contents::MAX_GAME_RESOURCE);
		}

		Resource FloatToResourceInt(const Resource& res, float factor) {
			return Resource{
				static_cast<int>(std::round(res.m_food * factor)),
				static_cast<int>(std::round(res.m_wood * factor)),
				static_cast<int>(std::round(res.m_mineral * factor))
			};
		}

	public:
		void Clear() {
			DataTableManager::Instance().GetStartResourcesTable(m_totalResource, m_maxPopulation);
			//m_totalResource.Clear();			// 누적 자원
			m_totalResourcePerSec.Clear();     // 총 생산 자원 (보너스 적용)
			m_resourcePerSec.Clear();		    // 기본 초당 자원
			m_resourceBonus.Clear();		    // % 자원 보너스 (예: 10 = 10%)
			m_synergyBonus.Clear();		    // % 시너지 보너스
			//m_maxPopulation = 10;		    // 최대 인구수
			m_curPopulation = 0;		    // 현재 인구수
		}

		void Update(float deltaTime) {
			m_secondTimer += deltaTime; // 누적 시간을 더해줍니다.
			UpdateTotalResourcePerSec(); // 초당 자원량을 먼저 계산합니다.

			if (m_secondTimer >= 1.0f) // 1초 이상 누적되었는지 확인합니다.
			{
				// 1초 누적 후 처리할 로직
				//std::cout << "1초가 지났읍니다..." << std::endl;

				// 초당 자원량을 한 번만 누적
				m_totalResource += m_totalResourcePerSec;
				ClampResourceToLimits(m_totalResource);

				// 누적 시간에서 1초를 빼고 남은 시간을 보존
				// 예를 들어 1.5초가 누적되었다면 0.5초가 남게 됨
				m_secondTimer -= 1.0f;
			}
		}

		void UpdateTotalResourcePerSec() {

			Resource nationBonus = Resource{
				static_cast<int>(std::round(static_cast<float>(m_resourcePerSec.m_food) * static_cast<float>(m_nationBonus.m_food) / 100.f)),
				static_cast<int>(std::round(static_cast<float>(m_resourcePerSec.m_wood) * static_cast<float>(m_nationBonus.m_wood) / 100.f)),
				static_cast<int>(std::round(static_cast<float>(m_resourcePerSec.m_mineral) * static_cast<float>(m_nationBonus.m_mineral) / 100.f)),
			};
			Resource resourceBonus = Resource{
				static_cast<int>(std::round(static_cast<float>(m_resourcePerSec.m_food) * static_cast<float>(m_resourceBonus.m_food) / 100.f)),
				static_cast<int>(std::round(static_cast<float>(m_resourcePerSec.m_wood) * static_cast<float>(m_resourceBonus.m_wood) / 100.f)),
				static_cast<int>(std::round(static_cast<float>(m_resourcePerSec.m_mineral) * static_cast<float>(m_resourceBonus.m_mineral) / 100.f)),
			};
			Resource synergyBonus = Resource{
				static_cast<int>(std::round(static_cast<float>(m_resourcePerSec.m_food) * static_cast<float>(m_synergyBonus.m_food) / 100.f)),
				static_cast<int>(std::round(static_cast<float>(m_resourcePerSec.m_wood) * static_cast<float>(m_synergyBonus.m_wood) / 100.f)),
				static_cast<int>(std::round(static_cast<float>(m_resourcePerSec.m_mineral) * static_cast<float>(m_synergyBonus.m_mineral) / 100.f)),
			};
			m_totalResourcePerSec = (m_resourcePerSec + nationBonus + resourceBonus + synergyBonus);
		}

		// Getters
		const Resource& GetTotalResourcePerSec() { return m_totalResourcePerSec; }
		const Resource& GetTotalResource() const { return m_totalResource; }
		const Resource& GetResourcePerSec() const { return m_resourcePerSec; }
		const Resource& GetNationBonus() { return m_nationBonus; }
		const Resource& GetResourceBonus() const { return m_resourceBonus; }
		const Resource& GetSynergyBonus() const { return m_synergyBonus; }
		const int& GetMaxPopulation() const { return m_maxPopulation; }
		const int& GetCurPopulation() const { return m_curPopulation; }

		// Setters
		void SetResourcePerSec(const Resource& value) { m_resourcePerSec = value; }
		void AddResourcePerSec(const Resource& value) { m_resourcePerSec += value; }

		void SetTotalResource(const Resource& value) {
			m_totalResource = value;
			ClampResourceToLimits(m_totalResource);
		}

		void AddTotalResource(const Resource& value) {
			m_totalResource += value;
			ClampResourceToLimits(m_totalResource);
		}

		void SetResourceBonus(const Resource& value) {
			m_resourceBonus = ClampBonus(value);
		}
		
		void AddResourceBonus(const Resource& value) {
			m_resourceBonus += value;
			m_resourceBonus = ClampBonus(m_resourceBonus);
		}

		void SetSynergyBonus(const Resource& value) {
			m_synergyBonus = ClampBonus(value);
		}

		void AddSynergyBonus(const Resource& value) {
			m_synergyBonus += value;
			m_synergyBonus = ClampBonus(m_synergyBonus);
		}

		void SetMaxPopulation(const int& value);
		void AddMaxPopulation(const int& value);
		void SetCurPopulation(const int& value);
		void AddCurPopulation(const int& value);

		CatType GetNation() { return m_nation; }
		void SetNation(CatType nation) {
			m_nation = nation; // 현재 국가 저장

			NationBonus bonus; // 구조체 인스턴스 생성

			switch (m_nation) {
			case CatType::Felis:
				m_nationBonus = bonus.FelisBonus;
				break;
			case CatType::Navi:
				m_nationBonus = bonus.NaviBonus;
				break;
			case CatType::Kone:
				m_nationBonus = bonus.KoneBonus;
				break;
			}
		}
	private:
		CatType m_nation;
		
		Resource m_totalResource;			// 누적 자원
		Resource m_totalResourcePerSec;     // 총 생산 자원 (보너스 적용)
		Resource m_resourcePerSec;		    // 기본 초당 자원
		Resource m_nationBonus;				// % 국가 보너스 (예: 10 = 10%)
		Resource m_resourceBonus;		    // % 자원 보너스 
		Resource m_synergyBonus;		    // % 시너지 보너스
		int m_maxPopulation = 0;		    // 최대 인구수
		int m_curPopulation = 0;		    // 현재 인구수

		float m_secondTimer;
	};
}
