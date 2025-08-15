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
            m_resourceSystem->AddTotalResource(-m_stats.m_upgradeCost[m_curLevel]);
            wcout << this->GetName() + L" 건설 및 자원(Wood) 소모량 : " + to_wstring(m_stats.m_upgradeCost[m_curLevel].m_wood) << endl;

            // 시작할 때 고양이가 있다면, 고양이 자원 보너스 주기
            ChangeCat(m_cat);
        }

        void Building::Update(float deltaTime)
        {
            __super::Update(deltaTime);
        }

        void Building::OnDestroy()
        {
            // 고양이가 배치되어 있다면, 관련된 모든 자원 효과를 제거합니다.
            if (m_cat != CatType::None) {
                // 인구 회복 (고양이 제거 시)
                m_resourceSystem->AddCurPopulation(1);

                // 고양이 보너스 제거
                auto bonus = JDGlobal::Contents::CatResourceBonus();
                switch (m_cat) {
                case CatType::Felis: m_resourceSystem->AddResourceBonus(-bonus.FelisBonus); break;
                case CatType::Navi:  m_resourceSystem->AddResourceBonus(-bonus.NaviBonus);  break;
                case CatType::Kone:  m_resourceSystem->AddResourceBonus(-bonus.KoneBonus);  break;
                default: break;
                }

                // 현재 레벨의 초당 생산/소모량 제거
                m_resourceSystem->AddResourcePerSec(-m_stats.m_resourceGenPerSec[m_curLevel]);
                m_resourceSystem->AddResourcePerSec(-m_stats.m_resourceSubPerSec[m_curLevel]);
            }
        }

        bool Building::ChangeCat(CatType newCat)
        {
            if (m_cat == newCat) return true; // 동일 타입이면 아무 것도 안 함

            auto bonus = JDGlobal::Contents::CatResourceBonus();
            CatType oldCat = m_cat;

            // 1) 이전 고양이 보너스 해제 (있었다면)
            switch (oldCat) {
            case CatType::Felis: m_resourceSystem->AddResourceBonus(-bonus.FelisBonus); break;
            case CatType::Navi:  m_resourceSystem->AddResourceBonus(-bonus.NaviBonus);  break;
            case CatType::Kone:  m_resourceSystem->AddResourceBonus(-bonus.KoneBonus);  break;
            default: break;
            }

            // 이전 고양이가 있었다면 초당 자원량 제거 및 인구 회복
            if (oldCat != CatType::None) {
                m_resourceSystem->AddResourcePerSec(-m_stats.m_resourceGenPerSec[m_curLevel]);
                m_resourceSystem->AddResourcePerSec(-m_stats.m_resourceSubPerSec[m_curLevel]);
                m_resourceSystem->AddCurPopulation(1);
            }

            // 새로운 고양이가 있다면
            if (newCat != CatType::None) {
                if (m_resourceSystem->GetCurPopulation() <= 0) {
                    // 인구 부족 시 이전 보너스 복구
                    switch (oldCat) {
                    case CatType::Felis: m_resourceSystem->AddResourceBonus(bonus.FelisBonus); break;
                    case CatType::Navi:  m_resourceSystem->AddResourceBonus(bonus.NaviBonus);  break;
                    case CatType::Kone:  m_resourceSystem->AddResourceBonus(bonus.KoneBonus);  break;
                    default: break;
                    }
                    std::cout << "인구 부족으로 고양이 배치 실패!" << std::endl;
                    return false;
                }

                // 초당 자원량 적용
                m_resourceSystem->AddResourcePerSec(m_stats.m_resourceGenPerSec[m_curLevel]);
                m_resourceSystem->AddResourcePerSec(m_stats.m_resourceSubPerSec[m_curLevel]);
                m_resourceSystem->AddCurPopulation(-1);

                // ★ 고양이 보너스 적용
                switch (newCat) {
                case CatType::Felis: m_resourceSystem->AddResourceBonus(bonus.FelisBonus); break;
                case CatType::Navi:  m_resourceSystem->AddResourceBonus(bonus.NaviBonus);  break;
                case CatType::Kone:  m_resourceSystem->AddResourceBonus(bonus.KoneBonus);  break;
                default: break;
                }
            }

            // 디버그 출력
            auto CatTypeToString = [](CatType cat) -> const char* {
                switch (cat) {
                case CatType::None:  return "None";
                case CatType::Felis: return "Felis";
                case CatType::Navi:  return "Navi";
                case CatType::Kone:  return "Kone";
                default:             return "Unknown";
                }
                };
            std::cout << "Building cat type changed from " << CatTypeToString(oldCat)
                << " to " << CatTypeToString(newCat) << std::endl;

            // 상태 갱신
            m_cat = newCat;

            return true;
        }


        // 해당 코드에서의 return false : 취소한다는 뜻
        bool Building::LevelUp()
        {
            if (m_curLevel >= JDGlobal::Contents::MAX_GAME_LEVEL - 1) { cout << "레벨 최대치!!" << endl; return false; }

            if (m_resourceSystem->GetTotalResource().m_food < m_stats.m_upgradeCost[m_curLevel + 1].m_food) { cout << "레벨 업 실패! 업그레이드 자원이 모자랍니다! - food" << endl; return false; }
            else if (m_resourceSystem->GetTotalResource().m_wood < m_stats.m_upgradeCost[m_curLevel + 1].m_wood) { cout << "레벨 업 실패! 업그레이드 자원이 모자랍니다! - wood" << endl; return false; }
            else if (m_resourceSystem->GetTotalResource().m_mineral < m_stats.m_upgradeCost[m_curLevel + 1].m_mineral) { cout << "레벨 업 실패! 업그레이드 자원이 모자랍니다! - mineral" << endl; return false; }

            // 레벨업 자원 소모
            m_resourceSystem->AddTotalResource(-m_stats.m_upgradeCost[m_curLevel + 1]);

            wcout << this->GetName()
                << L" - Level Up!! " << (m_curLevel + 1)
                << L" -> " << (m_curLevel + 2) << endl;

            // 초당 자원 재생/소모량 더하기
            // 고양이가 배치되어 있을 때만 자원량이 변동됩니다.
            if (m_cat != CatType::None) {
                m_resourceSystem->AddResourcePerSec(m_stats.m_resourceGenPerSec[m_curLevel + 1] - m_stats.m_resourceGenPerSec[m_curLevel]);
                m_resourceSystem->AddResourcePerSec(m_stats.m_resourceSubPerSec[m_curLevel + 1] - m_stats.m_resourceSubPerSec[m_curLevel]);
            }

            ++m_curLevel;
            return true;
        }

        // 해당 코드에서의 return false : 파괴한다는 뜻
        bool Building::LevelDown()
        {
            if (m_curLevel == 0) { cout << "이미 최저 레벨!! 파괴합니다!!" << endl; return false; } // 예외처리는 버튼 쪽에서

            // 초당 자원 재생량 더하기
            m_resourceSystem->AddResourcePerSec(-m_stats.m_resourceGenPerSec[m_curLevel]);
            m_resourceSystem->AddResourcePerSec(-m_stats.m_resourceSubPerSec[m_curLevel]);
            --m_curLevel;

            // 초당 자원 재생량 더하기
            m_resourceSystem->AddResourcePerSec(m_stats.m_resourceGenPerSec[m_curLevel]);
            m_resourceSystem->AddResourcePerSec(m_stats.m_resourceSubPerSec[m_curLevel]);

            return true;
        }

        void Building::PrintCat()
        {
            switch (m_cat) {
            case CatType::Felis: cout << "Felis" << endl; break;
            case CatType::Navi:  cout << "Navi" << endl; break;
            case CatType::Kone:  cout << "Kone" << endl; break;
            case CatType::None:  cout << "None" << endl; break;
            default:              cout << "Default" << endl; break;
            }
        }
    }
}
