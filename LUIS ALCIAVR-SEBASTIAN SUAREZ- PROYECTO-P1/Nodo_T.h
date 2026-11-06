#ifndef NODO_T_H
#define NODO_T_H

template <typename T>
class Nodo_T
{
public:
    explicit Nodo_T(const T& valor)
        : dato(valor), siguiente(this), anterior(this) {}

    T& getDato() { return dato; }
    const T& getDato() const { return dato; }
    void setDato(const T& valor) { dato = valor; }
    Nodo_T<T>* getSiguiente() const { return siguiente; }
    void setSiguiente(Nodo_T<T>* valor) { siguiente = valor; }
    Nodo_T<T>* getAnterior() const { return anterior; }
    void setAnterior(Nodo_T<T>* valor) { anterior = valor; }

private:
    T dato;
    Nodo_T<T>* siguiente;
    Nodo_T<T>* anterior;
};

#endif
