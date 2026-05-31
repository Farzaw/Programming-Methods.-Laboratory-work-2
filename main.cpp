/**
 * @file main.cpp
 * @brief Бенчмарк алгоритмов поиска по ключу airline.
 *
 * Загружает CSV-файлы различных размеров, заполняет структуры данных,
 * замеряет среднее время поиска и сохраняет результаты.
 */

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "flight.h"
#include "search_structures.h"

namespace {

using Clock = std::chrono::high_resolution_clock;

/** @brief Ключ поиска для всех замеров (существует в сгенерированных данных). */
const std::string SEARCH_KEY = "Aeroflot";

/** @brief Число повторов поиска для усреднения времени. */
const int SEARCH_REPEATS = 10;

/**
 * @brief Читает рейсы из CSV-файла.
 *
 * Формат: flightNumber,airline,arrivalDate,arrivalTime,passengers
 *
 * @param path Путь к файлу.
 * @return Вектор загруженных рейсов.
 */
std::vector<Flight> readCSV(const std::string& path) {
    std::vector<Flight> result;
    std::ifstream fin(path);
    if (!fin.is_open()) {
        std::cerr << "Cannot open file: " << path << '\n';
        return result;
    }

    std::string line;
    std::getline(fin, line);
    while (std::getline(fin, line)) {
        if (line.empty()) continue;
        std::istringstream ss(line);
        Flight flight;
        std::string passengersStr;
        std::getline(ss, flight.flightNumber, ',');
        std::getline(ss, flight.airline, ',');
        std::getline(ss, flight.arrivalDate, ',');
        std::getline(ss, flight.arrivalTime, ',');
        std::getline(ss, passengersStr, ',');
        flight.passengers = std::stoi(passengersStr);
        result.push_back(std::move(flight));
    }
    return result;
}

/**
 * @brief Усредняет время выполнения функции поиска.
 *
 * @tparam SearchFn Тип callable: std::vector<Flight>(const std::string&).
 * @param searchFn Функция поиска.
 * @param key Ключ поиска.
 * @param repeats Число повторов.
 * @return Среднее время в микросекундах.
 */
template <typename SearchFn>
double averageSearchMicroseconds(SearchFn searchFn,
                                 const std::string& key,
                                 int repeats) {
    std::vector<Flight> warmup = searchFn(key);
    (void)warmup;

    long long totalMicroseconds = 0;
    for (int i = 0; i < repeats; ++i) {
        const auto start = Clock::now();
        volatile std::size_t sink = searchFn(key).size();
        (void)sink;
        const auto end = Clock::now();
        totalMicroseconds += std::chrono::duration_cast<std::chrono::microseconds>(
                                 end - start)
                                 .count();
    }
    return static_cast<double>(totalMicroseconds) / repeats;
}

/**
 * @brief Форматирует время: микросекунды для малых значений, иначе миллисекунды.
 * @param microseconds Среднее время в мкс.
 * @return Строка для вывода в таблицу.
 */
std::string formatTime(double microseconds) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3);
    if (microseconds >= 1000.0) {
        oss << (microseconds / 1000.0) << " ms";
    } else {
        oss << microseconds << " us";
    }
    return oss.str();
}

}  // namespace

/**
 * @brief Точка входа: бенчмарк поиска по airline.
 * @return 0 при успешном завершении.
 */
int main() {
    const std::vector<std::pair<int, std::string>> inputs = {
        {100, "data/flights_100.csv"},
        {500, "data/flights_500.csv"},
        {1000, "data/flights_1000.csv"},
        {2000, "data/flights_2000.csv"},
        {5000, "data/flights_5000.csv"},
        {10000, "data/flights_10000.csv"},
        {20000, "data/flights_20000.csv"},
        {50000, "data/flights_50000.csv"},
        {75000, "data/flights_75000.csv"},
        {100000, "data/flights_100000.csv"},
        {200000, "data/flights_200000.csv"},
        {500000, "data/flights_500000.csv"},
        {1000000, "data/flights_1000000.csv"},
    };

    std::filesystem::create_directories("output");

    std::ofstream csvOut("output/search_results.csv");
    if (!csvOut.is_open()) {
        std::cerr << "Cannot create output/search_results.csv\n";
        return 1;
    }

    csvOut << "size,linear_us,bst_us,rb_tree_us,hash_us,multimap_us,"
              "hash_collisions,found_count\n";
    csvOut << std::fixed << std::setprecision(3);

    std::cout << "\n=== Search benchmark (key: \"" << SEARCH_KEY << "\", "
              << SEARCH_REPEATS << " runs avg) ===\n\n";

    std::cout << std::left
              << std::setw(10) << "Size"
              << std::setw(14) << "Linear"
              << std::setw(14) << "BST"
              << std::setw(14) << "RB-Tree"
              << std::setw(14) << "Hash"
              << std::setw(14) << "multimap"
              << std::setw(12) << "Collisions"
              << std::setw(8) << "Found"
              << '\n';
    std::cout << std::string(100, '-') << '\n';

    for (const auto& [size, path] : inputs) {
        const std::vector<Flight> data = readCSV(path);
        if (data.empty()) {
            std::cerr << "Skipping missing or empty file: " << path << '\n';
            continue;
        }

        BinarySearchTree bst;
        RedBlackTree rbTree;
        HashTable hashTable(recommendedBucketCount(data.size()));
        std::multimap<std::string, Flight> multiMap;

        for (const Flight& flight : data) {
            bst.insert(flight);
            rbTree.insert(flight);
            hashTable.insert(flight);
            multiMap.emplace(flight.airline, flight);
        }

        const std::size_t collisions = hashTable.collisionCount();
        const std::size_t foundCount = linearSearch(data, SEARCH_KEY).size();

        const double linearUs = averageSearchMicroseconds(
            [&](const std::string& key) { return linearSearch(data, key); },
            SEARCH_KEY, SEARCH_REPEATS);

        const double bstUs = averageSearchMicroseconds(
            [&](const std::string& key) { return bst.search(key); },
            SEARCH_KEY, SEARCH_REPEATS);

        const double rbUs = averageSearchMicroseconds(
            [&](const std::string& key) { return rbTree.search(key); },
            SEARCH_KEY, SEARCH_REPEATS);

        const double hashUs = averageSearchMicroseconds(
            [&](const std::string& key) { return hashTable.search(key); },
            SEARCH_KEY, SEARCH_REPEATS);

        const double multimapUs = averageSearchMicroseconds(
            [&](const std::string& key) {
                std::vector<Flight> found;
                const auto range = multiMap.equal_range(key);
                found.reserve(static_cast<std::size_t>(
                    std::distance(range.first, range.second)));
                for (auto it = range.first; it != range.second; ++it) {
                    found.push_back(it->second);
                }
                return found;
            },
            SEARCH_KEY, SEARCH_REPEATS);

        std::cout << std::setw(10) << size
                  << std::setw(14) << formatTime(linearUs)
                  << std::setw(14) << formatTime(bstUs)
                  << std::setw(14) << formatTime(rbUs)
                  << std::setw(14) << formatTime(hashUs)
                  << std::setw(14) << formatTime(multimapUs)
                  << std::setw(12) << collisions
                  << std::setw(8) << foundCount
                  << '\n';

        csvOut << size << ','
               << linearUs << ','
               << bstUs << ','
               << rbUs << ','
               << hashUs << ','
               << multimapUs << ','
               << collisions << ','
               << foundCount << '\n';
    }

    std::cout << "\nResults saved to output/search_results.csv\n";
    return 0;
}
