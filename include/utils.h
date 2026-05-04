#pragma once
#include <vector>
#include <chrono>
#include <utility>


template <typename Func>

/**
 * @brief Función que mide el tiempo que tarda una función en correr
 *
 * @param f La función a la cual se mide el tiempo
 */
double measure_time(Func f) {
    auto start = std::chrono::high_resolution_clock::now();
    f();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

/**
 * @brief Estructura que representa una consulta por rango
 *
 * Contiene:
 * - x1, x2, y1, y2: Puntos que representan el MBR de la consulta
 */
struct Query {
    float x1, x2, y1, y2;
};

// Semilla agregada para reproducibilidad
std::vector<Query> generate_queries(int count, float s, unsigned seed = 42);
std::pair<double, double> stadistics(const std::vector<int>& values);