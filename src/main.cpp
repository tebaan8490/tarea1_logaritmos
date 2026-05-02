#include "../include/rtree.h"
#include "../include/io.h"
#include "../include/utils.h"
#include <iostream>
#include <iomanip>
#include <numeric>
#include <string>
#include <vector>
#include <fstream>


static void experimentoConstruccion(const std::string& datasetRandom,
                                     const std::string& datasetEuropa,
                                     const std::string& dirSalida) {
    std::cout << "\n=== Experimento 5.1: Construcción ===\n";
    std::cout << std::setw(8)  << "N"
              << std::setw(18) << "RandNearestX(ms)"
              << std::setw(14) << "RandSTR(ms)"
              << std::setw(18) << "EurNearestX(ms)"
              << std::setw(14) << "EurSTR(ms)" << "\n";
    
    std::ofstream csv(dirSalida + "/resultados_construccion.csv");
    csv << "metodo,dataset,N,tiempo_ms\n";
    for (int exp = 15; exp <= 24; exp++) {
    int N = 1 << exp;
    std::vector<Nodo> nodos;
    double tRandNX, tRandSTR, tEurNX, tEurSTR;

    auto puntos = readPoints(datasetRandom, N);
    tRandNX = measure_time([&]() { RTreeUtils::nearest_x(puntos, nodos); });
    RTreeUtils::write_tree_to_file(dirSalida + "/rand_nx_" + std::to_string(N) + ".bin", nodos);

    tRandSTR = measure_time([&]() { RTreeUtils::str(puntos, nodos); });
    RTreeUtils::write_tree_to_file(dirSalida + "/rand_str_" + std::to_string(N) + ".bin", nodos);

    puntos = readPoints(datasetEuropa, N);
    tEurNX = measure_time([&]() { RTreeUtils::nearest_x(puntos, nodos); });
    RTreeUtils::write_tree_to_file(dirSalida + "/eur_nx_" + std::to_string(N) + ".bin", nodos);

    tEurSTR = measure_time([&]() { RTreeUtils::str(puntos, nodos); });
    RTreeUtils::write_tree_to_file(dirSalida + "/eur_str_" + std::to_string(N) + ".bin", nodos);

    csv << "NearestX,Random," << N << "," << tRandNX  << "\n";
    csv << "STR,Random,"      << N << "," << tRandSTR << "\n";
    csv << "NearestX,Europa," << N << "," << tEurNX   << "\n";
    csv << "STR,Europa,"      << N << "," << tEurSTR  << "\n";

    std::cout << std::setw(8)  << N
              << std::setw(18) << std::fixed << std::setprecision(2) << tRandNX
              << std::setw(14) << tRandSTR
              << std::setw(18) << tEurNX
              << std::setw(14) << tEurSTR << "\n";
}
    csv.close();
}

static void experimentoConsultas(const std::string& dirSalida) {
    const int N = 1 << 24;
    const std::vector<float> valores_s = {0.0025f, 0.005f, 0.01f, 0.025f, 0.05f};

    struct Config { std::string nombre; std::string archivo; };
    std::vector<Config> arboles = {
        {"Rand NearestX", dirSalida + "/rand_nx_"  + std::to_string(N) + ".bin"},
        {"Rand STR",      dirSalida + "/rand_str_" + std::to_string(N) + ".bin"},
        {"Eur NearestX", dirSalida + "/eur_nx_"  + std::to_string(N) + ".bin"},
        {"Eur STR",      dirSalida + "/eur_str_" + std::to_string(N) + ".bin"},
    };

    std::cout << "\n=== Experimento 5.2: Consultas (N=2^24) ===\n";

    // CSV para graficar
    std::ofstream csv(dirSalida + "/resultados_consultas.csv");
    csv << "arbol,s,avg_ios,avg_puntos,std_puntos\n";

    for (const auto& arbol : arboles) {
        std::cout << "\n--- " << arbol.nombre << " ---\n";
        std::cout << std::setw(10) << "s"
                  << std::setw(14) << "avg I/Os"
                  << std::setw(16) << "avg puntos"
                  << std::setw(16) << "std puntos" << "\n";

        RTree tree(arbol.archivo);

        for (float s : valores_s) {
            auto queries = generate_queries(100, s);

            std::vector<int> ios_vec, puntos_vec;

            for (const auto& q : queries) {
                long long ios = 0;
                auto res = tree.search(q.x1, q.x2, q.y1, q.y2, ios);
                ios_vec.push_back(static_cast<int>(ios));
                puntos_vec.push_back(static_cast<int>(res.size()));
            }

            auto [avg_ios,   std_ios]    = stadistics(ios_vec);
            auto [avg_puntos, std_puntos] = stadistics(puntos_vec);

            std::cout << std::setw(10) << std::fixed << std::setprecision(4) << s
                      << std::setw(14) << std::setprecision(2) << avg_ios
                      << std::setw(16) << avg_puntos
                      << std::setw(16) << std_puntos << "\n";

            csv << arbol.nombre << "," << s << ","
                << avg_ios << "," << avg_puntos << "," << std_puntos << "\n";
        }
    }
    csv.close();
    std::cout << "\nCSV guardado en " << dirSalida << "/resultados_consultas.csv\n";
}


int main(int argc, char* argv[]) {
    std::string datasetRandom = "data/random.bin";
    std::string datasetEuropa    = "data/europa.bin";
    std::string dirSalida        = "data";

    if (argc >= 3) { datasetRandom = argv[1]; datasetEuropa = argv[2]; }
    if (argc >= 4) { dirSalida = argv[3]; }

    experimentoConstruccion(datasetRandom, datasetEuropa, dirSalida);
    experimentoConsultas(dirSalida);
    return 0;
}