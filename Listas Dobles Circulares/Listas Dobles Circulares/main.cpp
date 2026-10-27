#include <iostream>
#include "ListaDobleCircular.h"

using namespace std;

int main() {
    ListaDobleCircular<int> lista;

    // Pruebas de insercion
    lista.insertarAlFinal(20);
    lista.insertarAlFinal(30);
    lista.insertarAlFinal(40);
    lista.insertarAlInicio(10);

    cout << "ESTADO INICIAL" << endl;
    lista.imprimir();
    cout << "Tamanio: " << lista.getTamanio() << endl;

// Pruebas de busqueda
    cout << "\nOBTENER ELEMENTOS POR POSICION" << endl;

try
{
    cout << "Posicion 0: "
         << lista.obtenerEnPosicion(0) << endl;

    cout << "Posicion 1: "
         << lista.obtenerEnPosicion(1) << endl;

    cout << "Posicion 2: "
         << lista.obtenerEnPosicion(2) << endl;

    cout << "Posicion 3: "
         << lista.obtenerEnPosicion(3) << endl;
}
catch (const out_of_range& e)
{
    cout << "Error: " << e.what() << endl;
}
// Prueba de busqueda

    cout << "\nBUSCANDO EL 30" << endl;
    Nodo<int>* encontrado = lista.buscar(30);
    if (encontrado != nullptr)
        cout << "Elemento " << encontrado->getDato() << " encontrado." << endl;
    else
        cout << "Elemento no encontrado." << endl;

    cout << "\nELIMINANDO UN VALOR EN MEDIO (20)" << endl;
    lista.eliminar(20);
    lista.imprimir();

    cout << "\nELIMINANDO LA CABEZA (10)" << endl;
    lista.eliminar(10);
    lista.imprimir();

    cout << "\nELIMINANDO UN VALOR QUE NO EXISTE (99)" << endl;
    lista.eliminar(99);

    cout << "\nPRUEBA CON OTRO TIPO (string)" << endl;
    ListaDobleCircular<string> nombres;
    nombres.insertarAlFinal("Ana");
    nombres.insertarAlInicio("Luis");
    nombres.imprimir();

    return 0;
}
