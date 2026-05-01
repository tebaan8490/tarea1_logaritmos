#pragma once
#include "rtree.h"
#include <vector>
#include <string>

std::vector<MBR> readPoints(const std::string& archivo, int N);

void writeTree(const std::vector<Nodo>& nodos, const std::string& archivo);

Nodo readNode(std::ifstream& file, int index, long long& ios);

