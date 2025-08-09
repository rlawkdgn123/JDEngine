#include "pch.h"
#include "framework.h"
using namespace JDComponent;
using ResourceSystem = JDGameSystem::ResourceSystem;
using namespace std;
namespace JDGameObject {
    namespace Content {
        void Building::Awake()
        {
            __super::Awake();
            m_resourceSystem = &ResourceSystem::Instance();
        }

        void Building::Start()
        {
            __super::Start();
            // 설치 자원량 빼기
            m_resourceSystem->AddTotalResource(-stats.m_upgradeCost[m_curLevel]);
            wcout << this->GetName() + L" 건설 및 자원(Wood) 소모량 : " + to_wstring(stats.m_upgradeCost[m_curLevel].m_wood) << endl;

            // 초당 자원 재생/소모량 더하기
            m_resourceSystem->AddResourcePerSec(stats.m_resourceGenPerSec[m_curLevel]);
            m_resourceSystem->AddResourcePerSec(stats.m_resourceSubPerSec[m_curLevel]);
        }
        void Building::Update(float deltaTime)
        {
            __super::Update(deltaTime);
        }
        void Building::ChangeCat(CatType catType)
        {
            if (m_cat == catType) return;

            auto bonus = JDGlobal::Contents::CatResourceBonus();

            switch (m_cat) {
            case CatType::Felis:
                m_resourceSystem->AddResourceBonus(-bonus.FelisBonus);
                break;
            case CatType::Navi:
                m_resourceSystem->AddResourceBonus(-bonus.NaviBonus);
                break;
            case CatType::Kone:
                m_resourceSystem->AddResourceBonus(-bonus.KoneBonus);
                break;
            default:
                break;
            }
            
            switch (catType) {
            case CatType::Felis:
                m_resourceSystem->AddResourceBonus(bonus.FelisBonus);
                break;
            case CatType::Navi:
                m_resourceSystem->AddResourceBonus(bonus.NaviBonus);
                break;
            case CatType::Kone:
                m_resourceSystem->AddResourceBonus(bonus.KoneBonus);
                break;
            default:
                break;
            }

            m_cat = catType;
        }

        // 해당 코드에서의 return false : 취소한다는 뜻
        bool Building::LevelUp()
        {
            if (m_curLevel >= JDGlobal::Contents::MAX_GAME_LEVEL - 1) { cout << "레벨 최대치!!" << endl; return false; }
            
            if (m_resourceSystem->GetTotalResource().m_food < stats.m_upgradeCost[m_curLevel + 1].m_food) { cout << "레벨 업 실패! 업그레이드 자원이 모자랍니다! - food" << endl; return false; }
            else if (m_resourceSystem->GetTotalResource().m_wood < stats.m_upgradeCost[m_curLevel + 1].m_wood) { cout << "레벨 업 실패! 업그레이드 자원이 모자랍니다! - wood" << endl; return false; }
            else if (m_resourceSystem->GetTotalResource().m_mineral < stats.m_upgradeCost[m_curLevel + 1].m_mineral) { cout << "레벨 업 실패! 업그레이드 자원이 모자랍니다! - mineral" << endl; return false; }

            // 레벨업 자원 소모
           m_resourceSystem->AddTotalResource(-stats.m_upgradeCost[m_curLevel + 1]);

            wcout << this->GetName()
                << L" - Level Up!! " << (m_curLevel + 1)
                << L" -> " << (m_curLevel + 2) << endl;

            // 초당 자원 재생/소모량 더하기
            m_resourceSystem->AddResourcePerSec(-stats.m_resourceGenPerSec[m_curLevel]);
            m_resourceSystem->AddResourcePerSec(-stats.m_resourceSubPerSec[m_curLevel]);
            ++m_curLevel;
            m_resourceSystem->AddResourcePerSec(stats.m_resourceGenPerSec[m_curLevel]);
            m_resourceSystem->AddResourcePerSec(stats.m_resourceSubPerSec[m_curLevel]);
            
            return true;
        }

        // 해당 코드에서의 return false : 파괴한다는 뜻
        bool Building::LevelDown()
        {
            if (m_curLevel == 0) { cout << "이미 최저 레벨!! 파괴합니다!!" << endl; return false; } // 예외처리는 버튼 쪽에서

            // 초당 자원 재생량 더하기
            m_resourceSystem->AddResourcePerSec(-stats.m_resourceGenPerSec[m_curLevel]);
            m_resourceSystem->AddResourcePerSec(-stats.m_resourceSubPerSec[m_curLevel]);
            --m_curLevel;
            // 초당 자원 재생량 더하기
            m_resourceSystem->AddResourcePerSec(stats.m_resourceGenPerSec[m_curLevel]);
            m_resourceSystem->AddResourcePerSec(stats.m_resourceSubPerSec[m_curLevel]);

            return true;
        }
    }
}