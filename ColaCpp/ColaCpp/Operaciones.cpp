#include "Operaciones.h"
#include <iostream>
using namespace std;

void insertar(Nodo*& frente, Nodo*& fin, int valor) {
    // Reservar memoria y asignar el dato mediante el constructor.
    Nodo* nuevo = new Nodo(valor);

    if (frente == nullptr) {
        frente = nuevo;
    } else {
        // Conectar el ultimo nodo con el nuevo.
        fin->setSiguiente(nuevo);
    }
    // Mover el final al nodo insertado.
    fin = nuevo;
}

void eliminar(Nodo*& frente, Nodo*& fin) {
    if (frente == nullptr) {
        cout << "La cola esta vacia.\n";
        return;
    }

    Nodo* auxiliar = frente;
    frente = frente->getSiguiente();
    if (frente == nullptr) {
        fin = nullptr;
    }
    delete auxiliar;
}

bool buscar(const Nodo* frente, int valor) {
    const Nodo* actual = frente;
    while (actual != nullptr) {
        if (actual->getDato() == valor) {
            return true;
        }
        actual = actual->getSiguiente();
    }
    return false;
}

void imprimir(const Nodo* frente) {
    const Nodo* actual = frente;
    cout << "Cola (frente a fin): ";
    if (actual == nullptr) {
        cout << "vacia";
    }
    while (actual != nullptr) {
        cout << actual->getDato() << " ";
        actual = actual->getSiguiente();
    }
    cout << '\n';
}

// Contar cuántas veces aparece un elemento en la cola.
int contarElemento(const Nodo* frente, int valor) {
    int contador = 0;
    const Nodo* actual = frente;

    while (actual != nullptr) {
        if (actual->getDato() == valor) {
            contador++;
        }

        actual = actual->getSiguiente();
    }

    return contador;
}