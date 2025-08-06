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
    void Initalize();
    void GetFishingSpotTable(BuildingStats& table) { table = m_fishingSpotTable; }
    void GetLumberMillTable(BuildingStats& table) { table = m_lumberMillTable; }
    void GetMineTable(BuildingStats& table) { table = m_mineTable; }
    void GetHouseTable(HouseStats& table) { table = m_houseTable; }

    // CSV Parsing
    bool LoadCSV(const std::string& name, const std::string& filePath);
    bool LoadAllCSV();
    std::string GetCSV(const std::string& key) const;

private:
    // CSV 파싱 함수
    void ParseTestTable();      // 테스트용 테이블
    //void ParseBuildingTable(BuildingStats& stats, JDGlobal::Contents::BuildingType type);
    void ParseStartResourceTable();  // 시작 자원 테이블
    void ParseFishingSpotTable();
    void ParseLumberMillTable();
    void ParseMineTable();
    void ParseHouseTable();
    void ParseCatTable();       // 운영용 고양이 스탯 테이블
    void ParseArmyTable();      // 전투용 고양이 스탯 테이블
    void ParseEnemyTable();     // 적 스탯 & 물량 테이블
private:
    // string은 RAII가 이미 적용되어 있으므로, smart_ptr을 적용시킬 필요가 없다.
    std::unordered_map<std::string, std::string> m_csvFileMap; // csv 파일
    
    BuildingStats m_fishingSpotTable;
    BuildingStats m_lumberMillTable;
    BuildingStats m_mineTable;
    HouseStats m_houseTable;

    DataTableManager() = default;
    ~DataTableManager() = default;
    DataTableManager(const DataTableManager&) = delete;
    DataTableManager& operator=(const DataTableManager&) = delete;
   
};

