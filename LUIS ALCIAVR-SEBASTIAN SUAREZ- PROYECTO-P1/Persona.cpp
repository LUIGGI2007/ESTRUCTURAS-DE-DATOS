#include "Persona.h"
#include "ValidadorCedula.h"

#include <stdexcept>

Persona::Persona() = default;
Persona::Persona(const std::string& cedulaValor, const std::string& nombreValor,
                 const std::string& apellidoValor, const std::string& contraseniaValor)
    : cedula(cedulaValor), nombre(nombreValor), apellido(apellidoValor),
      contrasenia(contraseniaValor)
{
    ValidadorCedula validador;
    if (!validador.validarCedula(cedula) ||
        !ValidadorCedula::validarNombres(nombre) ||
        !ValidadorCedula::validarNombres(apellido) ||
        !ValidadorCedula::validarContrasenia(contrasenia))
        throw std::invalid_argument("Datos de persona invalidos");
}
Persona::~Persona() = default;
const std::string& Persona::getCedula() const { return cedula; }
const std::string& Persona::getNombre() const { return nombre; }
const std::string& Persona::getApellido() const { return apellido; }
const std::string& Persona::getContrasenia() const { return contrasenia; }
void Persona::setCedula(const std::string& valor)
{
    ValidadorCedula validador;
    if (!validador.validarCedula(valor))
        throw std::invalid_argument("Cedula ecuatoriana invalida");
    cedula = valor;
}
void Persona::setNombre(const std::string& valor)
{
    if (!ValidadorCedula::validarNombres(valor))
        throw std::invalid_argument("Nombre invalido");
    nombre = valor;
}
void Persona::setApellido(const std::string& valor)
{
    if (!ValidadorCedula::validarNombres(valor))
        throw std::invalid_argument("Apellido invalido");
    apellido = valor;
}
void Persona::setContrasenia(const std::string& valor)
{
    if (!ValidadorCedula::validarContrasenia(valor))
        throw std::invalid_argument("Contrasena invalida");
    contrasenia = valor;
}
std::string Persona::nombreCompleto() const { return nombre + " " + apellido; }
std::string Persona::tipoPersona() const { return "Persona"; }
