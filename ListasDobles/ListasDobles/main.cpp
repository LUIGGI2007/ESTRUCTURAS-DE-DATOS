#include "Lista.h"
#include <iostream>

using namespace std;

int main() {
    ListaDoble miLista;

    cout << "=== INSERTANDO DATOS QUEMADOS ===\n";
    miLista.insertar("Carlos");
    miLista.insertar("Ana");
    miLista.insertar("Mateo");
    miLista.insertar("Sofia");

    // Mostrar la lista en ambos sentidos
    cout << "\n=== INVERTIR LISTA ===\n";
    miLista.invertir();

    cout << "\n=== LISTA INVERTIDA ===\n";
    miLista.imprimirAdelante();
    miLista.imprimirAtras();
    // Mostrar la lista en ambos sentidos

    cout << "\n=== MOSTRAR LISTA ===\n";
    miLista.imprimirAdelante();
    miLista.imprimirAtras();

    cout << "\n=== PRUEBA DE BUSQUEDA ===\n";
    miLista.buscar("Ana");
    miLista.buscar("Luis");

    cout << "\n=== PRUEBA DE ELIMINACION ===\n";
    miLista.eliminar("Ana");

    cout << "\n=== LISTA FINAL ===\n";
    miLista.imprimirAdelante();
    miLista.imprimirAtras();

    return 0;
}