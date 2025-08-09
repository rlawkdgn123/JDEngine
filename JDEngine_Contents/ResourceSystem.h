#pragma once
#include "JDGlobal.h"
#include <limits>
#include <iostream>

namespace JDGameSystem {

	class ResourceSystem
	{
	public:
		using Resource = JDGlobal::Contents::Resource;

		static ResourceSystem& Instance()
		{
			static ResourceSystem instance;
			return instance;
		}

	private:
		ResourceSystem() = default;
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
			m_totalResource.Clear();			// 누적 자원
			m_totalResourcePerSec.Clear();     // 총 생산 자원 (보너스 적용)
			m_resourcePerSec.Clear();		    // 기본 초당 자원
			m_resourceBonus.Clear();		    // % 자원 보너스 (예: 10 = 10%)
			m_synergyBonus.Clear();		    // % 시너지 보너스
			m_maxPopulation = 10;		    // 최대 인구수
			m_curPopulation = 0;		    // 현재 인구수
		}
		void Update(float deltaTime) {
			UpdateTotalResourcePerSec();
			AccumulateResources(deltaTime);
		}

		void UpdateTotalResourcePerSec() {
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
			m_totalResourcePerSec = (m_resourcePerSec + resourceBonus + synergyBonus);
		}

		void AccumulateResources(float deltaTime) {
			Resource delta(
				static_cast<int>(m_totalResourcePerSec.m_food * deltaTime),
				static_cast<int>(m_totalResourcePerSec.m_wood * deltaTime),
				static_cast<int>(m_totalResourcePerSec.m_mineral * deltaTime)
			);

			m_totalResource += delta;
			ClampResourceToLimits(m_totalResource);
		}

		// Getters
		const Resource& GetTotalResourcePerSec() { return m_totalResourcePerSec; }
		const Resource& GetTotalResource() const { return m_totalResource; }
		const Resource& GetResourcePerSec() const { return m_resourcePerSec; }
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
		void SetSynergyBonus(const Resource& value) {
			m_synergyBonus = ClampBonus(value);
		}

		void SetMaxPopulation(const int& value);
		void AddMaxPopulation(const int& value);
		void SetCurPopulation(const int& value);
		void AddCurPopulation(const int& value);

	private:
		Resource m_totalResource;			// 누적 자원
		Resource m_totalResourcePerSec;     // 총 생산 자원 (보너스 적용)
		Resource m_resourcePerSec;		    // 기본 초당 자원
		Resource m_resourceBonus;		    // % 자원 보너스 (예: 10 = 10%)
		Resource m_synergyBonus;		    // % 시너지 보너스
		int m_maxPopulation = 10;		    // 최대 인구수
		int m_curPopulation = 0;		    // 현재 인구수
	};

}
