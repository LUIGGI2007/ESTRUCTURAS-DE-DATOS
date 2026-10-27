#include "Nodo.h"

Nodo::Nodo(string n) {
    nombre = n;
    siguiente = nullptr;
    anterior = nullptr;
}

string Nodo::getNombre() { 
    return nombre; 
}

void Nodo::setNombre(string n) { 
    nombre = n; 
}

Nodo* Nodo::getSiguiente() { 
    return siguiente; 
}

void Nodo::setSiguiente(Nodo* sig) { 
    siguiente = sig; 
}

Nodo* Nodo::getAnterior() { 
    return anterior; 
}

void Nodo::setAnterior(Nodo* ant) { 
    anterior = ant; 
}