/* Universidad de las Fuerzas Armadas 
     Ciencias de la Computacion
        Estructura de Datos

Grupo: 3            Integrantes: Kevin Cabezas, Martin Paz 
Fecha: 00/10/2026                Erick Sabando, Adrian Toapanta
        Tema: Pilas
*/

// Clase Pila.cpp
  

#include "Pila.h"
#include "Persona.h"

//constructor
Pila::Pila() {}

//desctructor
Pila::~Pila(){
  while(this->tope!=nullptr){
    this->desapilar();
  }
}

//metodo ingresar
void Pila::apilar(const Persona &p) {
  
  Nodo *nuevo = new Nodo(p);
  nuevo->setSiguiente(this->tope);
  this->tope = nuevo;
}

//metodo eliminar
void Pila::desapilar() {
  if (tope == nullptr) {
    std::cout << "\nPila vacia, no se puede desapilar" << std::endl;
    return;
  }
  Nodo *temp = this->tope;
  this->tope= this->tope->getSiguiente();
  std::cout<<"Desapilado con exito" << std::endl;
  delete temp;
}

//metodo imprimir
void Pila::imprimir() {
  Nodo *actual = this->tope;
  if (tope == nullptr) {
    std::cout << "\nPila vacia" << std::endl;
    return;
  }
  while (actual != nullptr) {
    Persona impr = actual->getDato();
    std::cout << "\nPersona: " << "\nNombre: " << impr.getNombre()
              << "\nApellido: " << impr.getApellido()
              << "\nCedula: " << impr.getCedula() << std::endl;
    actual = actual->getSiguiente();
  }
}

// Devuelve el numero actual de elementos de la pila
int Pila::sizePila() {
    int contador = 0;
    Nodo *actual = this->tope;

    while (actual != nullptr) {
        contador++;
        actual = actual->getSiguiente();
    }

    return contador;
}

//metodo buscar
bool Pila::buscar(const std::string &cedula) {

  if (tope == nullptr) {
    std::cout << "La pila esta vacia. Por favor ingresar un Persona" << std::endl;
    return false;
  }
  Nodo *actual = this->tope;

  while (actual != nullptr) {
    Persona busc = actual->getDato();
    if (busc.getCedula()==cedula) {
      std::cout << "\nPersona: " << "\nNombre: " << busc.getNombre()
                << "\nApellido: " << busc.getApellido()
                << "\nCedula: " << busc.getCedula() << std::endl;
      return true;
    }
    actual = actual->getSiguiente();
    
  }
  std::cout<<"Persona con la cedula: " << cedula << " no encontrada" << std::endl;
  return false;
}