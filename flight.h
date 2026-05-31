#pragma once

#include <iostream>
#include <string>

/**
 * @file flight.h
 * @brief Структура данных авиарейса и операторы сравнения.
 * @author Andrey
 * @date 2026
 */

/**
 * @struct Flight
 * @brief Представляет авиарейс, прибывающий в аэропорт.
 *
 * Содержит информацию о рейсе: номер, авиакомпания, дата и время прилёта,
 * количество пассажиров на борту.
 *
 * Правила сравнения (по приоритету):
 * 1. Дата прилёта (возрастание)
 * 2. Время прилёта (возрастание)
 * 3. Название авиакомпании (лексикографически)
 * 4. Число пассажиров (убывание)
 */
struct Flight {
    std::string flightNumber; ///< Номер рейса (например, "SU1234")
    std::string airline;      ///< Название авиакомпании — ключ для поиска
    std::string arrivalDate;  ///< Дата прилёта в формате "YYYY-MM-DD"
    std::string arrivalTime;  ///< Время прилёта по расписанию в формате "HH:MM"
    int passengers;           ///< Число пассажиров на борту

    /**
     * @brief Оператор «меньше» для сравнения двух рейсов.
     *
     * Сравнение выполняется по приоритету:
     * дата → время → авиакомпания → число пассажиров (убывание).
     *
     * @param o Правый операнд сравнения.
     * @return true, если текущий объект «меньше» o.
     */
    bool operator<(const Flight& o) const {
        if (arrivalDate != o.arrivalDate) return arrivalDate < o.arrivalDate;
        if (arrivalTime != o.arrivalTime) return arrivalTime < o.arrivalTime;
        if (airline != o.airline) return airline < o.airline;
        return passengers > o.passengers;
    }

    /**
     * @brief Оператор «больше».
     * @param o Правый операнд.
     * @return true, если текущий объект «больше» o.
     */
    bool operator>(const Flight& o) const { return o < *this; }

    /**
     * @brief Оператор «меньше или равно».
     * @param o Правый операнд.
     * @return true, если текущий объект «меньше или равен» o.
     */
    bool operator<=(const Flight& o) const { return !(o < *this); }

    /**
     * @brief Оператор «больше или равно».
     * @param o Правый операнд.
     * @return true, если текущий объект «больше или равен» o.
     */
    bool operator>=(const Flight& o) const { return !(*this < o); }
};

/**
 * @brief Лексикографическое сравнение строк-ключей для деревьев поиска.
 *
 * Использует стандартный оператор @c std::string::operator<.
 *
 * @param a Первая строка-ключ.
 * @param b Вторая строка-ключ.
 * @return Отрицательное число, 0 или положительное (как @c strcmp).
 */
inline int compareKeys(const std::string& a, const std::string& b) {
    if (a < b) return -1;
    if (a > b) return 1;
    return 0;
}
