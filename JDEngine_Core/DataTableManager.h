#pragma once
#include "AssetManager.h"
#include "JDGlobal.h"
class DataTableManager
{
public:
    using BuildingStats = JDGlobal::Contents::BuildingStats;
    using HouseStats = JDGlobal::Contents::HouseStats;
public:
    static DataTableManager& Instance()
    {
        static DataTableManager instance;
        return instance;
    }
public:
    bool Initalize();

    // CSV 파싱 함수
    void ParseTestTable();      // 테스트용 테이블
    //void ParseBuildingTable(BuildingStats& stats, JDGlobal::Contents::BuildingType type);
    void ParseStartResourceTable();  // 시작 자원 테이블
    void ParseFishingSpotTable(BuildingStats& stats);
    void ParseLumberMillTable(BuildingStats& stats);
    void ParseMineTable(BuildingStats& stats);
    void ParseHouseTable(HouseStats& stats);
    void ParseCatTable();       // 운영용 고양이 스탯 테이블
    void ParseArmyTable();      // 전투용 고양이 스탯 테이블
    void ParseEnemyTable();     // 적 스탯 & 물량 테이블


private:
    DataTableManager() = default;
    ~DataTableManager() = default;
    DataTableManager(const DataTableManager&) = delete;
    DataTableManager& operator=(const DataTableManager&) = delete;
   
};

