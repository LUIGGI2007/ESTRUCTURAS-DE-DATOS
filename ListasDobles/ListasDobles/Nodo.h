#ifndef NODO_H
#define NODO_H

#include <string>

using namespace std;

class Nodo {
private:
    string nombre;
    Nodo* siguiente;
    Nodo* anterior;

public:
    Nodo(string n);

    // Getters y Setters
    string getNombre();
    void setNombre(string n);

    Nodo* getSiguiente();
    void setSiguiente(Nodo* sig);

    Nodo* getAnterior();
    void setAnterior(Nodo* ant);
};

#endif