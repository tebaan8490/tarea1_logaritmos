#pragma once
#include <vector>
#include <chrono>
#include <utility>


template <typename Func>
double measure_time(Func f) {
    auto start = std::chrono::high_resolution_clock::now();
    f();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

struct Query {
    float x1, x2, y1, y2;
};

// semilla agregada para reproducibilidad
std::vector<Query> generate_queries(int count, float s, unsigned seed = 42);
std::pair<double, double> stadistics(const std::vector<int>& values);