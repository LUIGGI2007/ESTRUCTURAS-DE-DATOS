#include "Persona.h"

#include <ostream>

Persona::Persona() : cedula(), primerNombre(), segundoNombre(), apellido() {}

Persona::Persona(const std::string& cedula, const std::string& primerNombre,
                 const std::string& segundoNombre, const std::string& apellido)
    : cedula(cedula), primerNombre(primerNombre), segundoNombre(segundoNombre),
      apellido(apellido) {}

Persona::~Persona() {}

const std::string& Persona::getCedula() const {
    return cedula;
}

const std::string& Persona::getPrimerNombre() const {
    return primerNombre;
}

const std::string& Persona::getSegundoNombre() const {
    return segundoNombre;
}

const std::string& Persona::getApellido() const {
    return apellido;
}

void Persona::setCedula(const std::string& valor) {
    cedula = valor;
}

void Persona::setNombres(const std::string& primerNombre, const std::string& apellido) {
    this->primerNombre = primerNombre;
    segundoNombre.clear();
    this->apellido = apellido;
}

void Persona::setNombres(const std::string& primerNombre, const std::string& segundoNombre,
                         const std::string& apellido) {
    this->primerNombre = primerNombre;
    this->segundoNombre = segundoNombre;
    this->apellido = apellido;
}

bool Persona::operator==(const Persona& otra) const {
    return cedula == otra.cedula;
}

void Persona::imprimir(std::ostream& salida) const {
    salida << "Cédula: " << cedula << " | Nombre: " << primerNombre;
    if (!segundoNombre.empty()) {
        salida << ' ' << segundoNombre;
    }
    salida << ' ' << apellido;
}

std::ostream& operator<<(std::ostream& salida, const Persona& persona) {
    persona.imprimir(salida);
    return salida;
}
