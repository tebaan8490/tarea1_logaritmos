#pragma once
#include <fstream>
#include <string>
#include <vector>

/// Capacidad máxima de hijos por nodo (calculada para que sizeof(Nodo)==4096)
constexpr int B = 204;

/// Tamaño de un bloque de disco en bytes (= tamaño de un nodo serializado)
constexpr int BLOCK_SIZE = 4096;

struct MBR {
    float x1, x2, y1, y2;
    int   valor;
};

struct Nodo {
    int  k;
    MBR  hijos[B];
    char pad[12];
};

static_assert(sizeof(Nodo) == BLOCK_SIZE,
              "sizeof(Nodo) debe ser exactamente 4096 bytes");

class RTree {
public:
    explicit RTree(const std::string& filename);

    Nodo read_node_at(int idx, long long& ios) const;

    std::vector<MBR> search(float xmin, float xmax,
                             float ymin, float ymax,
                             long long& ios) const;

private:
    std::string filename;

    void search_rec(int idx,
                    float xmin, float xmax, float ymin, float ymax,
                    std::vector<MBR>& results, long long& ios) const;
};

MBR another_calc_mbr(const std::vector<MBR>& items);

namespace RTreeUtils {

void write_tree_to_file(const std::string& filename,
                         const std::vector<Nodo>& nodes);

void nearest_x(std::vector<MBR> points, std::vector<Nodo>& nodes);

void str(std::vector<MBR> points, std::vector<Nodo>& nodes);

void sort_tile_recursive(std::vector<MBR> pares, std::vector<Nodo>& results,
                          int current_node = 0);

}