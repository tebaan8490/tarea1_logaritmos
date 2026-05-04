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

/**
 * @brief Determina si un MBR intersecta un cierto rango
 * 
 *
 * @param xmin Coordenada x mínimo del rango de búsqueda
 * @param xmax Coordenada x máximo del rango de búsqueda
 * @param ymin Coordenada y mínimo del rango de búsqueda
 * @param ymax Coordenada y máximo del rango de búsqueda
 * @return booleano que indica si intersecta o no
 */
static bool intersects(const MBR& m, float xmin, float xmax,
                        float ymin, float ymax) {
    return m.x1 <= xmax && m.x2 >= xmin &&
           m.y1 <= ymax && m.y2 >= ymin;
}

/**
 * @brief Determina si un MBR está contenido en un rango de búsqueda
 * 
 *
 * @param xmin Coordenada x mínimo del rango de búsqueda
 * @param xmax Coordenada x máximo del rango de búsqueda
 * @param ymin Coordenada y mínimo del rango de búsqueda
 * @param ymax Coordenada y máximo del rango de búsqueda
 * @return booleano que indica si está contenido o no
 */
static bool contains(const MBR& m, float xmin, float xmax,
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
            if (contains(child, xmin, xmax, ymin, ymax))
                results.push_back(child);
        } else {
            search_rec(child.valor, xmin, xmax, ymin, ymax, results, ios);
        }
    }
}

std::vector<MBR> RTree::search(float xmin, float xmax,
                                 float ymin, float ymax,
                                 long long& ios) const {
    std::vector<MBR> results;
    search_rec(0, xmin, xmax, ymin, ymax, results, ios);
    return results;
}

/**
 * @brief Función para calcular un MBR que contiene otros MBR
 *
 * Recibe un vector de MBR y calcula el MBR que los contiene
 *
 * @param items Los MBR
 * @param from Entero que determina desde que MBR se empieza a calcular el nuevo MBR
 * @param to Entero que determina hasta que MBR se calcula
 * @return MBR que contiene los MBR indicados de items
 */
static MBR calc_mbr(const std::vector<MBR>& items, int from, int to) {
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

MBR another_calc_mbr(const std::vector<MBR>& items) {
    MBR result;
    result.valor = -1;
    result.x1 =  INFINITY;
    result.x2 = -INFINITY;
    result.y1 =  INFINITY;
    result.y2 = -INFINITY;
    for (int i = 0; i < (int)items.size(); i++) {
        result.x1 = std::min(result.x1, items[i].x1);
        result.x2 = std::max(result.x2, items[i].x2);
        result.y1 = std::min(result.y1, items[i].y1);
        result.y2 = std::max(result.y2, items[i].y2);
    }
    return result;
}

/**
 * @brief Funciones que calculan el centro de un MBR. En coordenada x
 *
 * @param m MBR al que se le calcula su centro
 * @return Su centro en coordenada x
 */
static float center_x(const MBR& m) { return (m.x1 + m.x2) * 0.5f; }

/**
 * @brief Funciones que calculan el centro de un MBR. En coordenada y
 *
 * @param m MBR al que se le calcula su centro
 * @return Su centro en coordenada y
 */
static float center_y(const MBR& m) { return (m.y1 + m.y2) * 0.5f; }

/**
 * @brief Empaquete MBRs como nodos, y calcula su MBR que los contiene
 *
 * @param items MBRs
 * @param nodes Vector de nodos donde se guardan los MBR de items
 * @param offset Offset para saber donde guardar los nodos y MBR
 * @return Vector de MBR 
 */
static std::vector<MBR> pack_nodes(const std::vector<MBR>& items,
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

        MBR mbr   = calc_mbr(items, from, to);
        mbr.valor = offset + i;
        upper[i]  = mbr;
    }
    return upper;
}

/**
 * @brief Construye la raiz del RTree
 *
 * @param upper Vector de MBR con los hijos
 * @param nodes Vector de nodos donde se guardará la raiz
 * @return Raiz en nodes 
 */
static void build_root(const std::vector<MBR>& upper, std::vector<Nodo>& nodes) {
    Nodo root{};
    root.k = static_cast<int>(upper.size());
    for (int i = 0; i < root.k; i++)
        root.hijos[i] = upper[i];
    nodes[0] = root;
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

/**
 * @brief Crea un RTree representado como un vector de nodos
 * 
 * Toma los puntos ordenados por coordenada x, los divide en n/B grupos
 * para luego guardarlos como nodos, y calcular sus MBR. Si estos n/B
 * pares llave valor caben en un nodo raiz, se construye dicha raiz
 * en nodes. Si no, se hace una recursión de nearest_x con esos
 * n/B pares
 * 
 * @param points Los puntos a transformar
 * @param nodes El vector de nodos en el que se guarda el resultado
 * @param offset Entero que indica donde guardar MBRs o nodos
 * @return Vector de nodos desde nodes
 */
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

/**
 * @brief Crea un RTree representado como un vector de nodos
 * 
 * Toma los puntos ordenados por coordenada x, los divide en raiz de n/B
 * grupos. Luego, por cada grupo se ordenan los puntos por coordenada y,
 * y se vuelve a dividir en raiz de n/B sub-grupos, quedando con n/B
 * grupos en total para luego guardarlos como nodos, y calcular sus MBR.
 * Si estos n/B pares llave valor caben en un nodo raiz, se construye 
 * dicha raiz en nodes. Si no, se hace una recursión de nearest_x con
 * esos n/B pares.
 * 
 * @param points Los puntos a transformar
 * @param nodes El vector de nodos en el que se guarda el resultado
 * @param current_node Indicador de en que nodo queremos guardar en los resultados
 * @return Vector de nodos desde nodes
 */
static void str_rec(std::vector<MBR> pares, std::vector<Nodo>& results, int current_node) {

    // Ordenamos los pares de MBR por coordenada x
    std::sort(pares.begin(), pares.end(), [](const MBR& a, const MBR& b) {
        float a_center = (a.x2 + a.x1) * 0.5f;
        float b_center = (b.x2 + b.x1) * 0.5f;
        return a_center < b_center;
    });
    int n = pares.size();


    // Calculos de cuantos grupos se haran
    int s = std::ceil(static_cast<float>(n)/B); // Numero de rectangulos/grupos totales de la recursion. Techo para que no queden puntos/MRBs sin encapsular
    int S = std::round(std::sqrt(s)); // Numero de rectangulos/grupos verticales (a lo alto). Redondeado
    std::vector<std::vector<MBR>> groups(S); // Vector para almazenar los rectangulos/grupos verticales
    std::vector<std::vector<MBR>> temporary_MBRs(s); // MBRs temporales, para calcular sus MBRs al final de la funcion
    int currrent_MBR = 0; // Contador de en que MBR temporal estamos

    // Se calcula numero de elementos por rectangulo/grupo vertical
    // Demore como 2 horas enteras encontrando esta formula. it just works
    // Idea: El numero de elem lo divides por cantidad de rectangulos para ver cuantos elementos van en cada uno
    // Luego, lo divides por B para ver cuantos elementos van por bloque en el rectangulo
    // Sacas techo para que no falten elementos sin revisar
    // Multiplicas otra vez por B para ver la cantidad final. Estos dos ultimos pasos son para "normalizar"

    // n_division_x = ceil(n/SB)*B
    float float_div = static_cast<float>(n)/(S*B);
    int n_division_x = std::ceil(float_div)*B; // Numero de elementos por rectangulo


    // Reservar espacio para los s nodos de este nivel antes de escribirlos,
    // para que los índices sean estables dentro del vector global
    results.resize(current_node + s);

    int node_idx = current_node; // cursor sobre results[], análogo a current_node original

    //Dividir los pares en S rectangulos/grupos verticales
    for(int k=0; k<S && node_idx < current_node + s; k++) { // Por cada grupo
        int from_k = k * n_division_x;
        int to_k   = (k == S-1) ? n : std::min(from_k + n_division_x, n);

        std::vector<MBR> group(pares.begin() + from_k, pares.begin() + to_k);
        groups[k] = group; // Se guarda el grupo en el vector de grupos (recordar que es vector de vectores de MBR)

        // Copiamos el grupo recien creado y lo ordenamos por coordenada y de los centros
        std::sort(group.begin(), group.end(), [](const MBR& a, const MBR& b) {
            return center_y(a) < center_y(b);
        });

        // Ahora en el mismo ciclo for. Dividimos el rectangulo/grupo vertical en subgrupos horizontales
        int n_rectangles = std::ceil((static_cast<float>(group.size())/B)); // Numero de subgrupos por cada grupo. Techo

        // Agarramos los grupos de puntos (ya ordenados por coordenada y) y los guardamos en temporary_MBRs
        // Tambien, transformamos estos subgrupos en nodos
        for (int m=0; m<n_rectangles && node_idx < current_node + s; m++, node_idx++) {
            int from_m = m * B;
            int to_m   = (currrent_MBR == s-1)
                         ? static_cast<int>(group.size())
                         : std::min(from_m + B, static_cast<int>(group.size()));

            std::vector<MBR> sub(group.begin() + from_m, group.begin() + to_m);
            temporary_MBRs[currrent_MBR] = sub;

            // Transformamos el subgrupo en nodo y lo escribimos en su posición real
            Nodo& new_node = results[node_idx];
            new_node.k = sub.size();
            for (int j = 0; j < (int)sub.size(); j++) {
                new_node.hijos[j] = sub[j];
            }
            currrent_MBR++;
        }
    }


    // Calculamos los MBR de los vectores de MBR temporales y lo guardamos en final_MBRs
    std::vector<MBR> final_MBRs(s);
    for(int i=0; i<s; i++) {
        MBR result = another_calc_mbr(temporary_MBRs[i]);
        // Para llevar la cuenta del indicador de donde estan los hijos. Usamos el current_node como base
        result.valor = current_node + i;
        final_MBRs[i] = result;
    }


    if (s > B) {   // Vemos si s (igual a n/B) > B
        current_node += final_MBRs.size();
        str_rec(final_MBRs, results, current_node);
    } else {       // Si no, creamos el nodo raiz y lo ponemos al principio del vector del arbol
        build_root(final_MBRs, results);
    }
}

void str(std::vector<MBR> points, std::vector<Nodo>& nodes) {
    nodes.clear();
    nodes.emplace_back(); // nodes[0] reservado para la raíz
    str_rec(points, nodes, 1);
}

void sort_tile_recursive(std::vector<MBR> pares, std::vector<Nodo>& results,
                          int /*current_node*/) {
    str(std::move(pares), results);
}

} // namespace RTreeUtils