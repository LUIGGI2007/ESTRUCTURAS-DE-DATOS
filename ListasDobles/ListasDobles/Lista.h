#ifndef LISTA_H
#define LISTA_H

#include "Nodo.h"
#include <iostream>
#include <string>

using namespace std;

class ListaDoble {
private:
    Nodo* cabeza;
    Nodo* cola;

public:
    ListaDoble();
    ~ListaDoble();

    void insertar(string n);
    void buscar(string n);
    void eliminar(string n);
    void imprimirAdelante();
    void imprimirAtras();
    void invertir(); //funcion nueva para invertir la lista
};

#endif