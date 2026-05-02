#pragma once

#include <vector>

double measure_time(void (*f)());

struct Query {
    float x1, x2, y1, y2;
};

std::vector<Query> generate_queries(int count, float s);

std::pair<double, double> stadistics(const std::vector<int>& values);
