/* Universidad de las Fuerzas Armadas
     Ciencias de la Computacion
        Estructura de Datos

Grupo: 3            Integrantes: Kevin Cabezas, Martin Paz
Fecha: 00/10/2026                Erick Sabando, Adrian Toapanta
        Tema: Pilas
*/

// Clase Pila.h

#pragma once
#include "IPila.h"
#include "Nodo.h"

/// class Pila

class Pila : public IPila {
private:
  Nodo *tope;

public:
  // constructor
  Pila();
  // destructor
  virtual ~Pila();

  // setter y getter
  void setTope(Nodo *tope);
  Nodo *getTope() const;

  // metodos de la interface
  // metodo ingresar
  void apilar(const Persona &p) override;
  // metodo eliminar
  void desapilar() override;
  // metodo buscar
  bool buscar(const std::string &cedula) override;
  // metodo imprimir
  void imprimir() override;
  // metodo tamaño
  int sizePila() override;
};
