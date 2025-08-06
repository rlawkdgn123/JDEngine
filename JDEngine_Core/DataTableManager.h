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

    // CSV �Ľ� �Լ�
    void ParseTestTable();      // �׽�Ʈ�� ���̺�
    //void ParseBuildingTable(BuildingStats& stats, JDGlobal::Contents::BuildingType type);
    void ParseStartResourceTable();  // ���� �ڿ� ���̺�
    void ParseFishingSpotTable(BuildingStats& stats);
    void ParseLumberMillTable(BuildingStats& stats);
    void ParseMineTable(BuildingStats& stats);
    void ParseHouseTable(HouseStats& stats);
    void ParseCatTable();       // ��� ����� ���� ���̺�
    void ParseArmyTable();      // ������ ����� ���� ���̺�
    void ParseEnemyTable();     // �� ���� & ���� ���̺�


private:
    DataTableManager() = default;
    ~DataTableManager() = default;
    DataTableManager(const DataTableManager&) = delete;
    DataTableManager& operator=(const DataTableManager&) = delete;
   
};

