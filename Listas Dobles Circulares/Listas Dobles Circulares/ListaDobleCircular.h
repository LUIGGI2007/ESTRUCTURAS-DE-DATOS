#ifndef LISTADOBLECIRCULAR_H
#define LISTADOBLECIRCULAR_H

#include <iostream>
#include "Nodo.h"

#include <stdexcept> // Para manejar excepciones de rango

using namespace std;

template <typename T>
class ListaDobleCircular
{
public:
    ListaDobleCircular();
    ~ListaDobleCircular();

    bool estaVacia();
    int getTamanio();

    void insertarAlInicio(T valor);
    void insertarAlFinal(T valor);
    Nodo<T>* buscar(T dato);
    void eliminar(T dato);
    void imprimir();

    T obtenerEnPosicion(int pos); //Nueva función para obtener el valor en una posición específica

private:
    Nodo<T>* cabeza;
    int tamanio;
};

template <typename T>
ListaDobleCircular<T>::ListaDobleCircular()
{
    cabeza = nullptr;
    tamanio = 0;
}

template <typename T>
ListaDobleCircular<T>::~ListaDobleCircular()
{
    while (!estaVacia())
    {
        eliminar(cabeza->getDato());
    }
}

template <typename T>
bool ListaDobleCircular<T>::estaVacia()
{
    return cabeza == nullptr;
}

template <typename T>
int ListaDobleCircular<T>::getTamanio()
{
    return tamanio;
}

template <typename T>
void ListaDobleCircular<T>::insertarAlFinal(T valor)
{
    Nodo<T>* nuevo = new Nodo<T>(valor);

    if (estaVacia())
    {
        nuevo->setSiguiente(nuevo);
        nuevo->setAnterior(nuevo);
        cabeza = nuevo;
    }
    else
    {
        Nodo<T>* ultimo = cabeza->getAnterior();

        nuevo->setSiguiente(cabeza);
        nuevo->setAnterior(ultimo);
        ultimo->setSiguiente(nuevo);
        cabeza->setAnterior(nuevo);
    }
    tamanio++;
}

template <typename T>
void ListaDobleCircular<T>::insertarAlInicio(T valor)
{
    Nodo<T>* nuevo = new Nodo<T>(valor);

    if (estaVacia())
    {
        nuevo->setSiguiente(nuevo);
        nuevo->setAnterior(nuevo);
    }
    else
    {
        Nodo<T>* ultimo = cabeza->getAnterior();

        nuevo->setSiguiente(cabeza);
        nuevo->setAnterior(ultimo);
        ultimo->setSiguiente(nuevo);
        cabeza->setAnterior(nuevo);
    }
    cabeza = nuevo;   // el nuevo nodo pasa a ser la cabeza
    tamanio++;
}

template <typename T>
Nodo<T>* ListaDobleCircular<T>::buscar(T dato)
{
    if (estaVacia())
    {
        return nullptr;
    }

    Nodo<T>* actual = cabeza;

    do
    {
        if (actual->getDato() == dato)
        {
            return actual;
        }
        actual = actual->getSiguiente();
    } while (actual != cabeza);

    return nullptr;
}

template <typename T>
void ListaDobleCircular<T>::eliminar(T dato)
{
    if (estaVacia())
    {
        cout << "La lista esta vacia" << endl;
        return;
    }

    Nodo<T>* actual = cabeza;

    do
    {
        if (actual->getDato() == dato)
        {
            // Caso 1: solo existe un nodo
            if (actual->getSiguiente() == actual)
            {
                cabeza = nullptr;
            }
            else
            {
                // Enlazar el anterior con el siguiente
                actual->getAnterior()->setSiguiente(actual->getSiguiente());
                actual->getSiguiente()->setAnterior(actual->getAnterior());

                // Si se elimina la cabeza
                if (actual == cabeza)
                {
                    cabeza = actual->getSiguiente();
                }
            }

            delete actual;
            tamanio--;
            cout << "Elemento eliminado" << endl;
            return;
        }

        actual = actual->getSiguiente();

    } while (actual != cabeza);

    cout << "Elemento no encontrado" << endl;
}

template <typename T>
void ListaDobleCircular<T>::imprimir()
{
    if (estaVacia())
    {
        cout << "Lista vacia" << endl;
        return;
    }

    Nodo<T>* actual = cabeza;

    do
    {
        cout << actual->getDato() << " ";
        actual = actual->getSiguiente();
    } while (actual != cabeza);

    cout << endl;
}

// Nueva función para obtener el valor en una posición específica
template <typename T>
T ListaDobleCircular<T>::obtenerEnPosicion(int pos)
{
    if (pos < 0 || pos >= tamanio)
    {
        throw out_of_range("Posicion fuera de rango");
    }

    Nodo<T>* actual = cabeza;

    for (int i = 0; i < pos; i++)
    {
        actual = actual->getSiguiente();
    }

    return actual->getDato();
}

#endif
