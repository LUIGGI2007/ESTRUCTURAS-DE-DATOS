#ifndef NODO_H
#define NODO_H

template <typename T>
class Nodo
{
public:
    Nodo(T dato);

    T getDato();
    void setDato(T dato);

    Nodo<T>* getSiguiente();
    void setSiguiente(Nodo<T>* siguiente);

    Nodo<T>* getAnterior();
    void setAnterior(Nodo<T>* anterior);

private:
    T dato;
    Nodo<T>* siguiente;
    Nodo<T>* anterior;
};

template <typename T>
Nodo<T>::Nodo(T dato)
{
    this->dato = dato;
    siguiente = nullptr;
    anterior = nullptr;
}

template <typename T>
T Nodo<T>::getDato()
{
    return dato;
}

template <typename T>
void Nodo<T>::setDato(T dato)
{
    this->dato = dato;
}

template <typename T>
Nodo<T>* Nodo<T>::getSiguiente()
{
    return siguiente;
}

template <typename T>
void Nodo<T>::setSiguiente(Nodo<T>* siguiente)
{
    this->siguiente = siguiente;
}

template <typename T>
Nodo<T>* Nodo<T>::getAnterior()
{
    return anterior;
}

template <typename T>
void Nodo<T>::setAnterior(Nodo<T>* anterior)
{
    this->anterior = anterior;
}

#endif
