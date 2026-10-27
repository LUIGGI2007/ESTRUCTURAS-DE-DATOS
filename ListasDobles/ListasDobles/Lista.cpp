#include "Lista.h"

ListaDoble::ListaDoble() {
    cabeza = nullptr;
    cola = nullptr;
}

void ListaDoble::insertar(string n) {
    Nodo* nuevo = new Nodo(n);

    if (cabeza == nullptr) {
        cabeza = nuevo;
        cola = nuevo;
    } else {
        cola->setSiguiente(nuevo);
        nuevo->setAnterior(cola);
        cola = nuevo;
    }

    cout << "Insertado: " << n << endl;
}

void ListaDoble::buscar(string n) {
    Nodo* actual = cabeza;

    while (actual != nullptr) {
        if (actual->getNombre() == n) {
            cout << "-> Nombre encontrado: " << actual->getNombre() << endl;
            return;
        }
        actual = actual->getSiguiente();
    }

    cout << "-> Nombre '" << n << "' no encontrado.\n";
}

void ListaDoble::eliminar(string n) {
    Nodo* actual = cabeza;

    while (actual != nullptr && actual->getNombre() != n) {
        actual = actual->getSiguiente();
    }

    if (actual == nullptr) {
        cout << "-> No se puede eliminar. Nombre '" << n << "' no encontrado.\n";
        return;
    }

    if (actual == cabeza && actual == cola) {
        cabeza = nullptr;
        cola = nullptr;
    }
    else if (actual == cabeza) {
        cabeza = cabeza->getSiguiente();
        cabeza->setAnterior(nullptr);
    }
    else if (actual == cola) {
        cola = cola->getAnterior();
        cola->setSiguiente(nullptr);
    }
    else {
        actual->getAnterior()->setSiguiente(actual->getSiguiente());
        actual->getSiguiente()->setAnterior(actual->getAnterior());
    }

    delete actual;
    cout << "-> Se elimino correctamente: " << n << endl;
}

void ListaDoble::imprimirAdelante() {
    if (cabeza == nullptr) {
        cout << "La lista esta vacia.\n";
        return;
    }

    Nodo* actual = cabeza;
    cout << "Lista (Inicio a Fin): NULL <-> ";
    while (actual != nullptr) {
        cout << actual->getNombre() << " <-> ";
        actual = actual->getSiguiente();
    }
    cout << "NULL\n";
}

void ListaDoble::imprimirAtras() {
    if (cola == nullptr) {
        cout << "La lista esta vacia.\n";
        return;
    }

    Nodo* actual = cola;
    cout << "Lista (Fin a Inicio): NULL <-> ";
    while (actual != nullptr) {
        cout << actual->getNombre() << " <-> ";
        actual = actual->getAnterior();
    }
    cout << "NULL\n";
}

//NUEVA FUNCION PARA INVERTIR LA LISTA
void ListaDoble::invertir() {
    Nodo* actual = cabeza;
    Nodo* temporal = nullptr;

    while (actual != nullptr) {
        temporal = actual->getSiguiente();

        actual->setSiguiente(actual->getAnterior());
        actual->setAnterior(temporal);

        actual = temporal;
    }

    temporal = cabeza;
    cabeza = cola;
    cola = temporal;

    cout << "-> Lista invertida correctamente.\n";
}

ListaDoble::~ListaDoble() {
    Nodo* actual = cabeza;
    while (actual != nullptr) {
        Nodo* temporal = actual;
        actual = actual->getSiguiente();
        delete temporal;
    }
}