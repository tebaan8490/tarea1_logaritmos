#include "../include/rtree.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <cstdlib>

RTree::RTree(const std::string& filename) {
    this->filename = filename;
}

Nodo RTree::read_node_at(int idx, long long& ios) const {
    Nodo node;
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Error al abrir árbol: " << filename << std::endl;
        std::exit(1);
    }
    in.seekg(static_cast<std::streamoff>(idx) * BLOCK_SIZE);
    in.read(reinterpret_cast<char*>(&node), BLOCK_SIZE);
    if (!in) {
        std::cerr << "Error al leer nodo en posición " << idx << std::endl;
        std::exit(1);
    }
    ios++;
    return node;
}

// Retorna true si el MBR 'm' intersecta el rectángulo de consulta
bool intersects(const MBR& m, float xmin, float xmax,
                                     float ymin, float ymax) {
    return m.x1 <= xmax && m.x2 >= xmin &&
           m.y1 <= ymax && m.y2 >= ymin;
}

// Retorna true si el punto (MBR de área 0) está dentro del rectángulo
bool contains(const MBR& m, float xmin, float xmax,
                                   float ymin, float ymax) {
    return m.x1 >= xmin && m.x1 <= xmax &&
           m.y1 >= ymin && m.y1 <= ymax;
}

void RTree::search_rec(int idx,
                        float xmin, float xmax, float ymin, float ymax,
                        std::vector<MBR>& results, long long& ios) const {
    Nodo node = read_node_at(idx, ios);

    for (int i = 0; i < node.k; i++) {
        const MBR& child = node.hijos[i];

        if (!intersects(child, xmin, xmax, ymin, ymax))
            continue;

        if (child.valor == -1) {
            // Entrada hoja: es un punto; verificar contención exacta
            if (contains(child, xmin, xmax, ymin, ymax))
                results.push_back(child);
        } else {
            // Entrada interna: descender al nodo hijo
            search_rec(child.valor, xmin, xmax, ymin, ymax, results, ios);
        }
    }
}

std::vector<MBR> RTree::search(float xmin, float xmax,
                                 float ymin, float ymax,
                                 long long& ios) const {
    std::vector<MBR> results;
    search_rec(0, xmin, xmax, ymin, ymax, results, ios); // 0 = raíz
    return results;
}

namespace {

// Calcula el MBR mínimo que engloba todas las entradas en [from, to)
MBR calc_mbr(const std::vector<MBR>& items, int from, int to) {
    MBR result = items[from];
    result.valor = -1;
    for (int i = from + 1; i < to; i++) {
        result.x1 = std::min(result.x1, items[i].x1);
        result.x2 = std::max(result.x2, items[i].x2);
        result.y1 = std::min(result.y1, items[i].y1);
        result.y2 = std::max(result.y2, items[i].y2);
    }
    return result;
}

float center_x(const MBR& m) { return (m.x1 + m.x2) * 0.5f; }
float center_y(const MBR& m) { return (m.y1 + m.y2) * 0.5f; }

// Agrupa 'items' de a B, crea un Nodo por grupo en nodes[offset..],
// y devuelve los MBR del nivel superior (uno por nodo creado).
std::vector<MBR> pack_nodes(const std::vector<MBR>& items,
                              std::vector<Nodo>& nodes, int offset) {
    int n = static_cast<int>(items.size());
    int num_nodes = (n + B - 1) / B;
    nodes.resize(offset + num_nodes);

    std::vector<MBR> upper(num_nodes);
    for (int i = 0; i < num_nodes; i++) {
        int from = i * B;
        int to   = std::min(from + B, n);

        Nodo& node = nodes[offset + i];
        node.k = to - from;
        for (int j = 0; j < node.k; j++)
            node.hijos[j] = items[from + j];

        MBR mbr    = calc_mbr(items, from, to);
        mbr.valor  = offset + i;   // índice de este nodo en el vector global
        upper[i]   = mbr;
    }
    return upper;
}

// Ensambla la raíz con los MBR del nivel superior y la ubica en nodes[0]
void build_root(const std::vector<MBR>& upper, std::vector<Nodo>& nodes) {
    Nodo root{};
    root.k = static_cast<int>(upper.size());
    for (int i = 0; i < root.k; i++)
        root.hijos[i] = upper[i];
    nodes[0] = root;
}

}

namespace RTreeUtils {

void write_tree_to_file(const std::string& filename,
                         const std::vector<Nodo>& nodes) {
    std::ofstream out(filename, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "Error al abrir archivo para escritura: " << filename << std::endl;
        std::exit(1);
    }
    for (const Nodo& node : nodes)
        out.write(reinterpret_cast<const char*>(&node), BLOCK_SIZE);
}


static void nearest_x_rec(std::vector<MBR> items,
                            std::vector<Nodo>& nodes, int offset) {
    std::sort(items.begin(), items.end(),
              [](const MBR& a, const MBR& b) { return center_x(a) < center_x(b); });

    std::vector<MBR> upper = pack_nodes(items, nodes, offset);

    if (static_cast<int>(upper.size()) <= B) {
        build_root(upper, nodes);
        return;
    }
    nearest_x_rec(upper, nodes, offset + static_cast<int>(upper.size()));
}

void nearest_x(std::vector<MBR> points, std::vector<Nodo>& nodes) {
    nodes.clear();
    nodes.emplace_back();
    nearest_x_rec(points, nodes, 1);
}
}