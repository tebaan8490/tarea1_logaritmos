#pragma once
#include <fstream>
#include <string>
#include <vector>

// DISCLAIMER: Algunas de estas funciones son adaptadas del Template Tarea 1 
// compartido en el enunciado de la tarea, por lo que varias firmas serán
// similares


// Capacidad máxima de hijos por nodo (calculada para que sizeof(Nodo)==4096)
constexpr int B = 204;

// Tamaño de un bloque de disco en bytes (= tamaño de un nodo serializado)
constexpr int BLOCK_SIZE = 4096;


/**
 * @brief Estructura que representa un MBR en disco
 *
 * Cada MBR contiene:
 * - x1, x2, y1, y2: Puntos que representan un MBR
 * - valor: Puntero a la dirección donde están guardados los hijos de un nodo
 */
struct MBR {
    float x1, x2, y1, y2;
    int   valor;
};

/**
 * @brief Estructura que representa un Nodo del RTree en disco
 * 
 * Cada nodo contiene:
 * - k: Cantidad de hijos del nodo
 * - hijos: Arreglo de MRBs que son los hijos del nodo, cada uno con su puntero
 * - pad: Relleno para que el tamaño de un Nodo coincida con el tamaño de un bloque
 */
struct Nodo {
    int  k;
    MBR  hijos[B];
    char pad[12];
};

// Aseguramos que el tamaño de Nodo sea igual al de un bloque
static_assert(sizeof(Nodo) == BLOCK_SIZE,
              "sizeof(Nodo) debe ser exactamente 4096 bytes");



/**
 * @brief Clase que representa un RTree almacenado en disco
 *
 * Esta clase permite navegar un RTree guardado en un archivo binario.
 */              
class RTree {
public:
    /**
 * @brief Estructura que representa un MBR en disco
 *
 * Cada MBR contiene:
 * - x1, x2, y1, y2: Puntos que representan un MBR
 * - valor: Puntero a la dirección donde están guardados los hijos de un nodo
 */
    explicit RTree(const std::string& filename);

    /**
    * @brief Lee y deserializa un nodo del RTree desde una posición específica
    * del archivo binario
    *
    * Esta función calcula la posición exacta en bytes en el archivo binario a
    * partir del índice proporcionado, lee los bytes correspondientes a un nodo,
    * y los deserializa para reconstruir la estructura Nodo en memoria.
    * Es responsabilidad del usuario asegurarse de que el offset sea válido,
    * es decir, que exista un nodo en esa posición.
    *
    * @param idx Posición del nodo en el archivo
    * @param ios Contador de I/Os (lecturas y escrituras)
    * @return Nodo deserializado desde el archivo
    * @throws Termina el programa si no puede abrir el archivo o leer los bytes
    * del nodo
    */
    Nodo read_node_at(int idx, long long& ios) const;

    /**
    * @brief Busca los MBR en un rango determinado
    *
    * Esta función busca recursivamente MBR contenidos
    * en un rango determinado en el RTree
    *
    * @param xmin Coordenada x mínimo del rango de búsqueda
    * @param xmax Coordenada x máximo del rango de búsqueda
    * @param ymin Coordenada y mínimo del rango de búsqueda
    * @param ymax Coordenada y máximo del rango de búsqueda
    * @param ios Contador de I/Os (lecturas y escrituras)
    * @return Conjunto de MBR contenidos en el rango
    */
    std::vector<MBR> search(float xmin, float xmax,
                             float ymin, float ymax,
                             long long& ios) const;

private:
    std::string filename;

    
    /**
    * @brief Recursión para la función search
    *
    * Ve secuencialmente en los hijos de un Nodo si están contenidos
    * en el rango o no, para seguir con la recursión
    *
    * @param idx Indice del nodo leido
    * @param xmin Coordenada x mínimo del rango de búsqueda
    * @param xmax Coordenada x máximo del rango de búsqueda
    * @param ymin Coordenada y mínimo del rango de búsqueda
    * @param ymax Coordenada y máximo del rango de búsqueda
    * @param results El vector que será retornado con los resultados
    * @param ios Contador de I/Os (lecturas y escrituras)
    * @return Conjunto de MBR contenidos en el rango
    */                         
    void search_rec(int idx,
                    float xmin, float xmax, float ymin, float ymax,
                    std::vector<MBR>& results, long long& ios) const;
};

/**
 * @brief Otra función para calcular un MBR que contiene otros MBR
 *
 * Recibe un vector de MBR y calcula el MBR que los contiene
 *
 * @param items los MBR a contener
 * @return MBR que contiene los MBR de items
 */
MBR another_calc_mbr(const std::vector<MBR>& items);

namespace RTreeUtils {

/**
 * @brief Escribe un RTree en un archivo binario
 *
 * Esta función recibe un vector de nodos y los escribe secuencialmente
 * en un archivo binario. Cada nodo ocupa un bloque de espacio en
 * bytes.
 *
 * @param filename Nombre del archivo donde se escribirá el RTree
 * @param nodes Vector de nodos del RTree
 * @return Nodo deserializado desde el archivo
 * @throws Termina el programa si no puede abrir el archivo
 */
void write_tree_to_file(const std::string& filename,
                         const std::vector<Nodo>& nodes);


/**
 * @brief Comienzo de nearest_x
 * 
 * @param points Los puntos a transformar
 * @param nodes El vector de nodos en el que se guarda el resultado
 * @return Vector de nodos desde nodes
 */
void nearest_x(std::vector<MBR> points, std::vector<Nodo>& nodes);

/**
 * @brief Función que actua antes de la recursión de sort_tile_recursive
 * 
 * Simplemente elimina los elementos en nodes y reserva un espacio al
 * comienzo para la raiz. Luego procede a usar str_rec para comenzar
 * el algoritmo.
 * 
 * @param points Los puntos a transformar
 * @param nodes El vector de nodos en el que se guarda el resultado
 * @return Vector de nodos desde nodes
 */
void str(std::vector<MBR> points, std::vector<Nodo>& nodes);

/**
 * @brief Comienzo de sort_tile_recursive
 * 
 * @param points Los puntos a transformar
 * @param nodes El vector de nodos en el que se guarda el resultado
 * @param current_node Indicador de en que nodo queremos guardar en los resultados
 * @return Vector de nodos desde nodes
 */
void sort_tile_recursive(std::vector<MBR> pares, std::vector<Nodo>& results,
                          int current_node = 0);

}