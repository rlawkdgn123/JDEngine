#include "pch.h"
#include "framework.h"
#include "ColliderBase.h"
#include "BoxCollider.h"
#include "BuildSystem.h"
#include "Grid.h"

using namespace JDComponent;
using namespace JDGlobal::Contents;
using namespace std;

using Grid = JDGameObject::Content::Grid;

void BuildSystem::CreateGrid(SceneBase* curScene)
{

    // =====================================================================
    // 그리드 생성 로직
    // =====================================================================

    // 0. 그리드 생성 전 필드 청소 // 나중에 다른 씬에도 쓸 수 있도록 확장할거면 preScene 필요합니다.
    DestroyGrid(curScene);

    // 1. 실제 셀들이 배치될 수 있는 공간 계산 (전체 영역 - 총 간격)
    float m_contentWidth = m_areaWidth - (m_padding * (m_totalCols - 1));
    float m_contentHeight = m_areaHeight - (m_padding * (m_totalRows - 1));

    // 2. 정사각형 셀의 한 변의 최대 크기 계산
    float cellWidthPossible = m_contentWidth / m_totalCols;
    float cellHeightPossible = m_contentHeight / m_totalRows;

    // 3. 정사각형 유지를 위해 가능한 크기 중 더 작은 쪽을 최종 셀 크기로 선택
    const float squareCellSize = std::min(cellWidthPossible, cellHeightPossible);

    // 4. 최종 그리드의 전체 크기 계산
    float finalGridWidth = (squareCellSize * m_totalCols) + (m_padding * (m_totalCols - 1));
    float finalGridHeight = (squareCellSize * m_totalRows) + (m_padding * (m_totalRows - 1));

    // 5. 전체 그리드를 영역 내에서 중앙에 배치하기 위한 시작 오프셋 계산
    float offsetX = (m_areaWidth - finalGridWidth) / 2.0f;
    float offsetY = (m_areaHeight - finalGridHeight) / 2.0f;

    // std::vector<std::vector<Grid*>> gridArray(m_totalRows, std::vector<Grid*>(m_totalCols, nullptr));
    m_gridArray.assign(m_totalRows, std::vector<Grid*>(m_totalCols, nullptr));

    for (int row = 0; row < m_totalRows; ++row) {
        for (int col = 0; col < m_totalCols; ++col) {

            std::wstring name = L"Box_" + std::to_wstring(row) + L"_" + std::to_wstring(col);
            auto* box = curScene->CreateGameObject<Grid>(name.c_str());

            // 6. 각 셀의 최종 위치 계산
            //    시작점 + 중앙 오프셋 + (셀 크기 + 간격) * 인덱스 + 셀 중앙 보정
            float x = m_areaTopLeft.x + offsetX + (col * (squareCellSize + m_padding)) + (squareCellSize / 2.0f);
            float y = m_areaTopLeft.y - offsetY - (row * (squareCellSize + m_padding)) - (squareCellSize / 2.0f);

           
            box->GetComponent<Transform>()->SetPosition({ x, y });
            box->AddComponent<Editor_Clickable>();

            // ==========================================================
            // 타일 점유 / 확장 영역 지정
            // ==========================================================
             //Occupied 설정 : 2~4행, 2~4열만 true
            if (row >= m_startRows && row <= m_endRows &&
                col >= m_startCols && col <= m_endCols) {
                box->SetOccupied(true);
            }
            else {
                box->SetOccupied(false);
            }

            // 모든 타일 HasBuilding 초기화
            box->SetHasBuilding(false);

            // Expanded 설정 : 십자 인접(상하좌우) 1칸
           // Occupied는 제외
            if (!box->IsOccupied()) {
                bool isAdjacent =
                    ((row >= m_startRows - 1 && row <= m_endRows + 1) && (col >= m_startCols && col <= m_endCols)) || // 상하
                    ((col >= m_startCols - 1 && col <= m_endCols + 1) && (row >= m_startRows && row <= m_endRows));   // 좌우

                if (isAdjacent) box->SetExpanded(true);
                else box->SetExpanded(false);
            }
            else box->SetExpanded(false);

            /*
            //Expanded 설정: 전방위 3x3 영역 주변 1칸
            if (row >= m_startRows - 1 && row <= m_endRows + 1 &&
                col >= m_startCols - 1 && col <= m_endCols + 1 && !box->IsOccupied()) {
                box->SetExpanded(true);
            }
            else {
                box->SetExpanded(false);
            }
            */
            // ==========================================================

            // TextureRenderer
            auto* textureRenderer = box->AddComponent<TextureRenderer>("");

            int idx = row * m_totalCols + col; // 행 우선 인덱스
            int textureNumber = idx + 1;       // 1 ~ N

            std::stringstream ss;
            ss << "ART_Tile" << std::setw(2) << std::setfill('0') << textureNumber;
            textureRenderer->SetTextureName(ss.str());
            // ==========================================================

            textureRenderer->SetSize({ squareCellSize, squareCellSize });
            textureRenderer->SetLayerInfo({ SortingLayer::Grid, 0 });

            auto* collider = box->AddComponent<BoxCollider>(Vector2F{ squareCellSize / 2.0f, squareCellSize / 2.0f });
            collider->SetIndex(idx);

            box->CreateTextureObj(*curScene);

            // [수정] 생성된 그리드 포인터를 멤버 변수에 저장
            // gridArray[row][col] = box;
            m_gridArray[row][col] = box;
        }
    }
    //각 그리드의 others[4]에 상하좌우 인접 그리드 포인터 설정
    for (int row = 0; row < m_totalRows; ++row) {
        for (int col = 0; col < m_totalCols; ++col) {
            Grid* current = m_gridArray[row][col];

            // 상
            current->SetOtherGrid(Direction::North, (row > 0) ? m_gridArray[row - 1][col] : nullptr);
            // 하
            current->SetOtherGrid(Direction::South, (row < m_totalRows - 1) ? m_gridArray[row + 1][col] : nullptr);
            // 좌
            current->SetOtherGrid(Direction::West, (col > 0) ? m_gridArray[row][col - 1] : nullptr); 
            // 우
            current->SetOtherGrid(Direction::East, (col < m_totalCols - 1) ? m_gridArray[row][col + 1] : nullptr); 
        }
    }
}

void BuildSystem::DestroyGrid(JDScene::SceneBase* curScene) {

for (auto& row : m_gridArray) {
    for (auto* grid : row) {
            grid->DeleteTextureObj(*curScene); // 그리드와 관련된(그리드가 생성한) 오브젝트 우선 일괄 제거
            curScene->DestroyObject(grid); // 그리드 본체 제거
        }
    }
    m_gridArray.clear();
}

/* {
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
}*/

void BuildSystem::UpdateTextureObj(JDComponent::ColliderBase* selectedCol) {
    for (auto& row : m_gridArray) {
        for (auto* grid : row) {
            // 확장 지역
            if (m_choiceCount >= 0 && grid->IsExpanded()) { // 카운트가 있으면
                grid->SetActiveExpendEffectTextureObj(true);
            }
            else {
                grid->SetActiveExpendEffectTextureObj(false);
            }

            if (grid->IsOccupied()) {
                // 건물 상태
                if (grid->HasBuilding()) {
                    grid->SetActiveLevelTextureObj(true);
                }
                else {
                    false;
                }
                // 고양이 상태
                if (grid->GetCatType() != CatType::None && grid->GetCatType() != CatType::CatTypeMAX) {
                    grid->SetActiveCatTextureObj(true);
                }
                else {
                    grid->SetActiveCatTextureObj(false);
                }
            }
            if (!selectedCol) { grid->SetActiveSelectEffectTextureObj(false); continue; }

            if (grid->GetID() == selectedCol->GetOwner()->GetID()) {
                grid->SetActiveSelectEffectTextureObj(true);
            }
            else {
                grid->SetActiveSelectEffectTextureObj(false);
            }
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

void BuildSystem::OnDestroy() {

}
