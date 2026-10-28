#include <iostream>
#include "Pila.h"

using namespace std;

int main() {
    Pila miPila;
    int opcion = 0;
    int valor = 0;

    do {
        cout << "\n==========================" << endl;
        cout << "      MENU DE PILA        " << endl;
        cout << "==========================" << endl;
        cout << "1. Insertar elemento" << endl;
        cout << "2. Eliminar elemento (Cima)" << endl;
        cout << "3. Buscar elemento" << endl;
        cout << "4. Imprimir pila" << endl;
        cout << "5. Sumar elementos de la pila" << endl;
        cout << "6. Salir" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1:
                cout << "Ingrese el valor a insertar: ";
                cin >> valor;
                miPila.insertar(valor);
                cout << "-> Elemento " << valor << " insertado correctamente." << endl;
                break;

            case 2:
                if (miPila.estaVacia()) {
                    cout << "-> La pila esta vacia, no se puede eliminar." << endl;
                } else {
                    miPila.eliminar();
                    cout << "-> Elemento de la cima eliminado." << endl;
                }
                break;

            case 3:
                if (miPila.estaVacia()) {
                    cout << "-> La pila esta vacia." << endl;
                } else {
                    cout << "Ingrese el valor a buscar: ";
                    cin >> valor;
                    if (miPila.buscar(valor)) {
                        cout << "-> El elemento " << valor << " SI existe en la pila." << endl;
                    } else {
                        cout << "-> El elemento " << valor << " NO existe en la pila." << endl;
                    }
                }
                break;

            case 4:
                if (miPila.estaVacia()) {
                    cout << "-> La pila esta vacia." << endl;
                } else {
                    cout << "Pila actual (Cima -> Base): ";
                    miPila.imprimir();
                    cout << endl;
                }
                break;

            case 5:
                    cout << "Suma de los elementos de la pila: "
                    << miPila.sumarElementos() << endl;
                 break;

            case 6:
                 cout << "Saliendo del programa..." << endl;
                 break;

            default:
                cout << "Opcion invalida. Intente de nuevo." << endl;
                break;
        }
    } while (opcion != 6);

    return 0;
}