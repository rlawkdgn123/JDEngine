#include "pch.h"
#include "framework.h"
#include "ColliderBase.h"
#include "BoxCollider.h"
#include "BuildSystem.h"

using namespace JDComponent;
using namespace JDGlobal::Contents;
using namespace std;

void BuildSystem::CreateGrid(SceneBase* curScene)
{
    const float startX = -500.0f;
    const float startY = 300.0f;
    const float spacingX = 100.0f;
    const float spacingY = -100.0f;

    for (int row = 0; row < MAX_GAME_GRID_RAW; ++row) {
        for (int col = 0; col < MAX_GAME_GRID_COL; ++col) {
            // 1) 이름 생성
            std::wstring name = L"Box_" + std::to_wstring(row) + L"_" + std::to_wstring(col);

            // 2) 생성
            auto* box = curScene->CreateGameObject<Grid>(name.c_str());

            // 3) 위치 세팅
            float x = startX + spacingX * col;
            float y = startY + spacingY * row;
            box->GetComponent<Transform>()->SetPosition({ x, y });
            box->AddComponent<Editor_Clickable>();

            // 4) Occupied 설정: 2~4행, 2~4열만 true
            if (row >= m_startRows && row <= m_endRows &&
                col >= m_startCols && col <= m_endCols) {
                box->SetOccupied(true);
            }
            else {
                box->SetOccupied(false);
            }

            // 5) 모든 타일 HasBuilding 초기화
            box->SetHasBuilding(false);

            // 6) Expanded 설정: 3x3 영역 주변 1칸
            if (row >= m_startRows - 1 && row <= m_endRows + 1 &&
                col >= m_startCols - 1 && col <= m_endCols + 1 && !box->IsOccupied()) {
                box->SetExpanded(true);
            }
            else {
                box->SetExpanded(false);
            }

            // 7) 콜라이더 추가 및 인덱스 설정
            int idx = row * MAX_GAME_GRID_COL + col;
            auto* collider = box->AddComponent<BoxCollider>(Vector2F{ 47,47 });
            collider->SetIndex(idx);
        }
    }

}

void BuildSystem::ExpandTile(Grid* tile)
{
    // 다음 선택이 최대 그리드양을 초과하면
    if (m_expandedCount + 1 == MAX_GAME_GRID_MAT) { cout << "응 안돼~ 꽉꽉 채워놓고 욕심이 많아~" << endl; return; }
    if (m_choiceCount <= 0) { cout << "타일 선택권 가져오거라 아이야" << endl; return; }

    if (tile->IsExpanded()) {
        --m_choiceCount;
        tile->SetExpanded(false);
        tile->SetOccupied(true);
        cout << "땅을 샀다. 나도 사고싶다." << endl;
        cout << "남은 카운트 : " << m_choiceCount << endl;
    }
}
