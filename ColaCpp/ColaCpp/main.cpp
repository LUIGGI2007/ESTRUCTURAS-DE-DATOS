#include <iostream>
#include "Operaciones.h"
using namespace std;

int main() {
    Nodo* frente = nullptr;
    Nodo* fin = nullptr;

    // Datos fijos para observar el comportamiento FIFO.
    cout << "Insertar 10, 20 y 30:\n";
    insertar(frente, fin, 10);
    insertar(frente, fin, 20);
    insertar(frente, fin, 30);
    imprimir(frente);

    cout << "Buscar 20: ";
    if (buscar(frente, 20)) {
        cout << "encontrado\n";
    } else {
        cout << "no encontrado\n";
    }

    // Contar elementos
    cout << "Contar elemento 20: "
     << contarElemento(frente, 20) << '\n';

    cout << "Contar elemento 99: "
     << contarElemento(frente, 99) << '\n';



    cout << "Eliminar el primero (10):\n";
    eliminar(frente, fin);
    imprimir(frente);

    cout << "Buscar 10: ";
    if (buscar(frente, 10)) {
        cout << "encontrado\n";
    } else {
        cout << "no encontrado\n";
    }

    // Liberar todos los nodos antes de finalizar.
    while (frente != nullptr) {
        eliminar(frente, fin);
    }
    imprimir(frente);
    return 0;
}
