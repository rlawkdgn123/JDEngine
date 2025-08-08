#pragma once
#include "Grid.h"

namespace JDGameObject { namespace Content { class Grid; } }

class BuildSystem
{
public:
	BuildSystem() : m_startCols(1), m_startRows(1), m_endCols(3), m_endRows(3), m_choiceCount(4) {
		m_expandedCount = (m_endCols - m_startCols + 1) * (m_endRows - m_startRows + 1);
	}
public:

	void SetStartGridMat(int sRow, int sCol, int eRow, int eCol) { 
		m_startRows = sRow; m_startCols = sCol;
		m_endRows = eRow; m_endCols = eCol;
	}
	void CreateGrid(JDScene::SceneBase* curScene); // 전체 그리드 그리기
	void ExpandTile(JDGameObject::Content::Grid* tile); // 건축 가능한 타일 확장하기
	void AddChoiceCount(int add) { m_choiceCount += add; }
private:
	// 주의 : 7칸이라고 하면, 0~6임을 감안할 것
	int m_startCols;
	int m_startRows;
	int m_endCols;
	int m_endRows;
	int m_expandedCount;
	int m_choiceCount;  // 확장 지역 선택 남은 횟수 (킵 가능)
};

