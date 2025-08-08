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
    }

}

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
