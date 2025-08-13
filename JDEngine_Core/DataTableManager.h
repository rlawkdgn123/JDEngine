#pragma once
#include "AssetManager.h"
#include "JDGlobal.h"
class DataTableManager
{
public:
    using BuildingStats = JDGlobal::Contents::BuildingStats;
    using HouseStats = JDGlobal::Contents::HouseStats;
    using Resource = JDGlobal::Contents::Resource;
    using StartResources = JDGlobal::Contents::StartResources;
    using SordierTypeStats = JDGlobal::Contents::SordierTypeStats;
    using WaveStats = JDGlobal::Contents::WaveStats;
    using ExpeditionStats = JDGlobal::Contents::ExpeditionStats;
    
public:
    static DataTableManager& Instance()
    {
        static DataTableManager instance;
        return instance;
    }

public:
    void Initalize();
    void GetStartResourcesTable(Resource& resource, int& population) {
        resource.m_food = m_startResourceTable.m_food;
        resource.m_wood = m_startResourceTable.m_wood;
        resource.m_mineral = m_startResourceTable.m_mineral;
        population = m_startResourceTable.m_population;
    }
    void GetFishingSpotTable(BuildingStats& table) { table = m_fishingSpotTable; }
    BuildingStats GetFishingSpotTableInfo() const { return m_fishingSpotTable; }
    void GetLumberMillTable(BuildingStats& table) { table = m_lumberMillTable; }
    BuildingStats GetLumbermillTableInfo() { return m_lumberMillTable; }
    void GetMineTable(BuildingStats& table) const { table = m_mineTable; }
    BuildingStats GetMineTableInfo() { return m_mineTable; }
    void GetHouseTable(HouseStats& table) { table = m_houseTable; }
    HouseStats GetHouseTableInfo() const { return m_houseTable; }

    void GetSordierTypeTable(SordierTypeStats& table) { table = m_soldierTypeTable; }
    SordierTypeStats GetSordierTypeTableInfo() const { return m_soldierTypeTable; }
    void GetWaveTable(WaveStats& table) { table = m_waveTable; }
    WaveStats GetWaveTableInfo() const { return m_waveTable; }
    void GetExpeditionTable(ExpeditionStats& table) { table = m_expeditionTable; }
    ExpeditionStats GetExpeditionTableInfo() const { return m_expeditionTable; }

    // CSV Parsing
    bool LoadCSV(const std::string& name, const std::string& filePath);
    bool LoadAllCSV();
    std::string GetCSV(const std::string& key) const;

    void PrintAllTable();
private:
    // CSV �Ľ� �Լ�
    void ParseTestTable();      // �׽�Ʈ�� ���̺�
    //void ParseBuildingTable(BuildingStats& stats, JDGlobal::Contents::BuildingType type);
    void ParseStartResourceTable();  // ���� �ڿ� ���̺�
    void ParseFishingSpotTable();
    void ParseLumberMillTable();
    void ParseMineTable();
    void ParseHouseTable();
    void ParseSoldierTypeTable(); // ����� ���� ���̺�
    void ParseWaveTable();       // �� ���̺�� ���� ���̺�
    void ParseExpeditionTable(); // ���� ���� ���̺�

    
private:
    // string�� RAII�� �̹� ����Ǿ� �����Ƿ�, smart_ptr�� �����ų �ʿ䰡 ����.
    std::unordered_map<std::string, std::string> m_csvFileMap; // csv ����
    
    BuildingStats m_fishingSpotTable;
    BuildingStats m_lumberMillTable;
    BuildingStats m_mineTable;
    HouseStats m_houseTable;
    SordierTypeStats m_soldierTypeTable;
    WaveStats m_waveTable;
    ExpeditionStats m_expeditionTable; // ���� ���� ����.

    StartResources m_startResourceTable;

    DataTableManager() = default;
    ~DataTableManager() = default;
    DataTableManager(const DataTableManager&) = delete;
    DataTableManager& operator=(const DataTableManager&) = delete;
   
};

