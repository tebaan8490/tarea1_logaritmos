struct MBR {
    float x1, x2, y1, y2;   //límites del rectángulo
    int valor;  //posición del hijo en el archivo, -1 si es hoja
};

struct Nodo {
    int k; // cantidad de hijos, varía entre 1 y b
    MBR mbr[204]; //arreglo de b pares clave-valor
    char pad[12]; // pad: 12 bytes libres para que la estructura coincida con un bloque
};
