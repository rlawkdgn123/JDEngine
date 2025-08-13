#include "pch.h"
#include "framework.h"
#include "DataTableManager.h"

using namespace std;
using namespace JDGlobal::Contents;
void DataTableManager::Initalize()
{
    cout << "데이터 테이블 이니셜라이즈 진입" << endl;
    LoadAllCSV();
    ParseStartResourceTable();
    ParseFishingSpotTable();
    ParseLumberMillTable();
    ParseMineTable();
    ParseHouseTable();
    ParseSoldierTypeTable();
    ParseWaveTable();
    ParseExpeditionTable();
}
bool DataTableManager::LoadCSV(const std::string& name, const std::string& filePath)
{
    std::wifstream wifs(filePath);
    if (!wifs.is_open())
    {
        std::cout << "[ERROR] CSV 파일 열기 실패" << filePath << std::endl;
        return false;
    }

    // 파일 인코딩이 UTF-8이면 아래 줄 활성화 필요 (시스템에 따라 생략 가능)
    //wifs.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));


    std::wstringstream wbuffer;
    wbuffer << wifs.rdbuf(); // 전체 파일 내용을 읽음

    std::wstring wcontent = wbuffer.str();
    std::string content(wcontent.begin(), wcontent.end());

    m_csvFileMap[name] = content;
    return true;
}

bool DataTableManager::LoadAllCSV()
{
    namespace fs = std::filesystem;
    bool foundCSV = false;

    for (const auto& entry : fs::directory_iterator(csvFolderPath)) {
        if (!entry.is_regular_file()) continue;

        fs::path path = entry.path();

        if (path.extension() == L".csv" || path.extension() == L".CSV") {
            foundCSV = true;
            std::string key = path.stem().string(); // 키는 확장자 제외 파일명
            std::string fullPath = path.string(); // 전체 경로 전달

            if (!LoadCSV(key, fullPath)) {
                std::cout << "[ERROR] CSV 파일 로드 실패 : " << path.filename() << std::endl;
                continue;
            }

            std::cout << "[INFO] Document 등록됨 : " << path.filename() << std::endl;
        }
    }
    if (!foundCSV) {
        std::cout << "[WARNNING] CSV 폴더에 로드할 파일이 없습니다. " << csvFolderPath << std::endl;
        return false;
    }

    return true;
}

// CSV 파일명 반환 함수. ex) Doc.CSV
std::string DataTableManager::GetCSV(const std::string& key) const
{
    auto it = m_csvFileMap.find(key); // find 시 찾음 찾지 못하면 end됨
    if (it != m_csvFileMap.end()) {
        return it->second;
    }

    throw std::runtime_error("[AssetManager] GetCSV: 존재하지 않는 키: " + key); // 예외처리
}

void DataTableManager::ParseTestTable()
{

	cout << "테스트 진입" << endl;
	const std::string& csv = GetCSV("동물소리"); // 동물소리.CSV

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
    std::cout << "데이터 파싱 진입 - StartResource" << std::endl;
    const std::string& csv = GetCSV("StartResource");
    std::istringstream stream(csv);
    std::string dataLine;
    int linesParsed = 0; // 파싱된 행의 수를 세는 변수

    // 'Food' 행을 찾을 때까지 헤더와 빈 줄을 건너뜁니다.
    do {
        if (!std::getline(stream, dataLine)) {
            throw std::runtime_error("[ParseStartResourceTable] 파일에서 유효한 데이터 행을 찾을 수 없습니다.");
        }
    } while (JDUtil::trim(dataLine).rfind("Food", 0) != 0);

    // Food, Wood, Mineral, Population 4개의 데이터 행을 순차적으로 파싱합니다.
    int linesToParse = 4;
    while (linesParsed < linesToParse && !dataLine.empty())
    {
        std::vector<std::string> tokens;
        std::istringstream lineStream(dataLine);
        std::string cell;

        while (std::getline(lineStream, cell, ',')) {
            tokens.push_back(JDUtil::trim(cell));
        }

        // CSV 포맷 오류: 열 개수가 부족한지 확인
        if (tokens.size() < 2) {
            throw std::runtime_error("[ParseStartResourceTable] CSV 포맷 오류 : 열 개수 부족");
        }

        std::cout << "[DEBUG] 자원 데이터 줄: \"" << dataLine << "\"" << std::endl;

        const std::string& resourceName = tokens[0];
        auto toInt = [](const std::string& s) { return std::stoi(s); };

        if (resourceName == "Food") {
            m_startResourceTable.m_food = toInt(tokens[1]);
        }
        else if (resourceName == "Wood") {
            m_startResourceTable.m_wood = toInt(tokens[1]);
        }
        else if (resourceName == "Mineral") {
            m_startResourceTable.m_mineral = toInt(tokens[1]);
        }
        else if (resourceName == "Population") {
            m_startResourceTable.m_population = toInt(tokens[1]);
        }

        linesParsed++;

        // 다음 데이터 행을 읽습니다. (마지막 행이 아니면)
        if (linesParsed < linesToParse) {
            if (!std::getline(stream, dataLine)) {
                throw std::runtime_error("[ParseStartResourceTable] 예상보다 데이터 행이 적습니다.");
            }
        }
    }

    if (linesParsed < linesToParse) {
        throw std::runtime_error("[ParseStartResourceTable] 예상보다 데이터 행이 적습니다.");
    }
}

void DataTableManager::ParseFishingSpotTable()
{
    std::cout << "데이터 파싱 진입 - FishingSpot" << std::endl;
    const std::string& csv = GetCSV("FishingSpot");
    std::istringstream stream(csv);
    std::string dataLine;

    // 헤더와 빈 줄을 건너뛰는 초기 루프
    do {
        if (!std::getline(stream, dataLine)) {
            throw std::runtime_error("[ParseFishingSpotTable] 파일에서 유효한 데이터 행을 찾을 수 없습니다.");
        }
    } while (JDUtil::trim(dataLine).rfind("Food", 0) != 0);

    // 이제 첫 번째 데이터 행(Food)에 도달했으므로 3개의 데이터 행을 순차적으로 파싱
    for (int i = 0; i < RESOURCE_COUNT; ++i)
    {
        std::vector<std::string> tokens;
        std::istringstream lineStream(dataLine);
        std::string cell;

        while (std::getline(lineStream, cell, ',')) {
            tokens.push_back(JDUtil::trim(cell));
        }

        // MAX_GAME_LEVEL * 3(업글비용,획득량,유지비) + 2(자원명,초기자원) = 10 * 3 + 2 = 32
        if (tokens.size() < 32) {
            throw std::runtime_error("[ParseFishingSpotTable] CSV 포맷 오류 : 열 개수 부족");
        }

        std::cout << "[DEBUG] 자원 데이터 줄: \"" << dataLine << "\"" << std::endl;

        const std::string& resourceName = tokens[0];
        auto toInt = [](const std::string& s) { return std::stoi(s); };

        if (resourceName == "Food") {
            m_fishingSpotTable.m_initResource.m_food = toInt(tokens[1]);
            for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                m_fishingSpotTable.m_upgradeCost[level].m_food = toInt(tokens[2 + level]);
                m_fishingSpotTable.m_resourceGenPerSec[level].m_food = toInt(tokens[12 + level]);
                m_fishingSpotTable.m_resourceSubPerSec[level].m_food = -std::abs(toInt(tokens[22 + level]));
            }
        }
        else if (resourceName == "Wood") {
            m_fishingSpotTable.m_initResource.m_wood = toInt(tokens[1]);
            for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                m_fishingSpotTable.m_upgradeCost[level].m_wood = toInt(tokens[2 + level]);
                m_fishingSpotTable.m_resourceGenPerSec[level].m_wood = toInt(tokens[12 + level]);
                m_fishingSpotTable.m_resourceSubPerSec[level].m_wood = -std::abs(toInt(tokens[22 + level]));
            }
        }
        else if (resourceName == "Mineral") {
            m_fishingSpotTable.m_initResource.m_mineral = toInt(tokens[1]);
            for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                m_fishingSpotTable.m_upgradeCost[level].m_mineral = toInt(tokens[2 + level]);
                m_fishingSpotTable.m_resourceGenPerSec[level].m_mineral = toInt(tokens[12 + level]);
                m_fishingSpotTable.m_resourceSubPerSec[level].m_mineral = -std::abs(toInt(tokens[22 + level]));
            }
        }

        // 다음 데이터 행을 읽음 (마지막 행이 아니면)
        if (i < RESOURCE_COUNT - 1) {
            if (!std::getline(stream, dataLine)) {
                throw std::runtime_error("[ParseFishingSpotTable] 예상보다 데이터 행이 적습니다.");
            }
        }
    }
}


void DataTableManager::ParseLumberMillTable()
{
    std::cout << "데이터 파싱 진입 - LumberMill" << std::endl;
    const std::string& csv = GetCSV("LumberMill");
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

        // MAX_GAME_LEVEL * 3(업글비용,획득량,유지비) + 2(자원명,초기자원) = 10 * 3 + 2 = 32
        if (tokens.size() < 32) {
            throw std::runtime_error("[ParseLumberMillTable] CSV 포맷 오류 : 열 개수 부족");
        }

        std::cout << "[DEBUG] 자원 데이터 줄: \"" << dataLine << "\"" << std::endl;

        const std::string& resourceName = tokens[0];
        auto toInt = [](const std::string& s) { return std::stoi(s); };

        if (resourceName == "Food") {
            m_lumberMillTable.m_initResource.m_food = toInt(tokens[1]);
            for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                m_lumberMillTable.m_upgradeCost[level].m_food = toInt(tokens[2 + level]);
                m_lumberMillTable.m_resourceGenPerSec[level].m_food = toInt(tokens[12 + level]);
                m_lumberMillTable.m_resourceSubPerSec[level].m_food = -std::abs(toInt(tokens[22 + level]));
            }
        }
        else if (resourceName == "Wood") {
            m_lumberMillTable.m_initResource.m_wood = toInt(tokens[1]);
            for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                m_lumberMillTable.m_upgradeCost[level].m_wood = toInt(tokens[2 + level]);
                m_lumberMillTable.m_resourceGenPerSec[level].m_wood = toInt(tokens[12 + level]);
                m_lumberMillTable.m_resourceSubPerSec[level].m_wood = -std::abs(toInt(tokens[22 + level]));
            }
        }
        else if (resourceName == "Mineral") {
            m_lumberMillTable.m_initResource.m_mineral = toInt(tokens[1]);
            for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                m_lumberMillTable.m_upgradeCost[level].m_mineral = toInt(tokens[2 + level]);
                m_lumberMillTable.m_resourceGenPerSec[level].m_mineral = toInt(tokens[12 + level]);
                m_lumberMillTable.m_resourceSubPerSec[level].m_mineral = -std::abs(toInt(tokens[22 + level]));
            }
        }

        if (i < RESOURCE_COUNT - 1) {
            if (!std::getline(stream, dataLine)) {
                throw std::runtime_error("[ParseLumberMillTable] 예상보다 데이터 행이 적습니다.");
            }
        }
    }
}


void DataTableManager::ParseMineTable()
{
    std::cout << "데이터 파싱 진입 - Mine" << std::endl;
    const std::string& csv = GetCSV("Mine");
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

        // MAX_GAME_LEVEL * 3(업글비용,획득량,유지비) + 2(자원명,초기자원) = 10 * 3 + 2 = 32
        if (tokens.size() < 32) {
            throw std::runtime_error("[ParseMineTable] CSV 포맷 오류 : 열 개수 부족");
        }

        std::cout << "[DEBUG] 자원 데이터 줄: \"" << dataLine << "\"" << std::endl;

        const std::string& resourceName = tokens[0];
        auto toInt = [](const std::string& s) { return std::stoi(s); };

        if (resourceName == "Food") {
            m_mineTable.m_initResource.m_food = toInt(tokens[1]);
            for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                m_mineTable.m_upgradeCost[level].m_food = toInt(tokens[2 + level]);
                m_mineTable.m_resourceGenPerSec[level].m_food = toInt(tokens[12 + level]);
                m_mineTable.m_resourceSubPerSec[level].m_food = -std::abs(toInt(tokens[22 + level]));
            }
        }
        else if (resourceName == "Wood") {
            m_mineTable.m_initResource.m_wood = toInt(tokens[1]);
            for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                m_mineTable.m_upgradeCost[level].m_wood = toInt(tokens[2 + level]);
                m_mineTable.m_resourceGenPerSec[level].m_wood = toInt(tokens[12 + level]);
                m_mineTable.m_resourceSubPerSec[level].m_wood = -std::abs(toInt(tokens[22 + level]));
            }
        }
        else if (resourceName == "Mineral") {
            m_mineTable.m_initResource.m_mineral = toInt(tokens[1]);
            for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                m_mineTable.m_upgradeCost[level].m_mineral = toInt(tokens[2 + level]);
                m_mineTable.m_resourceGenPerSec[level].m_mineral = toInt(tokens[12 + level]);
                m_mineTable.m_resourceSubPerSec[level].m_mineral = -std::abs(toInt(tokens[22 + level]));
            }
        }

        if (i < RESOURCE_COUNT - 1) {
            if (!std::getline(stream, dataLine)) {
                throw std::runtime_error("[ParseMineTable] 예상보다 데이터 행이 적습니다.");
            }
        }
    }
}

void DataTableManager::ParseHouseTable()
{
    std::cout << "데이터 파싱 진입 - House" << std::endl;
    const std::string& csv = GetCSV("House");
    std::istringstream stream(csv);
    std::string dataLine;

    int linesToParse = 4; // Food, Wood, Mineral, Population 총 4개의 데이터 행을 파싱
    int linesParsed = 0;

    // 파일의 모든 행을 순차적으로 읽음
    while (linesParsed < linesToParse && std::getline(stream, dataLine))
    {
        std::vector<std::string> tokens;
        std::istringstream lineStream(dataLine);
        std::string cell;

        // 쉼표로 각 셀을 분리합니다.
        while (std::getline(lineStream, cell, ',')) {
            tokens.push_back(JDUtil::trim(cell));
        }

        // 빈 줄 또는 첫 번째 셀이 빈 행은 스킵
        if (tokens.empty() || tokens[0].empty()) {
            continue;
        }

        const std::string& rowName = tokens[0];
        auto toInt = [](const std::string& s) { return std::stoi(s); };

        if (rowName == "Food") {
            // 1(자원명) + 10(인구) + 10(업글비용) + 10(유지비) = 31
            if (tokens.size() < 31) {
                throw std::runtime_error("[ParseHouseTable] CSV 포맷 오류 : Food 열 개수 부족");
            }
            for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                m_houseTable.m_upgradeCost[level].m_food = toInt(tokens[11 + level]);
                m_houseTable.m_resourceSubPerSec[level].m_food = -std::abs(toInt(tokens[21 + level]));
            }
            linesParsed++;
        }
        else if (rowName == "Wood") {
            if (tokens.size() < 31) {
                throw std::runtime_error("[ParseHouseTable] CSV 포맷 오류 : Wood 열 개수 부족");
            }
            for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                m_houseTable.m_upgradeCost[level].m_wood = toInt(tokens[11 + level]);
                m_houseTable.m_resourceSubPerSec[level].m_wood = -std::abs(toInt(tokens[21 + level]));
            }
            linesParsed++;
        }
        else if (rowName == "Mineral") {
            if (tokens.size() < 31) {
                throw std::runtime_error("[ParseHouseTable] CSV 포맷 오류 : Mineral 열 개수 부족");
            }
            for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                m_houseTable.m_upgradeCost[level].m_mineral = toInt(tokens[11 + level]);
                m_houseTable.m_resourceSubPerSec[level].m_mineral = -std::abs(toInt(tokens[21 + level]));
            }
            linesParsed++;
        }
        else if (rowName == "Population") {
            // 1(행 이름) + 10(인구) = 11
            if (tokens.size() < 11) {
                throw std::runtime_error("[ParseHouseTable] CSV 포맷 오류 : Population 열 개수 부족");
            }
            for (int level = 0; level < MAX_GAME_LEVEL; ++level) {
                m_houseTable.m_initPopulation[level] = toInt(tokens[1 + level]);
            }
            linesParsed++;
        }
        else {
            // 알 수 없는 행은 경고를 출력하고 무시
            std::cerr << "[WARNING] 알 수 없는 행 발견: " << rowName << ". 해당 줄을 건너뜁니다." << std::endl;
        }
    }

    // 필요한 모든 데이터 행을 다 읽지 못하고 파일이 끝났다면 예외 발생
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


void DataTableManager::ParseSoldierTypeTable()
{
    std::cout << "데이터 파싱 진입 - SoldierType" << std::endl;
    const std::string& csv = GetCSV("SoldierType");
    std::istringstream stream(csv);
    std::string dataLine;
    int linesParsed = 0; // 파싱된 행의 수를 세는 변수

    // 'Food' 행을 찾을 때까지 헤더와 빈 줄을 건너뜁니다.
    do {
        if (!std::getline(stream, dataLine)) {
            throw std::runtime_error("[ParseStartResourceTable] 파일에서 유효한 데이터 행을 찾을 수 없습니다.");
        }
    } while (JDUtil::trim(dataLine).rfind("Food", 0) != 0);

    // Food, Wood, Mineral 3개의 데이터 행을 순차적으로 파싱합니다.
    int linesToParse = 3;
    while (linesParsed < linesToParse && !dataLine.empty())
    {
        std::vector<std::string> tokens;
        std::istringstream lineStream(dataLine);
        std::string cell;

        while (std::getline(lineStream, cell, ',')) {
            tokens.push_back(JDUtil::trim(cell));
        }

        // CSV 포맷 오류: 열 개수가 부족한지 확인
        if (tokens.size() < 3) {
            throw std::runtime_error("[ParseStartResourceTable] CSV 포맷 오류 : 열 개수 부족");
        }

        std::cout << "[DEBUG] 자원 데이터 줄: \"" << dataLine << "\"" << std::endl;

        const std::string& resourceName = tokens[0];
        auto toInt = [](const std::string& s) { return std::stoi(s); };

        if (resourceName == "Food") {
            m_soldierTypeTable.m_novice.m_food = toInt(tokens[1]);
            m_soldierTypeTable.m_expert.m_food = toInt(tokens[2]);
        }
        else if (resourceName == "Wood") {
            m_soldierTypeTable.m_novice.m_wood = toInt(tokens[1]);
            m_soldierTypeTable.m_expert.m_wood = toInt(tokens[2]);
        }
        else if (resourceName == "Mineral") {
            m_soldierTypeTable.m_novice.m_mineral = toInt(tokens[1]);
            m_soldierTypeTable.m_expert.m_mineral = toInt(tokens[2]);
        }

        linesParsed++;

        // 다음 데이터 행을 읽습니다. (마지막 행이 아니면)
        if (linesParsed < linesToParse) {
            if (!std::getline(stream, dataLine)) {
                throw std::runtime_error("[ParseStartResourceTable] 예상보다 데이터 행이 적습니다.");
            }
        }
    }

    if (linesParsed < linesToParse) {
        throw std::runtime_error("[ParseStartResourceTable] 예상보다 데이터 행이 적습니다.");
    }
}

void DataTableManager::ParseWaveTable()
{
    std::cout << "[DEBUG] 데이터 파싱 진입 - Wave" << std::endl;
    const std::string& csv = GetCSV("Wave");
    std::istringstream stream(csv);
    std::string dataLine;

    int linesToParse = 3; // Novice, Expert, Day 총 3개의 데이터 행을 파싱
    int linesParsed = 0;

    // 파일의 모든 행을 순차적으로 읽음
    while (linesParsed < linesToParse && std::getline(stream, dataLine))
    {
        std::cout << "[DEBUG] 읽은 CSV 행: \"" << dataLine << "\"" << std::endl;

        std::vector<std::string> tokens;
        std::istringstream lineStream(dataLine);
        std::string cell;

        // 쉼표로 각 셀을 분리
        while (std::getline(lineStream, cell, ',')) {
            tokens.push_back(JDUtil::trim(cell));
        }

        // 빈 줄 또는 첫 번째 셀이 빈 행은 스킵
        if (tokens.empty() || tokens[0].empty()) {
            std::cout << "[DEBUG] 빈 행 스킵" << std::endl;
            continue;
        }

        const std::string& rowName = tokens[0];
        std::cout << "[DEBUG] 현재 행 이름: " << rowName << std::endl;

        auto toInt = [](const std::string& s) { return std::stoi(s); };

        if (rowName == "Novice") {
            if (tokens.size() < MAX_GAME_WAVE_COUNT + 1) {
                throw std::runtime_error("[ParseWaveTable] CSV 포맷 오류 : Novice 열 개수 부족");
            }
            for (int wave = 0; wave < MAX_GAME_WAVE_COUNT; ++wave) {
                m_waveTable.m_novice[wave] = toInt(tokens[1 + wave]);
                std::cout << "[DEBUG] Novice[" << wave << "] = " << m_waveTable.m_novice[wave] << std::endl;
            }
            linesParsed++;
            std::cout << "[DEBUG] Novice 행 파싱 완료" << std::endl;
        }
        else if (rowName == "Expert") {
            if (tokens.size() < MAX_GAME_WAVE_COUNT + 1) {
                throw std::runtime_error("[ParseWaveTable] CSV 포맷 오류 : Expert 열 개수 부족");
            }
            for (int wave = 0; wave < MAX_GAME_WAVE_COUNT; ++wave) {
                m_waveTable.m_expert[wave] = toInt(tokens[1 + wave]);
                std::cout << "[DEBUG] Expert[" << wave << "] = " << m_waveTable.m_expert[wave] << std::endl;
            }
            linesParsed++;
            std::cout << "[DEBUG] Expert 행 파싱 완료" << std::endl;
        }
        else if (rowName == "Day") {
            if (tokens.size() < MAX_GAME_WAVE_COUNT + 1) {
                throw std::runtime_error("[ParseWaveTable] CSV 포맷 오류 : Day 열 개수 부족");
            }
            for (int wave = 0; wave < MAX_GAME_WAVE_COUNT; ++wave) {
                m_waveTable.m_day[wave] = toInt(tokens[1 + wave]);
                std::cout << "[DEBUG] Day[" << wave << "] = " << m_waveTable.m_day[wave] << std::endl;
            }
            linesParsed++;
            std::cout << "[DEBUG] Day 행 파싱 완료" << std::endl;
        }
        else {
            std::cerr << "[WARNING] 알 수 없는 행 발견: " << rowName << ". 해당 줄을 건너뜁니다." << std::endl;
        }
    }

    if (linesParsed < linesToParse) {
        throw std::runtime_error("[ParseWaveTable] 예상보다 데이터 행이 적습니다.");
    }

    std::cout << "[DEBUG] 모든 Wave 데이터 파싱 완료" << std::endl;
}


void DataTableManager::ParseExpeditionTable()
{
    std::cout << "[DEBUG] 데이터 파싱 진입 - ExpeditionReward" << std::endl;
    const std::string& csv = GetCSV("ExpeditionReward");
    std::istringstream stream(csv);
    std::string dataLine;

    constexpr int N = MAX_GAME_EXPEDITION_TYPE; // CSV는 Expedition1..N, 그리고 Expedition1_reward..N_reward
    bool gotFood = false, gotWood = false, gotMineral = false, gotPoint = false, gotSuccess = false;

    auto toInt = [](const std::string& s) {
        std::string t = JDUtil::trim(s);
        return t.empty() ? 0 : std::stoi(t);
        };
    auto toFloat = [](const std::string& s) {
        std::string t = JDUtil::trim(s);
        return t.empty() ? 0.f : std::stof(t);
        };

    while (std::getline(stream, dataLine))
    {
        std::vector<std::string> tokens;
        std::istringstream lineStream(dataLine);
        std::string cell;
        while (std::getline(lineStream, cell, ',')) tokens.push_back(JDUtil::trim(cell));

        if (tokens.empty() || tokens[0].empty()) continue;

        const std::string& row = tokens[0];
        const int have = static_cast<int>(tokens.size()) - 1;

        auto parseResourceRow = [&](auto setCost, auto setReward) {
            // 필요 최소: 앞 N(비용) + 뒤 N(보상) = 2N
            if (have < 2 * N) {
                throw std::runtime_error("[ParseExpeditionTable] CSV 포맷 오류 : " + row + " 열 개수 부족");
            }
            for (int i = 0; i < N; ++i) {
                setCost(i, toInt(tokens[1 + i]));
                setReward(i, toInt(tokens[1 + N + i]));
            }
            };

        if (row == "Food") {
            parseResourceRow(
                [&](int i, int v) { m_expeditionTable.m_cost[i].m_food = v; },
                [&](int i, int v) { m_expeditionTable.m_reward[i].m_food = v; }
            );
            gotFood = true;
        }
        else if (row == "Wood") {
            parseResourceRow(
                [&](int i, int v) { m_expeditionTable.m_cost[i].m_wood = v; },
                [&](int i, int v) { m_expeditionTable.m_reward[i].m_wood = v; }
            );
            gotWood = true;
        }
        else if (row == "Mineral") {
            parseResourceRow(
                [&](int i, int v) { m_expeditionTable.m_cost[i].m_mineral = v; },
                [&](int i, int v) { m_expeditionTable.m_reward[i].m_mineral = v; }
            );
            gotMineral = true;
        }
        else if (row == "Point") {
            if (have < N) {
                throw std::runtime_error("[ParseExpeditionTable] CSV 포맷 오류 : Point 열 개수 부족");
            }
            for (int i = 0; i < N; ++i) m_expeditionTable.m_point[i] = toInt(tokens[1 + i]);
            gotPoint = true;
        }
        else if (row == "SuccessRate") {
            if (have < N) {
                throw std::runtime_error("[ParseExpeditionTable] CSV 포맷 오류 : SuccessRate 열 개수 부족");
            }
            for (int i = 0; i < N; ++i) {
                // 퍼센트(10,15,25) 그대로 저장하려면 그대로, 확률(0.10f 등)로 쓰려면 /100.f
                m_expeditionTable.m_successRate[i] = toFloat(tokens[1 + i]); // 필요시 /100.f
            }
            gotSuccess = true;
        }
        // 그 외 라벨은 무시
    }

    if (!(gotFood && gotWood && gotMineral && gotPoint && gotSuccess))
        throw std::runtime_error("[ParseExpeditionTable] 필요한 행(Food/Wood/Mineral/Point/SuccessRate)을 모두 찾지 못했습니다.");

    std::cout << "[DEBUG] ExpeditionReward 파싱 완료" << std::endl;
}


void DataTableManager::PrintAllTable()
{
    cout << "=========================================" << endl;
    cout << "=========================================" << endl;
    cout << "파싱 테스트" << endl;
    cout << "=========================================" << endl;
    cout << "=========================================" << endl;
    m_startResourceTable.PrintResources();
    m_fishingSpotTable.PrintStats();
    m_lumberMillTable.PrintStats();
    m_mineTable.PrintStats();
    m_houseTable.PrintStats();
    cout << "=========================================" << endl;
    cout << "=========================================" << endl;
    m_soldierTypeTable.PrintStats();
    cout << "=========================================" << endl;
    cout << "=========================================" << endl;
    m_waveTable.PrintStats();
    cout << "=========================================" << endl;
    cout << "=========================================" << endl;
    m_expeditionTable.PrintStats();
    cout << "=========================================" << endl;
    cout << "=========================================" << endl;

}

