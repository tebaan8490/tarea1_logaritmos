#pragma once
#include <cstdint>
#include <vector>
#include <fstream>

const int B =204;
const int BLOCK_SIZE = 4096;

struct MBR{
    float x1,x2,y1,y2; // límites del rectángulo
    int valor; // posición del hijo en el archivo, -1 si es hoja
};

struct Nodo{
    int k; // cantidad de hijos, varía entre 1 y B
    MBR hijos[B]; // arreglo de B pares clave-valor
    char pad[12]; // pad: 12 bytes libres para que la estructura coincida con un bloque
};

static_assert(sizeof(Nodo) == BLOCK_SIZE, "El tamaño de Nodo debe ser igual a BLOCK_SIZE");


void nearestX(std::vector<MBR> pares, std::vector<Nodo>& nodos);

void str(std::vector<MBR> pares, std::vector<Nodo>& nodos);

std::vector<MBR> buscar(const std::string& archivo, 
        float x1, float x2, 
        float y1, float y2,
        long long& ios);