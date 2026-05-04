#pragma once
#include "rtree.h"
#include <vector>
#include <string>

/**
 * @brief Lee N puntos de un RTree guardado en un archivo binario
 *
 * Recibe un vector de MBR y calcula el MBR que los contiene
 *
 * @param archivo Nombre del archivo binario
 * @param N Cantidad de puntos a leer
 * @return Vector de puntos leidos
 * @throws Termina el programa si no se puede leer el archivo
 */
std::vector<MBR> readPoints(const std::string& archivo, int N);

/**
 * @brief Escribe un RTree en un archivo binario
 *
 * Esta función recibe un vector de nodos y los escribe secuencialmente
 * en un archivo binario. Cada nodo ocupa un bloque de espacio en
 * bytes.
 *
 * @param nodes Vector de nodos del RTree
 * @param archivo Nombre del archivo donde se escribirá el RTree
 * @return RTree escrito en el archivo
 * @throws Termina el programa si no puede abrir el archivo
 */
void writeTree(const std::vector<Nodo>& nodos, const std::string& archivo);

/**
 * @brief Lee un nodo específico de un RTree guardado en un archivo binario
 *
 * @param file Nombre del archivo binario con un RTree
 * @param index Número del nodo a leer
 * @param ios Contador de I/Os (lecturas y escrituras)
 * @return Nodo leido
 * @throws Termina el programa si no puede abrir el archivo
 */
Nodo readNode(std::ifstream& file, int index, long long& ios);

