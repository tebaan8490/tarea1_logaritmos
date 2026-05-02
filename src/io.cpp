#include "../include/io.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <cstdlib>

std::vector<MBR> readPoints(const std::string& archivo, int N) {
    std::ifstream in;
    in.open(archivo, std::ios::binary);

    if (!in) {
        std::cerr << "Error al abrir el archivo: " << archivo << std::endl;
        exit(1);
    }

    std::vector<MBR> puntos;

    float x, y;

    for (int i = 0; i < N; i++) {
        if (!in.read((char*)&x, sizeof(float)) ||
            !in.read((char*)&y, sizeof(float))) {
            break;
        }

        MBR m;
        m.x1 = x;
        m.y1 = y;
        m.x2 = x;
        m.y2 = y;
        m.valor = -1;

        puntos.push_back(m);
    }

    in.close();
    return puntos;
}

void writeTree(const std::vector<Nodo>& nodos, const std::string& archivo) {
    std::ofstream out;
    out.open(archivo, std::ios::binary);

    if (!out) {
        std::cout << "Error al escribir archivo: " << archivo << std::endl;
        exit(1);
    }

    for (int i = 0; i < (int)nodos.size(); i++) {
        out.write((char*)&nodos[i], BLOCK_SIZE);
    }

    out.close();
}

Nodo readNode(std::ifstream& file, int index, long long& ios) {
    Nodo nodo;

    file.seekg(index * BLOCK_SIZE);
    file.read((char*)&nodo, BLOCK_SIZE);

    if (!file) {
        std::cout << "Error leyendo nodo: " << index << std::endl;
        exit(1);
    }

    ios++;

    return nodo;
}