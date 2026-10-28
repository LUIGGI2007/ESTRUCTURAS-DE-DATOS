#include "Nodo.h"

Nodo::Nodo() {
    dato = 0;
    siguiente = nullptr;
}

Nodo::Nodo(int valor) {
    dato = valor;
    siguiente = nullptr;
}

int Nodo::getDato() const {
    return dato;
}

void Nodo::setDato(int nuevoDato) {
    dato = nuevoDato;
}

Nodo* Nodo::getSiguiente() const {
    return siguiente;
}

void Nodo::setSiguiente(Nodo* nuevoSiguiente) {
    siguiente = nuevoSiguiente;
}
