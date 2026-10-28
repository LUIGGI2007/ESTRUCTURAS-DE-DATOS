/* Universidad de las Fuerzas Armadas 
     Ciencias de la Computacion
        Estructura de Datos

Grupo: 3            Integrantes: Kevin Cabezas, Martin Paz 
Fecha: 00/10/2026                Erick Sabando, Adrian Toapanta
        Tema: Pilas
*/

//Clase Persona.cpp


#include "Persona.h"

//constructor
Persona::Persona(){
  this -> nombre= "";
  this-> apellido = "";
  this -> cedula = "";
}
//constructor con parametros
Persona::Persona(const std::string& nombre, const std::string& apellido, const std::string& cedula){
this->nombre=nombre;
this->apellido=apellido;
this->cedula=cedula;
}
//destructor
Persona::~Persona(){

}

//metodo pedir datos
void Persona::pedirDatos(){
std::string entrada;

    std::cout << "Ingrese los datos de la persona\n" << std::endl;

    std::cout << "Nombre: ";
    std::cin >> entrada;
    this->setNombre(entrada); 

    std::cout << "Apellido: ";
    std::cin >> entrada;
    this->setApellido(entrada);

    std::cout << "Cedula: ";
    std::cin >> entrada;
    this->setCedula(entrada);
}

//setters y getters
void Persona::setNombre(const std::string& nombre){
  this->nombre=nombre;

}  

std::string Persona::getNombre()const{
  return this->nombre;
}

void Persona::setApellido(const std::string& apellido){
this->apellido=apellido;
}

std::string Persona::getApellido()const{
  return this->apellido;
}

void Persona::setCedula(const std::string& cedula){
 this->cedula=cedula;
}

std::string Persona::getCedula()const{
  return this->cedula;
}