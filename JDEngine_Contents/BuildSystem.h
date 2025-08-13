#pragma once
#include "Grid.h"
#include "BoxCollider.h"

namespace JDGameObject { namespace Content { class Grid; } }

class BuildSystem
{
public:
	BuildSystem() : m_areaWidth(515.0f), m_areaHeight(760.0f),
		m_startCols(1), m_padding(10.0f), m_startRows(1),
		m_endCols(3), m_endRows(3), m_choiceCount(4),
		m_totalRows(6), m_totalCols(4)
	{
		m_expandedCount = (m_endCols - m_startCols + 1) * (m_endRows - m_startRows + 1);
		m_areaTopLeft = { -850.0f, 350.0f };
		if (m_totalCols <= JDGlobal::Contents::MAX_GAME_GRID_COL) m_totalCols = JDGlobal::Contents::MAX_GAME_GRID_COL;
		if (m_totalRows <= JDGlobal::Contents::MAX_GAME_GRID_ROW) m_totalRows = JDGlobal::Contents::MAX_GAME_GRID_ROW;
	}
public:

	void UpdateTextureObj(JDComponent::ColliderBase* selectedCol);
	void SetStartGridMat(int sRow, int sCol, int eRow, int eCol) { 
		m_startRows = sRow; m_startCols = sCol;
		m_endRows = eRow; m_endCols = eCol;
	}
	void CreateGrid(JDScene::SceneBase* curScene); // ��ü �׸��� �׸���
	void DestroyGrid(JDScene::SceneBase* curScene);
	void ExpandTile(JDGameObject::Content::Grid* tile); // ���� ������ Ÿ�� Ȯ���ϱ�
	void AddChoiceCount(int add) { m_choiceCount += add; }

	// Getter
	int GetStartCols() const { return m_startCols; }
	int GetStartRows() const { return m_startRows; }
	int GetEndCols() const { return m_endCols; }
	int GetEndRows() const { return m_endRows; }
	int GetExpandedCount() const { return m_expandedCount; }
	int GetChoiceCount() const { return m_choiceCount; }

	int GetTotalCols() const { return m_totalCols; }
	int GetTotalRows() const { return m_totalRows; }

	float GetAreaWidth() const { return m_areaWidth; }
	float GetAreaHeight() const { return m_areaHeight; }
	float GetPadding() const { return m_padding; }

	Vector2F GetAreaTopLeft() const { return m_areaTopLeft; }

	// Setter
	void SetStartCols(int cols) { m_startCols = cols; }
	void SetStartRows(int rows) { m_startRows = rows; }
	void SetEndCols(int cols) { m_endCols = cols; }
	void SetEndRows(int rows) { m_endRows = rows; }
	void SetExpandedCount(int count) { m_expandedCount = count; }
	void SetChoiceCount(int count) { m_choiceCount = count; }

	void SetTotalCols(int cols) { m_totalCols = cols; }
	void SetTotalRows(int rows) { m_totalRows = rows; }

	void SetAreaWidth(float width) { m_areaWidth = width; }
	void SetAreaHeight(float height) { m_areaHeight = height; }
	void SetPadding(float padding) { m_padding = padding; }

	void SetAreaTopLeft(const Vector2F& pos) { m_areaTopLeft = pos; }

	void OnDestroy();
private:

	std::vector<std::vector<JDGameObject::Content::Grid*>> m_gridArray; // ������ �׸��� �����̳�

	// ���� : 7ĭ�̶�� �ϸ�, 0~6���� ������ ��

	float m_areaWidth; // �׸��带 ��ġ�� ��ü ������ ũ��
	float m_areaHeight; // �׸��带 ��ġ�� ��ü ������ ũ��

	// �׸��带 ��ġ�� ������ ���� ��� ���� ��ǥ
	Vector2F m_areaTopLeft;

	// ���� �� ������ ���� (����)
	float m_padding; // ĭ ����

	// �� �׸��� ����
	int m_totalCols; // ��ü Ÿ�� ����
	int m_totalRows; // ��ü Ÿ�� ����

	// Ȯ�� ����
	int m_startCols; // ���� ���� Ÿ�� ~����(��)
	int m_startRows; // ���� ���� Ÿ�� ~����(��)
	int m_endCols; // ���� ���� Ÿ�� ~����(��)
	int m_endRows; // ���� ���� Ÿ�� ~����(��)
	int m_expandedCount;
	int m_choiceCount;  // Ȯ�� ���� ���� ���� Ƚ�� (ŵ ����)
};

