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
            // 시작할 때 고양이가 있다면, 고양이 자원 보너스 주기
            ChangeCat(m_cat);
        }
        void Building::Update(float deltaTime)
        {
            __super::Update(deltaTime);
        }

        bool Building::ChangeCat(CatType newCat)
        {
            // 동일 타입이면 아무 것도 하지 않음(성공)
            if (m_cat == newCat) return true;

            auto bonus = JDGlobal::Contents::CatResourceBonus();
            CatType oldCat = m_cat;

            // 1) 이전 고양이 보너스 해제 (있었다면)
            switch (oldCat) {
            case CatType::Felis: m_resourceSystem->AddResourceBonus(-bonus.FelisBonus); break;
            case CatType::Navi:  m_resourceSystem->AddResourceBonus(-bonus.NaviBonus);  break;
            case CatType::Kone:  m_resourceSystem->AddResourceBonus(-bonus.KoneBonus);  break;
            default: break;
            }

            // 2) oldCat -> newCat 전이별 처리

            // (A) Cat -> None : 인구--, per-sec 제거
            if (oldCat != CatType::None && newCat == CatType::None)
            {
                m_resourceSystem->AddCurPopulation(-1);

                // 초당 생산/소모 제거 (현재 레벨 기준)
                m_resourceSystem->AddResourcePerSec(-stats.m_resourceGenPerSec[m_curLevel]);
                m_resourceSystem->AddResourcePerSec(-stats.m_resourceSubPerSec[m_curLevel]);

                // 이미 oldBonus는 제거됨(위 switch)
                m_cat = CatType::None;
                return true;
            }

            // (B) None -> Cat : 인구 체크 후 인구++, per-sec 추가
            if (oldCat == CatType::None && newCat != CatType::None)
            {
                if (m_resourceSystem->GetCurPopulation() >= m_resourceSystem->GetMaxPopulation()) {
                    std::cout << "인구 초과로 고양이 배치 실패!" << std::endl;
                    // 실패: 이전 상태(=None) 유지
                    return false;
                }

                m_resourceSystem->AddCurPopulation(1);

                // 초당 생산/소모 추가 (현재 레벨 기준)
                m_resourceSystem->AddResourcePerSec(stats.m_resourceGenPerSec[m_curLevel]);
                m_resourceSystem->AddResourcePerSec(stats.m_resourceSubPerSec[m_curLevel]);
            }

            // (C) Cat1 -> Cat2 : 인구, per-sec는 그대로 두고 보너스만 교체
            // (이미 이전 보너스는 제거됨. 이제 새 보너스 적용)

            switch (newCat) {
            case CatType::Felis: m_resourceSystem->AddResourceBonus(bonus.FelisBonus); break;
            case CatType::Navi:  m_resourceSystem->AddResourceBonus(bonus.NaviBonus);  break;
            case CatType::Kone:  m_resourceSystem->AddResourceBonus(bonus.KoneBonus);  break;
            default: break;
            }

            // 모든 적용 성공 시에만 상태 갱신
            m_cat = newCat;
            return true;
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