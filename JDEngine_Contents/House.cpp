#include "pch.h"
#include "framework.h"
#include "ResourceSystem.h"
#include "House.h"
using namespace JDComponent;
using namespace std;
using ResourceSystem = JDGameSystem::ResourceSystem;
namespace JDGameObject {
    namespace Content {
        
        void House::Awake()
        {
            // 리소스 시스템 연결 & 파싱 기본값 할당
            m_resourceSystem = &ResourceSystem::Instance();
            DataTableManager::Instance().GetHouseTable(m_stats);
            m_stats.PrintStats();
        }
        void House::Start()
        {
            cout << "하우스 쓰따뜨" << endl;
            m_resourceSystem->AddTotalResource(-m_stats.m_upgradeCost[m_curLevel]); // 건축 시 초기 비용을 차감
            m_resourceSystem->AddMaxPopulation(m_stats.m_initPopulation[0]); // 초기 인구수 증가량 (레벨 0의 인구수)을 적용
            m_resourceSystem->AddCurPopulation(m_stats.m_initPopulation[0]);
            cout << m_stats.m_initPopulation[0] << endl;
            cout << m_resourceSystem->GetMaxPopulation() << endl;
            m_resourceSystem->AddResourcePerSec(m_stats.m_resourceSubPerSec[0]); // 초당 자원 소모량(유지비)을 적용합니다.
        }
        void House::Update(float deltaTime)
        {
        }
        void House::LateUpdate(float deltaTime)
        {
        }
        void House::FixedUpdate(float fixedDeltaTime)
        {
        }
        void House::OnDestroy() // 파괴 시
        {
            if (m_curLevel != 0) wcout << this->GetName() << L" - [추가 정보] 파괴가 요청되었으나 0레벨이 아닙니다!!" << endl;

            // 건물 파괴 시 현재 레벨의 유지비를 복구하고, 최대 인구수를 감소
            m_resourceSystem->AddResourcePerSec(-m_stats.m_resourceSubPerSec[m_curLevel]); // 초당 소모량 복구하기
            m_resourceSystem->AddMaxPopulation(-m_stats.m_initPopulation[m_curLevel]); // 인구수 줄이기}
            m_resourceSystem->AddCurPopulation(-m_stats.m_initPopulation[m_curLevel]);
        }

        bool House::LevelUp()
        {
            if (m_curLevel >= JDGlobal::Contents::MAX_GAME_LEVEL - 1) { // 배열 인덱스 오버플로우 방지
                cout << "레벨 최대치!!" << endl;
                return false;
            }
            
            const auto& nextUpgradeCost = m_stats.m_upgradeCost[m_curLevel + 1]; // 레벨업 비용

            // 자원 부족 여부 체크
            if (m_resourceSystem->GetTotalResource().m_food < nextUpgradeCost.m_food ||
                m_resourceSystem->GetTotalResource().m_wood < nextUpgradeCost.m_wood ||
                m_resourceSystem->GetTotalResource().m_mineral < nextUpgradeCost.m_mineral) {
                cout << "레벨 업 실패! 업그레이드 자원이 모자랍니다!" << endl;
                return false;
            }

            // 레벨업 자원 소모
            m_resourceSystem->AddTotalResource(-nextUpgradeCost);

            wcout << this->GetName()
                << L" - Level Up!! " << m_curLevel
                << L" -> " << m_curLevel + 1 << endl;

            // 현재 레벨의 유지비를 제거하고, 다음 레벨의 유지비를 적용합니다.
            m_resourceSystem->AddResourcePerSec(-m_stats.m_resourceSubPerSec[m_curLevel]);
            m_resourceSystem->AddResourcePerSec(m_stats.m_resourceSubPerSec[m_curLevel + 1]);

            // 현재 레벨과 다음 레벨의 인구수 차이만큼 최대 인구수를 조절합니다.
            m_resourceSystem->AddMaxPopulation(m_stats.m_initPopulation[m_curLevel + 1] - m_stats.m_initPopulation[m_curLevel]);
            m_resourceSystem->AddCurPopulation(m_stats.m_initPopulation[m_curLevel + 1] - m_stats.m_initPopulation[m_curLevel]);

            ++m_curLevel;

            return true;
        }
        bool House::LevelDown()
        {
            if (m_curLevel == 0) { cout << "이미 최저 레벨!! 파괴합니다!!" << endl; return false; } // 예외처리는 버튼 쪽에서

            // 현재 레벨의 유지비를 제거하고, 이전 레벨의 유지비를 적용
            m_resourceSystem->AddResourcePerSec(-m_stats.m_resourceSubPerSec[m_curLevel]);
            m_resourceSystem->AddResourcePerSec(m_stats.m_resourceSubPerSec[m_curLevel - 1]);

            // 현재 레벨과 이전 레벨의 인구수 차이만큼 최대 인구수를 조절
            m_resourceSystem->AddMaxPopulation(m_stats.m_initPopulation[m_curLevel - 1] - m_stats.m_initPopulation[m_curLevel]);
            m_resourceSystem->AddCurPopulation(m_stats.m_initPopulation[m_curLevel - 1] - m_stats.m_initPopulation[m_curLevel]);

            --m_curLevel;
            return true;
        }
    }
}
