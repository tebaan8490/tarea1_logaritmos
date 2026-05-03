#include "../include/rtree.h"
#include <bits/stdc++.h>
using namespace std;


void sort_tile_recursive(std::vector<MBR> pares, vector<Nodo>& results, int current_node = 0) {

    // Ordenamos los pares de MBR por coordenada x
    std::sort(pares.begin(), pares.end(), [](MBR &a, MBR &b) {
    float a_center = (a.x2 + a.x1)/0.5f;
    float b_center = (b.x2 + b.x1)/0.5f;
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


    //Dividir los pares en S rectangulos/grupos verticales
    for(int k=0; k<S; k++) { // Por cada grupo
        if (k == (S-1)) {    // Si k es el grupo final, puede que hayan menos elementos que n_division_x
            std::vector<MBR> sub(pares.begin() + (k*n_division_x), 
            pares.begin() + n);
            groups[k] = sub; // Se guarda el grupo en el vector de grupos (recordar que es vector de vectores de MBR)

        } else {
            std::vector<MBR> sub(pares.begin() + (k*n_division_x), 
            pares.begin() + ((k+1)*n_division_x));
            groups[k] = sub;
        }


        // Copiamos el grupo recien creado y lo ordenamos por coordenada y de los centros
        std::vector<MBR> group = groups[k];
        std::sort(group.begin(), group.end(), [](MBR &a, MBR &b) {
        float a_center = (a.y2 + a.y1)/0.5f;
        float b_center = (b.y2 + b.y1)/0.5f;
        return a_center < b_center;
        });
        

        // Ahora en el mismo ciclo for. Dividimos el rectangulo/grupo vertical en subgrupos horizontales
        int n_rectangles = std::ceil((static_cast<float>(group.size())/B)); // Numero de subgrupos por cada grupo. Techo

        // Agarramos los grupos de puntos (ya ordenados por coordenada y) y los guardamos en temporary_MBRs
        // Tambien, transformamos estos subgrupos en nodos
        for (int m=0; m<n_rectangles; m++) {
            if (currrent_MBR == (s-1)) {    // Otra vez, si l MBR actual es el ultimo de los subgrupos. Puede que no haya B elementos
                std::vector<MBR> sub(group.begin() + (m*B), 
                group.begin() + (group.size()));
                temporary_MBRs[currrent_MBR] = sub;
                
                Nodo new_node;
                new_node.k = sub.size();
                for (int k=0; k<sub.size(); k++) {
                    new_node.hijos[k] = sub[k];
                }
                results.push_back(new_node);

            } else {
                std::vector<MBR> sub(group.begin() + (m*B), 
                group.begin() + ((m+1)*B));
                temporary_MBRs[currrent_MBR] = sub;

                Nodo new_node;
                new_node.k = sub.size();
                for (int k=0; k<sub.size(); k++) {
                    new_node.hijos[k] = sub[k];
                }
                results.push_back(new_node);
            }
            currrent_MBR++;
        }    
    }


    // Calculamos los MBR de los vectores de MBR temporales y lo guardamos en final_MBRs
    std::vector<MBR> final_MBRs(s);
    for(int i=0; i<s; i++) {
        MBR result = another_calc_mbr(temporary_MBRs[i]);
        result.valor = (i+1) + current_node;   // Para llevar la cuenta del indicador de donde estan los hijos. Usamos el current_node como base
        final_MBRs[i] = result;
    }

    
    if (s > B) {   // Vemos si s (igual a n/B) > B
        current_node += final_MBRs.size();
        sort_tile_recursive(final_MBRs, results, current_node);
    } else {       // Si no, creamos el nodo raiz y lo ponemos al principio del vector del arbol
        Nodo root;
        root.k = final_MBRs.size();
        for (int i=0; i<final_MBRs.size(); i++) {
            root.hijos[i] = final_MBRs[i];
        }
        results.insert(results.begin(), root);
    }
}