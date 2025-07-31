#pragma once
#include "Global.h"
#include <limits>
namespace GameManager {
	class ResourceSystem
	{
	public:
		using Resource = JDGlobal::Contents::Resource;
	public:

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

	private:
		Resource ClampBonus(const Resource& value) {
			return Resource{
				JDMath::Clamp<float>(value.m_food, -1.0f, 1.0f),
				JDMath::Clamp<float>(value.m_wood, -1.0f, 1.0f),
				JDMath::Clamp<float>(value.m_mineral, -1.0f, 1.0f)
			};
		}
	public:

		void UpdateTotalResourcePerSec() {
			Resource& total = m_totalResourcePerSec;

			total = m_resourcePerSec
				+ (m_resourcePerSec * m_resourceBonus)
				+ (m_resourcePerSec * m_synergyBonus);

			if (total.m_food < 0) { total.m_food = 0; }
			if (total.m_wood < 0) { total.m_wood = 0; }
			if (total.m_mineral < 0) { total.m_mineral = 0; }
		}

		// Getter
		const Resource& GetTotalResourcePerSec() { this->UpdateTotalResourcePerSec(); return m_totalResourcePerSec; }
		const Resource& GetResourcePerSec() const { return m_resourcePerSec; }
		const Resource& GetResourceBonus() const { return m_resourceBonus; }
		const Resource& GetSynergyBonus() const { return m_synergyBonus; }
		const unsigned int& GetTotalPopulation() const { return m_totalPopulation; }

		// Setter
		void SetResourcePerSec(const Resource& value) { m_resourcePerSec = value; }
		void AddResourcePerSec(const  Resource& value) { m_resourcePerSec += value; }

		void SetTotalPopulation(const int& value) { m_totalPopulation = value; }
		void AddTotalPopulation(const int& value) { m_totalPopulation += value; }

		void SetResourceBonus(const Resource& value) {
			m_resourceBonus = this->ClampBonus(value);
		}
		void SetSynergyBonus(const Resource& value) {
			m_synergyBonus = this->ClampBonus(value);
		}

	private:
		Resource m_totalResourcePerSec; // 총 자원 생산량
		Resource m_resourcePerSec;		// 초당 건물 생산량
		Resource m_resourceBonus;		// 자원 보너스
		Resource m_synergyBonus;		// 시너지 보너스
		//Resource m_resourceAmount;	// 자원 획득량
		int m_totalPopulation;		//인구수
	};
}

