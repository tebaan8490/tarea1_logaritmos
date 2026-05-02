#include "../include/rtree.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <vector>
#include <functional>

double measure_time(void (*f)()) {
    auto start = std::chrono::high_resolution_clock::now();
    f();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(end-start).count();
}

