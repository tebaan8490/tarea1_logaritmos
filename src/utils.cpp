#include "../include/rtree.h"
#include "../include/utils.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <vector>
#include <random>
#include <cmath>

template <typename Func>

double measure_time(Func f) {
    auto start = std::chrono::high_resolution_clock::now();
    f();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(end-start).count();
}

std::vector<Query> generate_queries(int count, float s) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0, 1.0-s);
    std::vector<Query> queries;

    for (int i = 0; i < count; ++i) {
        float x = dist(gen);
        float y = dist(gen);

        Query q;
        q.x1 = x;
        q.y1 = y;
        q.x2 = x + s;
        q.y2 = y + s;

        queries.push_back(q);
    }

    return queries;
}

std::pair<double, double> stadistics(const std::vector<int>& values) {
    double promedio = 0;

    for (int i = 0; i < (int)values.size(); ++i) {
        promedio += values[i];
    }
    promedio /= values.size();

    double varianza = 0;
    for (int i = 0; i < (int)values.size(); ++i) {
        varianza += (values[i]-promedio) * (values[i]-promedio);
    }
    varianza /= values.size();

    varianza = std::sqrt(varianza);
    return std::make_pair(promedio, varianza);
}