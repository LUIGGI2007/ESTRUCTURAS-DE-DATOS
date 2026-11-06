#ifndef LISTASDOBLE_T_H
#define LISTASDOBLE_T_H

#include "Nodo_T.h"

#include <cstddef>
#include <iostream>

template <typename T>
class ListasDoble_T
{
public:
    ListasDoble_T() : cabeza(nullptr), cola(nullptr), cantidad(0) {}
    ~ListasDoble_T() { limpiar(); }

    ListasDoble_T(const ListasDoble_T&) = delete;
    ListasDoble_T& operator=(const ListasDoble_T&) = delete;

    Nodo_T<T>* getCabeza() const { return cabeza; }
    Nodo_T<T>* getCola() const { return cola; }
    bool estaVacia() const { return cabeza == nullptr; }
    std::size_t tamanio() const { return cantidad; }

    void insertar(const T& dato)
    {
        Nodo_T<T>* nuevo = new Nodo_T<T>(dato);
        if (estaVacia())
        {
            cabeza = cola = nuevo;
        }
        else
        {
            nuevo->setAnterior(cola);
            nuevo->setSiguiente(cabeza);
            cola->setSiguiente(nuevo);
            cabeza->setAnterior(nuevo);
            cola = nuevo;
        }
        ++cantidad;
    }

    Nodo_T<T>* buscar(const T& valor) const
    {
        Nodo_T<T>* actual = cabeza;
        for (std::size_t i = 0; i < cantidad; ++i)
        {
            if (actual->getDato() == valor)
                return actual;
            actual = actual->getSiguiente();
        }
        return nullptr;
    }

    template <typename Predicado>
    Nodo_T<T>* buscar(Predicado predicado) const
    {
        Nodo_T<T>* actual = cabeza;
        for (std::size_t indice = 0; indice < cantidad; ++indice)
        {
            if (predicado(actual->getDato()))
                return actual;
            actual = actual->getSiguiente();
        }
        return nullptr;
    }

    bool eliminar(const T& valor)
    {
        Nodo_T<T>* objetivo = buscar(valor);
        if (objetivo == nullptr)
            return false;
        if (cantidad == 1)
        {
            cabeza = cola = nullptr;
        }
        else
        {
            objetivo->getAnterior()->setSiguiente(objetivo->getSiguiente());
            objetivo->getSiguiente()->setAnterior(objetivo->getAnterior());
            if (objetivo == cabeza)
                cabeza = objetivo->getSiguiente();
            if (objetivo == cola)
                cola = objetivo->getAnterior();
        }
        delete objetivo;
        --cantidad;
        return true;
    }

    template <typename Funcion>
    void recorrer(Funcion funcion)
    {
        Nodo_T<T>* actual = cabeza;
        for (std::size_t i = 0; i < cantidad; ++i)
        {
            funcion(actual->getDato());
            actual = actual->getSiguiente();
        }
    }

    template <typename Funcion>
    void recorrer(Funcion funcion) const
    {
        const Nodo_T<T>* actual = cabeza;
        for (std::size_t i = 0; i < cantidad; ++i)
        {
            funcion(actual->getDato());
            actual = actual->getSiguiente();
        }
    }

    void imprimir() const
    {
        recorrer([](const T& dato) { std::cout << dato << '\n'; });
    }

private:
    void limpiar()
    {
        while (cabeza != nullptr)
        {
            Nodo_T<T>* eliminado = cabeza;
            if (cantidad == 1)
            {
                cabeza = cola = nullptr;
            }
            else
            {
                cabeza = cabeza->getSiguiente();
                cabeza->setAnterior(cola);
                cola->setSiguiente(cabeza);
            }
            delete eliminado;
            --cantidad;
        }
    }

    Nodo_T<T>* cabeza;
    Nodo_T<T>* cola;
    std::size_t cantidad;
};

#endif
