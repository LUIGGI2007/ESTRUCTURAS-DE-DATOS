#ifndef NODO_H
#define NODO_H

class Nodo {
private:
    int dato;
    Nodo* siguiente;
public:
    Nodo();
    Nodo(int valor);
    int getDato() const;
    void setDato(int nuevoDato);
    Nodo* getSiguiente() const;
    void setSiguiente(Nodo* nuevoSiguiente);
};

#endif
