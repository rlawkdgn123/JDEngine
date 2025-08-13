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

		// -MAX_GAME_RESOURCE_BONUS ~ MAX_GAME_RESOURCE_BONUS ������ ���� ����
		Resource ClampBonus(const Resource& value) {
			return Resource{
				JDMath::Clamp<int>(value.m_food, -JDGlobal::Contents::MAX_GAME_RESOURCE_BONUS, JDGlobal::Contents::MAX_GAME_RESOURCE_BONUS),
				JDMath::Clamp<int>(value.m_wood, -JDGlobal::Contents::MAX_GAME_RESOURCE_BONUS, JDGlobal::Contents::MAX_GAME_RESOURCE_BONUS),
				JDMath::Clamp<int>(value.m_mineral, -JDGlobal::Contents::MAX_GAME_RESOURCE_BONUS, JDGlobal::Contents::MAX_GAME_RESOURCE_BONUS)
			};
		}

		// �ڿ��� ���� ����
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
			//m_totalResource.Clear();			// ���� �ڿ�
			m_totalResourcePerSec.Clear();     // �� ���� �ڿ� (���ʽ� ����)
			m_resourcePerSec.Clear();		    // �⺻ �ʴ� �ڿ�
			m_resourceBonus.Clear();		    // % �ڿ� ���ʽ� (��: 10 = 10%)
			m_synergyBonus.Clear();		    // % �ó��� ���ʽ�
			//m_maxPopulation = 10;		    // �ִ� �α���
			m_curPopulation = 0;		    // ���� �α���
		}

		void Update(float deltaTime) {
			m_secondTimer += deltaTime; // ���� �ð��� �����ݴϴ�.
			UpdateTotalResourcePerSec(); // �ʴ� �ڿ����� ���� ����մϴ�.

			if (m_secondTimer >= 1.0f) // 1�� �̻� �����Ǿ����� Ȯ���մϴ�.
			{
				// 1�� ���� �� ó���� ����
				//std::cout << "1�ʰ� �������ϴ�..." << std::endl;

				// �ʴ� �ڿ����� �� ���� ����
				m_totalResource += m_totalResourcePerSec;
				ClampResourceToLimits(m_totalResource);

				// ���� �ð����� 1�ʸ� ���� ���� �ð��� ����
				// ���� ��� 1.5�ʰ� �����Ǿ��ٸ� 0.5�ʰ� ���� ��
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
			m_nation = nation; // ���� ���� ����

			NationBonus bonus; // ����ü �ν��Ͻ� ����

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
		
		Resource m_totalResource;			// ���� �ڿ�
		Resource m_totalResourcePerSec;     // �� ���� �ڿ� (���ʽ� ����)
		Resource m_resourcePerSec;		    // �⺻ �ʴ� �ڿ�
		Resource m_nationBonus;				// % ���� ���ʽ� (��: 10 = 10%)
		Resource m_resourceBonus;		    // % �ڿ� ���ʽ� 
		Resource m_synergyBonus;		    // % �ó��� ���ʽ�
		int m_maxPopulation = 0;		    // �ִ� �α���
		int m_curPopulation = 0;		    // ���� �α���

		float m_secondTimer;
	};
}
