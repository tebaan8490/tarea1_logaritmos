#include "../include/utils.h"
#include <random>
#include <cmath>
#include <numeric>

/**
 * @brief Función que genera un vector de consultas
 * 
 * Estas consultas están generadas basándose en la semilla puesta
 * en utils.h
 *
 * @param count El número de consultas a generar
 * @param s Para la distribución de las consultas
 * @param seed La semilla que determina las consultas generadas
 */
std::vector<Query> generate_queries(int count, float s, unsigned seed) {
    std::mt19937 gen(seed);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f - s);
    std::vector<Query> queries;
    queries.reserve(count);
    for (int i = 0; i < count; ++i) {
        float x = dist(gen);
        float y = dist(gen);
        queries.push_back({x, x + s, y, y + s});
    }
    return queries;
}

/**
 * @brief Genera un par con el promedio y la varianza de enteros
 *
 * @param values Enteros a los que se les calcula promedio y varianza
 */
std::pair<double, double> stadistics(const std::vector<int>& values) {
    double promedio = 0;
    for (int v : values) promedio += v;
    promedio /= values.size();

    double varianza = 0;
    for (int v : values) varianza += (v - promedio) * (v - promedio);
    varianza = std::sqrt(varianza / values.size());

    return {promedio, varianza};
}