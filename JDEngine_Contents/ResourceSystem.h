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
				JDMath::Clamp(value.m_wood, -1.0f, 1.0f),
				JDMath::Clamp(value.m_stone, -1.0f, 1.0f),
				JDMath::Clamp(value.m_mineral, -1.0f, 1.0f)
			};
		}
		Resource ClampTotal(const Resource& value) {
			return Resource{
				JDMath::Clamp(value.m_wood, 0.f, std::numeric_limits<float>::max()),
				JDMath::Clamp(value.m_stone, 0.f, std::numeric_limits<float>::max()),
				JDMath::Clamp(value.m_mineral, 0.f, std::numeric_limits<float>::max())
			};
		}
	public:

		void UpdateTotalResourcePerSec() {
			Resource& total = m_totalResourcePerSec;

			total = m_resourcePerSec
				+ (m_resourcePerSec * m_resourceBonus)
				+ (m_resourcePerSec * m_synergyBonus);

			total = ClampTotal(total);
		}

		
		// Getter
		const Resource& GetTotalResourcePerSec() { this->UpdateTotalResourcePerSec(); return m_totalResourcePerSec; }
		const Resource& GetResourcePerSec() const { return m_resourcePerSec; }
		const Resource& GetResourceBonus() const { return m_resourceBonus; }
		const Resource& GetSynergyBonus() const { return m_synergyBonus; }
		const Resource& GetResourceAmount() const { return m_resourceAmount; }
		
		// Setter
		void SetResourcePerSec(const Resource& value) { m_resourcePerSec = value; }
		void AddResourcePerSec(const  Resource& value) { m_resourcePerSec += value; }
		void SetResourceAmount(const Resource& value) { m_resourceAmount = value; }

		void SetResourceBonus(const Resource& value) {
			m_resourceBonus = this->ClampBonus(value);
		}
		void SetSynergyBonus(const Resource& value) {
			m_synergyBonus = this->ClampBonus(value);
		}

	private:
		Resource m_totalResourcePerSec;
		Resource m_resourcePerSec;
		Resource m_resourceBonus;
		Resource m_synergyBonus;
		Resource m_resourceAmount;
	};
}

