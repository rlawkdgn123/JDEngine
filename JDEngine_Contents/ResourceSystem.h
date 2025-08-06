#pragma once
#include "Global.h"
#include <limits>
#include <iostream>

namespace JDGameSystem {

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

		void ClampResourceToLimits(Resource& value) {
			value.m_food = JDMath::Clamp<float>(value.m_food, 0.0f, JDGlobal::Contents::MAX_GAME_RESOURCE);
			value.m_wood = JDMath::Clamp<float>(value.m_wood, 0.0f, JDGlobal::Contents::MAX_GAME_RESOURCE);
			value.m_mineral = JDMath::Clamp<float>(value.m_mineral, 0.0f, JDGlobal::Contents::MAX_GAME_RESOURCE);
		}

	public:
		void UpdateTotalResourcePerSec() {
			m_totalResourcePerSec = m_resourcePerSec
				+ (m_resourcePerSec * m_resourceBonus)
				+ (m_resourcePerSec * m_synergyBonus);

			ClampResourceToLimits(m_totalResourcePerSec);
		}

		void AccumulateResources(float deltaTime) {
			m_totalResource += m_totalResourcePerSec * Resource(deltaTime, deltaTime, deltaTime);
			ClampResourceToLimits(m_totalResource);
		}

		// Getter
		const Resource& GetTotalResourcePerSec() {
			UpdateTotalResourcePerSec();
			return m_totalResourcePerSec;
		}
		const Resource& GetTotalResource() const { return m_totalResource; }
		const Resource& GetResourcePerSec() const { return m_resourcePerSec; }
		const Resource& GetResourceBonus() const { return m_resourceBonus; }
		const Resource& GetSynergyBonus() const { return m_synergyBonus; }
		const int& GetMaxPopulation() const { return m_maxPopulation; }
		const int& GetCurPopulation() const { return m_curPopulation; }

		// Setter
		void SetResourcePerSec(const Resource& value) { m_resourcePerSec = value; }
		void AddResourcePerSec(const Resource& value) { m_resourcePerSec += value; }

		void SetMaxPopulation(const int& value);
		void AddMaxPopulation(const int& value);
		void SetCurPopulation(const int& value);
		void AddCurPopulation(const int& value);

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

	private:
		Resource m_totalResource;			// �ڿ�
		Resource m_totalResourcePerSec;     // �� �ڿ� ���귮
		Resource m_resourcePerSec;		    // �ʴ� �ǹ� ���귮
		Resource m_resourceBonus;		    // �ڿ� ���ʽ�
		Resource m_synergyBonus;		    // �ó��� ���ʽ�
		int m_maxPopulation = 10;		    // �ִ� �α���
		int m_curPopulation = 0;		    // ���� �α���
	};

}
