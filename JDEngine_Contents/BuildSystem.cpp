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


    // =====================================================================
    // �׸��� ���� ����
    // =====================================================================

    // 1. ���� ������ ��ġ�� �� �ִ� ���� ��� (��ü ���� - �� ����)
    float m_contentWidth = m_areaWidth - (m_padding * (m_totalCols - 1));
    float m_contentHeight = m_areaHeight - (m_padding * (m_totalRows - 1));

    // 2. ���簢�� ���� �� ���� �ִ� ũ�� ���
    float cellWidthPossible = m_contentWidth / m_totalCols;
    float cellHeightPossible = m_contentHeight / m_totalRows;

    // 3. ���簢�� ������ ���� ������ ũ�� �� �� ���� ���� ���� �� ũ��� ����
    const float squareCellSize = std::min(cellWidthPossible, cellHeightPossible);

    // 4. ���� �׸����� ��ü ũ�� ���
    float finalGridWidth = (squareCellSize * m_totalCols) + (m_padding * (m_totalCols - 1));
    float finalGridHeight = (squareCellSize * m_totalRows) + (m_padding * (m_totalRows - 1));

    // 5. ��ü �׸��带 ���� ������ �߾ӿ� ��ġ�ϱ� ���� ���� ������ ���
    float offsetX = (m_areaWidth - finalGridWidth) / 2.0f;
    float offsetY = (m_areaHeight - finalGridHeight) / 2.0f;


    for (int row = 0; row < m_totalRows; ++row) {
        for (int col = 0; col < m_totalCols; ++col) {

            std::wstring name = L"Box_" + std::to_wstring(row) + L"_" + std::to_wstring(col);
            auto* box = curScene->CreateGameObject<Grid>(name.c_str());

            // 6. �� ���� ���� ��ġ ���
            //    ������ + �߾� ������ + (�� ũ�� + ����) * �ε��� + �� �߾� ����
            float x = m_areaTopLeft.x + offsetX + (col * (squareCellSize + m_padding)) + (squareCellSize / 2.0f);
            float y = m_areaTopLeft.y - offsetY - (row * (squareCellSize + m_padding)) - (squareCellSize / 2.0f);

            box->GetComponent<Transform>()->SetPosition({ x, y });
            box->AddComponent<Editor_Clickable>();

           // ==========================================================
           // Ÿ�� ���� / Ȯ�� ���� ����
           // ==========================================================
            //Occupied ���� : 2~4��, 2~4���� true
            if (row >= m_startRows && row <= m_endRows &&
                col >= m_startCols && col <= m_endCols) {
                box->SetOccupied(true);
            }
            else {
                box->SetOccupied(false);
            }

            // ��� Ÿ�� HasBuilding �ʱ�ȭ
            box->SetHasBuilding(false);

            //Expanded ����: 3x3 ���� �ֺ� 1ĭ
            if (row >= m_startRows - 1 && row <= m_endRows + 1 &&
                col >= m_startCols - 1 && col <= m_endCols + 1 && !box->IsOccupied()) {
                box->SetExpanded(true);
            }
            else {
                box->SetExpanded(false);
            }

            // ==========================================================
  
            // TextureRenderer�� ���� �߰��մϴ�. (�̶� �ؽ�ó �̸��� ����Ӵϴ�)
            auto* textureRenderer = box->AddComponent<TextureRenderer>("");

            // ==========================================================
            // �ڽ� �ؽ�ó �̸� ���� ���� �� ����
            // ==========================================================
            int idx = row * m_totalCols + col; // �� row �켱 �ε����� ����
            int textureNumber = idx + 1;       // 1 ~ N

            std::stringstream ss;
            ss << "ART_Tile" << std::setw(2) << std::setfill('0') << textureNumber;
            textureRenderer->SetTextureName(ss.str());
            // ==========================================================

            textureRenderer->SetSize({ squareCellSize, squareCellSize });

            auto* collider = box->AddComponent<BoxCollider>(Vector2F{ squareCellSize / 2.0f, squareCellSize / 2.0f });
            collider->SetIndex(idx);
        }
    }
}
/* {
const float startX = -500.0f;
const float startY = 300.0f;
const float spacingX = 100.0f;
const float spacingY = -100.0f;

for (int row = 0; row < MAX_GAME_GRID_RAW; ++row) {
    for (int col = 0; col < MAX_GAME_GRID_COL; ++col) {
        // 1) �̸� ����
        std::wstring name = L"Box_" + std::to_wstring(row) + L"_" + std::to_wstring(col);

        // 2) ����
        auto* box = curScene->CreateGameObject<Grid>(name.c_str());

        // 3) ��ġ ����
        float x = startX + spacingX * col;
        float y = startY + spacingY * row;
        box->GetComponent<Transform>()->SetPosition({ x, y });
        box->AddComponent<Editor_Clickable>();

        // 4) Occupied ����: 2~4��, 2~4���� true
        if (row >= m_startRows && row <= m_endRows &&
            col >= m_startCols && col <= m_endCols) {
            box->SetOccupied(true);
        }
        else {
            box->SetOccupied(false);
        }

        // 5) ��� Ÿ�� HasBuilding �ʱ�ȭ
        box->SetHasBuilding(false);

        // 6) Expanded ����: 3x3 ���� �ֺ� 1ĭ
        if (row >= m_startRows - 1 && row <= m_endRows + 1 &&
            col >= m_startCols - 1 && col <= m_endCols + 1 && !box->IsOccupied()) {
            box->SetExpanded(true);
        }
        else {
            box->SetExpanded(false);
        }

        // 7) �ݶ��̴� �߰� �� �ε��� ����
        int idx = row * MAX_GAME_GRID_COL + col;
        auto* collider = box->AddComponent<BoxCollider>(Vector2F{ 47,47 });
        collider->SetIndex(idx);
    }
}*/


void BuildSystem::ExpandTile(Grid* tile)
{
    // ���� ������ �ִ� �׸������ �ʰ��ϸ�
    if (m_expandedCount + 1 == MAX_GAME_GRID_MAT) { cout << "�� �ȵ�~ �˲� ä������ ����� ����~" << endl; return; }
    if (m_choiceCount <= 0) { cout << "Ÿ�� ���ñ� �������Ŷ� ���̾�" << endl; return; }

    if (tile->IsExpanded()) {
        --m_choiceCount;
        tile->SetExpanded(false);
        tile->SetOccupied(true);
        cout << "���� ���. ���� ���ʹ�." << endl;
        cout << "���� ī��Ʈ : " << m_choiceCount << endl;
    }
}
