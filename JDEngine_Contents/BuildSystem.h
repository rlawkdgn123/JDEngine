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
	void CreateGrid(JDScene::SceneBase* curScene); // 전체 그리드 그리기
	void DestroyGrid(JDScene::SceneBase* curScene);
	void ExpandTile(JDGameObject::Content::Grid* tile); // 건축 가능한 타일 확장하기
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

	std::vector<std::vector<JDGameObject::Content::Grid*>> m_gridArray; // 생성된 그리드 컨테이너

	// 주의 : 7칸이라고 하면, 0~6임을 감안할 것

	float m_areaWidth; // 그리드를 배치할 전체 영역의 크기
	float m_areaHeight; // 그리드를 배치할 전체 영역의 크기

	// 그리드를 배치할 영역의 좌측 상단 월드 좌표
	Vector2F m_areaTopLeft;

	// 셀과 셀 사이의 간격 (여백)
	float m_padding; // 칸 간격

	// 총 그리드 개수
	int m_totalCols; // 전체 타일 개수
	int m_totalRows; // 전체 타일 개수

	// 확장 관련
	int m_startCols; // 시작 점유 타일 ~부터(열)
	int m_startRows; // 시작 점유 타일 ~부터(행)
	int m_endCols; // 시작 점유 타일 ~까지(열)
	int m_endRows; // 시작 점유 타일 ~까지(행)
	int m_expandedCount;
	int m_choiceCount;  // 확장 지역 선택 남은 횟수 (킵 가능)
};

