#include "pch.h"
#include "framework.h"
#include "DataTableManager.h"

using namespace std;
using namespace JDGlobal::Contents;
bool DataTableManager::Initalize()
{
	return false;
}

void DataTableManager::ParseTestTable()
{

	cout << "테스트 진입" << endl;
	const std::string& csv = AssetManager::Instance().GetCSV("동물소리"); // 동물소리.CSV

	std::istringstream stream(csv); // CSV 문자열 형태에 맞는 입력 스트림 객체를 생성하는 코드.
	std::string line;
	string anim;
	string sound;
	string size;
	
	// 첫 줄은 헤더(컬럼명)라서 건너뜀
	std::getline(stream, line);

	while (std::getline(stream, line))
	{
		if (line.empty()) continue; // 빈 줄 무시
			
		std::istringstream lineStream(line);
		std::string cell;

		// 첫 번째 빈 셀 무시
		std::getline(lineStream, cell, ','); // 버림

		std::getline(lineStream, cell, ','); anim = cell;
		std::getline(lineStream, cell, ','); sound = cell;
		std::getline(lineStream, cell, ','); size = cell;

		// 빈 데이터 줄 무시
		if (anim.empty() || sound.empty() || size.empty())
			continue;

		cout << "data.anim :" << anim << endl;
		cout << "data.sound :" << sound << endl;
		cout << "data.size :" << size << endl;
	}
}

void DataTableManager::ParseStartResourceTable()
{
	
}

void DataTableManager::ParseFishingSpotTable(BuildingStats& stats)
{
    std::cout << "데이터 파싱 진입 - FishingSpot" << std::endl;
    const std::string& csv = AssetManager::Instance().GetCSV("FishingSpot");
    std::istringstream stream(csv);
    std::string dataLine;

    // 헤더와 빈 줄을 건너뛰는 초기 루프
    // 첫 번째 유효한 데이터 행("Food,...")을 찾을 때까지 계속 읽습니다.
    do {
        if (!std::getline(stream, dataLine)) {
            throw std::runtime_error("[ParseFishingSpotTable] 파일에서 유효한 데이터 행을 찾을 수 없습니다.");
        }
    } while (JDUtil::trim(dataLine).rfind("Food", 0) != 0);

    // 이제 첫 번째 데이터 행(Food)에 도달했으므로 3개의 데이터 행을 순차적으로 파싱합니다.
    for (int i = 0; i < RESOURCE_COUNT; ++i)
    {
        std::vector<std::string> tokens;
        std::istringstream lineStream(dataLine);
        std::string cell;

        while (std::getline(lineStream, cell, ',')) {
            tokens.push_back(JDUtil::trim(cell));
        }

        if (tokens.size() < 11) {
            throw std::runtime_error("[ParseFishingSpotTable] CSV 포맷 오류 : 열 개수 부족");
        }

        std::cout << "[DEBUG] 자원 데이터 줄: \"" << dataLine << "\"" << std::endl;

        const std::string& resourceName = tokens[0];
        auto toInt = [](const std::string& s) { return std::stoi(s); };

        if (resourceName == "Food") {
            stats.m_initResource.m_food = toInt(tokens[1]);
            for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                stats.m_upgradeCost[level].m_food = toInt(tokens[2 + level]);
                stats.m_resourceGenPerSec[level].m_food = toInt(tokens[5 + level]);
                stats.m_resourceSubPerSec[level].m_food = toInt(tokens[8 + level]);
            }
        }
        else if (resourceName == "Wood") {
            stats.m_initResource.m_wood = toInt(tokens[1]);
            for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                stats.m_upgradeCost[level].m_wood = toInt(tokens[2 + level]);
                stats.m_resourceGenPerSec[level].m_wood = toInt(tokens[5 + level]);
                stats.m_resourceSubPerSec[level].m_wood = toInt(tokens[8 + level]);
            }
        }
        else if (resourceName == "Mineral") {
            stats.m_initResource.m_mineral = toInt(tokens[1]);
            for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                stats.m_upgradeCost[level].m_mineral = toInt(tokens[2 + level]);
                stats.m_resourceGenPerSec[level].m_mineral = toInt(tokens[5 + level]);
                stats.m_resourceSubPerSec[level].m_mineral = toInt(tokens[8 + level]);
            }
        }

        // 다음 데이터 행을 읽습니다. (마지막 행이 아니면)
        if (i < RESOURCE_COUNT - 1) {
            if (!std::getline(stream, dataLine)) {
                throw std::runtime_error("[ParseFishingSpotTable] 예상보다 데이터 행이 적습니다.");
            }
        }
    }
}


void DataTableManager::ParseLumberMillTable(BuildingStats& stats)
{
    std::cout << "데이터 파싱 진입 - LumberMill" << std::endl;
    const std::string& csv = AssetManager::Instance().GetCSV("LumberMill");
    std::istringstream stream(csv);
    std::string dataLine;

    do {
        if (!std::getline(stream, dataLine)) {
            throw std::runtime_error("[ParseLumberMillTable] 파일에서 유효한 데이터 행을 찾을 수 없습니다.");
        }
    } while (JDUtil::trim(dataLine).rfind("Food", 0) != 0);

    for (int i = 0; i < RESOURCE_COUNT; ++i)
    {
        std::vector<std::string> tokens;
        std::istringstream lineStream(dataLine);
        std::string cell;

        while (std::getline(lineStream, cell, ',')) {
            tokens.push_back(JDUtil::trim(cell));
        }

        if (tokens.size() < 11) {
            throw std::runtime_error("[ParseLumberMillTable] CSV 포맷 오류 : 열 개수 부족");
        }

        std::cout << "[DEBUG] 자원 데이터 줄: \"" << dataLine << "\"" << std::endl;

        const std::string& resourceName = tokens[0];
        auto toInt = [](const std::string& s) { return std::stoi(s); };

        if (resourceName == "Food") {
            stats.m_initResource.m_food = toInt(tokens[1]);
            for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                stats.m_upgradeCost[level].m_food = toInt(tokens[2 + level]);
                stats.m_resourceGenPerSec[level].m_food = toInt(tokens[5 + level]);
                stats.m_resourceSubPerSec[level].m_food = toInt(tokens[8 + level]);
            }
        }
        else if (resourceName == "Wood") {
            stats.m_initResource.m_wood = toInt(tokens[1]);
            for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                stats.m_upgradeCost[level].m_wood = toInt(tokens[2 + level]);
                stats.m_resourceGenPerSec[level].m_wood = toInt(tokens[5 + level]);
                stats.m_resourceSubPerSec[level].m_wood = toInt(tokens[8 + level]);
            }
        }
        else if (resourceName == "Mineral") {
            stats.m_initResource.m_mineral = toInt(tokens[1]);
            for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                stats.m_upgradeCost[level].m_mineral = toInt(tokens[2 + level]);
                stats.m_resourceGenPerSec[level].m_mineral = toInt(tokens[5 + level]);
                stats.m_resourceSubPerSec[level].m_mineral = toInt(tokens[8 + level]);
            }
        }

        if (i < RESOURCE_COUNT - 1) {
            if (!std::getline(stream, dataLine)) {
                throw std::runtime_error("[ParseLumberMillTable] 예상보다 데이터 행이 적습니다.");
            }
        }
    }
}


void DataTableManager::ParseMineTable(BuildingStats& stats)
{
    std::cout << "데이터 파싱 진입 - Mine" << std::endl;
    const std::string& csv = AssetManager::Instance().GetCSV("Mine"); // CSV 파일 가져오기
    std::istringstream stream(csv);
    std::string dataLine;

    do {
        if (!std::getline(stream, dataLine)) {
            throw std::runtime_error("[ParseMineTable] 파일에서 유효한 데이터 행을 찾을 수 없습니다.");
        }
    } while (JDUtil::trim(dataLine).rfind("Food", 0) != 0);

    for (int i = 0; i < RESOURCE_COUNT; ++i)
    {
        std::vector<std::string> tokens;
        std::istringstream lineStream(dataLine);
        std::string cell;

        while (std::getline(lineStream, cell, ',')) {
            tokens.push_back(JDUtil::trim(cell));
        }

        if (tokens.size() < 11) {
            throw std::runtime_error("[ParseMineTable] CSV 포맷 오류 : 열 개수 부족");
        }

        std::cout << "[DEBUG] 자원 데이터 줄: \"" << dataLine << "\"" << std::endl;

        const std::string& resourceName = tokens[0];
        auto toInt = [](const std::string& s) { return std::stoi(s); };

        if (resourceName == "Food") {
            stats.m_initResource.m_food = toInt(tokens[1]);
            for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                stats.m_upgradeCost[level].m_food = toInt(tokens[2 + level]);
                stats.m_resourceGenPerSec[level].m_food = toInt(tokens[5 + level]);
                stats.m_resourceSubPerSec[level].m_food = toInt(tokens[8 + level]);
            }
        }
        else if (resourceName == "Wood") {
            stats.m_initResource.m_wood = toInt(tokens[1]);
            for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                stats.m_upgradeCost[level].m_wood = toInt(tokens[2 + level]);
                stats.m_resourceGenPerSec[level].m_wood = toInt(tokens[5 + level]);
                stats.m_resourceSubPerSec[level].m_wood = toInt(tokens[8 + level]);
            }
        }
        else if (resourceName == "Mineral") {
            stats.m_initResource.m_mineral = toInt(tokens[1]);
            for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                stats.m_upgradeCost[level].m_mineral = toInt(tokens[2 + level]);
                stats.m_resourceGenPerSec[level].m_mineral = toInt(tokens[5 + level]);
                stats.m_resourceSubPerSec[level].m_mineral = toInt(tokens[8 + level]);
            }
        }

        if (i < RESOURCE_COUNT - 1) {
            if (!std::getline(stream, dataLine)) {
                throw std::runtime_error("[ParseMineTable] 예상보다 데이터 행이 적습니다.");
            }
        }
    }
}

void DataTableManager::ParseHouseTable(HouseStats& stats)
{
    std::cout << "데이터 파싱 진입 - House" << std::endl;
    const std::string& csv = AssetManager::Instance().GetCSV("House");
    std::istringstream stream(csv);
    std::string dataLine;

    int linesToParse = 4; // Food, Wood, Mineral, Population 총 4개의 데이터 행을 파싱해야 합니다.
    int linesParsed = 0;

    // 파일의 모든 행을 순차적으로 읽습니다.
    while (linesParsed < linesToParse && std::getline(stream, dataLine))
    {
        std::vector<std::string> tokens;
        std::istringstream lineStream(dataLine);
        std::string cell;

        // 쉼표로 각 셀을 분리합니다.
        while (std::getline(lineStream, cell, ',')) {
            tokens.push_back(JDUtil::trim(cell));
        }

        // 빈 줄 또는 첫 번째 셀이 빈 행은 건너뜁니다.
        if (tokens.empty() || tokens[0].empty()) {
            continue;
        }

        const std::string& rowName = tokens[0];
        auto toInt = [](const std::string& s) { return std::stoi(s); };

        if (rowName == "Food") {
            if (tokens.size() < 1 + MAX_GAME_LEVEL + MAX_GAME_LEVEL) {
                throw std::runtime_error("[ParseHouseTable] CSV 포맷 오류 : Food 열 개수 부족");
            }
            for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                stats.m_upgradeCost[level].m_food = toInt(tokens[4 + level]);
                stats.m_resourceSubPerSec[level].m_food = toInt(tokens[7 + level]);
            }
            linesParsed++;
        }
        else if (rowName == "Wood") {
            if (tokens.size() < 1 + MAX_GAME_LEVEL + MAX_GAME_LEVEL) {
                throw std::runtime_error("[ParseHouseTable] CSV 포맷 오류 : Wood 열 개수 부족");
            }
            for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                stats.m_upgradeCost[level].m_wood = toInt(tokens[4 + level]);
                stats.m_resourceSubPerSec[level].m_wood = toInt(tokens[7 + level]);
            }
            linesParsed++;
        }
        else if (rowName == "Mineral") {
            if (tokens.size() < 1 + MAX_GAME_LEVEL + MAX_GAME_LEVEL) {
                throw std::runtime_error("[ParseHouseTable] CSV 포맷 오류 : Mineral 열 개수 부족");
            }
            for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                stats.m_upgradeCost[level].m_mineral = toInt(tokens[4 + level]);
                stats.m_resourceSubPerSec[level].m_mineral = toInt(tokens[7 + level]);
            }
            linesParsed++;
        }
        else if (rowName == "Population") {
            if (tokens.size() < 1 + MAX_GAME_LEVEL) { // 1(행 이름) + 3(인구)
                throw std::runtime_error("[ParseHouseTable] CSV 포맷 오류 : Population 열 개수 부족");
            }
            for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                stats.m_initPopulation[level] = toInt(tokens[1 + level]);
            }
            linesParsed++;
        }
        else {
            // 알 수 없는 행은 경고를 출력하고 무시합니다.
            std::cerr << "[WARNING] 알 수 없는 행 발견: " << rowName << ". 해당 줄을 건너뜁니다." << std::endl;
        }
    }

    // 필요한 모든 데이터 행을 다 읽지 못하고 파일이 끝났다면 예외를 발생시킵니다.
    if (linesParsed < linesToParse) {
        throw std::runtime_error("[ParseHouseTable] 예상보다 데이터 행이 적습니다.");
    }
}

/*
void DataTableManager::ParseBuildingTable(BuildingStats& stats, JDGlobal::Contents::BuildingType type)
{
    std::cout << "빌딩 테스트 진입" << std::endl;
    const std::string& csv = AssetManager::Instance().GetCSV("Building_Test");
    std::istringstream stream(csv);
    std::string line;
    std::string targetName;

    switch (type) {
    case BuildingType::FishingSpot: targetName = "FishingSpot"; break;
    case BuildingType::LumberMill: targetName = "LumberMill"; break;
    case BuildingType::Mine:        targetName = "Mine"; break;
    default: throw std::runtime_error("[DataTableManager] Unknown BuildingType");
    }
    std::cout << "빌딩명 : " << targetName << std::endl;

    // 시트 시작 찾기
    while (std::getline(stream, line))
    {
        // C++20에서는 starts_with()를 사용할 수 있습니다.
        // 여기서는 rfind를 사용해 "문자열이 targetName으로 시작하는지" 확인합니다.
        if (JDUtil::trim(line).rfind(targetName, 0) == 0)
        {
            std::cout << "[DEBUG] 타겟 빌딩 시트 시작 발견: " << line << std::endl;

            // 3줄 읽기 (Food, Wood, Mineral)
            for (int i = 0; i < RESOURCE_COUNT; ++i)
            {
                std::string dataLine;
                std::vector<std::string> tokens;

                // 유효한 데이터가 있는 행을 찾을 때까지 계속 읽습니다.
                // 1. getline에 성공해야 하고
                // 2. 행의 trim된 내용이 비어있지 않아야 하고
                // 3. 첫 번째 셀(토큰)이 비어있지 않아야 합니다. (헤더 줄이나 빈 줄을 건너뜁니다)
                do {
                    if (!std::getline(stream, dataLine)) {
                        throw std::runtime_error("[ParseBuildingTable] CSV 파일의 데이터 끝에 도달했습니다.");
                    }
                    std::istringstream lineStream(dataLine);
                    std::string cell;
                    tokens.clear();
                    while (std::getline(lineStream, cell, ',')) {
                        tokens.push_back(JDUtil::trim(cell));
                    }
                } while (JDUtil::trim(dataLine).empty() || tokens.empty() || tokens[0].empty());

                std::cout << "[DEBUG] 자원 데이터 줄: \"" << dataLine << "\"" << std::endl;

                // 이제 유효한 데이터 행을 찾았으므로 파싱을 진행합니다.
                if (tokens.size() < 11) {
                    throw std::runtime_error("[ParseBuildingTable] CSV 포맷 오류 : 열 개수 부족");
                }

                const std::string& resourceName = tokens[0];
                auto toInt = [](const std::string& s) { return std::stoi(s); };

                if (resourceName == "Food") {
                    stats.m_initResource.m_food = toInt(tokens[1]);
                    for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                        stats.m_upgradeCost[level].m_food = toInt(tokens[2 + level]);
                        stats.m_resourceGenPerSec[level].m_food = toInt(tokens[5 + level]);
                        stats.m_resourceSubPerSec[level].m_food = toInt(tokens[8 + level]);
                    }
                }
                else if (resourceName == "Wood") {
                    stats.m_initResource.m_wood = toInt(tokens[1]);
                    for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                        stats.m_upgradeCost[level].m_wood = toInt(tokens[2 + level]);
                        stats.m_resourceGenPerSec[level].m_wood = toInt(tokens[5 + level]);
                        stats.m_resourceSubPerSec[level].m_wood = toInt(tokens[8 + level]);
                    }
                }
                else if (resourceName == "Mineral") {
                    stats.m_initResource.m_mineral = toInt(tokens[1]);
                    for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                        stats.m_upgradeCost[level].m_mineral = toInt(tokens[2 + level]);
                        stats.m_resourceGenPerSec[level].m_mineral = toInt(tokens[5 + level]);
                        stats.m_resourceSubPerSec[level].m_mineral = toInt(tokens[8 + level]);
                    }
                }
                else {
                    std::cerr << "[WARNING] 알 수 없는 자원명 발견: " << resourceName << ". 해당 줄을 건너뜁니다." << std::endl;
                    // 유효하지 않은 행이므로 카운터를 되돌립니다.
                    --i;
                    continue;
                }
            }
            return; // 해당 빌딩 파싱 완료
        }
    }
}
*/

void DataTableManager::ParseCatTable()
{
}

void DataTableManager::ParseArmyTable()
{
}

void DataTableManager::ParseEnemyTable()
{
}

