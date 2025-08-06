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
    // CSV �Ľ� �Լ�
    void ParseTestTable();      // �׽�Ʈ�� ���̺�
    //void ParseBuildingTable(BuildingStats& stats, JDGlobal::Contents::BuildingType type);
    void ParseStartResourceTable();  // ���� �ڿ� ���̺�
    void ParseFishingSpotTable();
    void ParseLumberMillTable();
    void ParseMineTable();
    void ParseHouseTable();
    void ParseCatTable();       // ��� ����� ���� ���̺�
    void ParseArmyTable();      // ������ ����� ���� ���̺�
    void ParseEnemyTable();     // �� ���� & ���� ���̺�
private:
    // string�� RAII�� �̹� ����Ǿ� �����Ƿ�, smart_ptr�� �����ų �ʿ䰡 ����.
    std::unordered_map<std::string, std::string> m_csvFileMap; // csv ����
    
    BuildingStats m_fishingSpotTable;
    BuildingStats m_lumberMillTable;
    BuildingStats m_mineTable;
    HouseStats m_houseTable;

    DataTableManager() = default;
    ~DataTableManager() = default;
    DataTableManager(const DataTableManager&) = delete;
    DataTableManager& operator=(const DataTableManager&) = delete;
   
};

