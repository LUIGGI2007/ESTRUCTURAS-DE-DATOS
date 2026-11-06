#include "Docente.h"
#include "IngresoDatos.h"
#include "ValidadorCedula.h"

#include <iostream>
#include <ostream>
#include <stdexcept>

Docente::Docente() : Persona(), nRC(nullptr) {}

Docente::Docente(const std::string& cedula, const std::string& nombre,
                 const std::string& apellido, const std::string& contrasenia,
                 const std::string& correo)
    : Persona(cedula, nombre, apellido, contrasenia),
      nRC(nullptr), mail(correo)
{
    if (!mail.empty() &&
        !ValidadorCedula::validarCorreo(mail, "espe.edu.ec"))
        throw std::invalid_argument("Correo institucional invalido");
}

bool Docente::ingresarDatos()
{
    IngresoDatos entrada;
    ValidadorCedula validador;
    do
    {
        cedula = entrada.leerCampo("Cedula (10 digitos, 0 para cancelar): ",
                                   "^(0|[0-9]{10})$",
                                   "^[0-9]$");
        if (ValidadorCedula::esCancelar(cedula))
            return false;
        if (!validador.validarCedula(cedula))
            std::cout << "Cedula ecuatoriana invalida.\n";
    } while (!validador.validarCedula(cedula));
    nombre = entrada.leerCampo(
        "Nombres (ej. Juan Carlos, 0 para cancelar): ",
        "^(0|[a-zA-ZáéíóúÁÉÍÓÚñÑ\\s]{1,40})$",
        "^[0a-zA-ZáéíóúÁÉÍÓÚñÑ ]$");
    if (ValidadorCedula::esCancelar(nombre))
        return false;
    apellido = entrada.leerCampo(
        "Apellidos (ej. Perez Garcia, 0 para cancelar): ",
        "^(0|[a-zA-ZáéíóúÁÉÍÓÚñÑ\\s]{1,40})$",
        "^[0a-zA-ZáéíóúÁÉÍÓÚñÑ ]$");
    if (ValidadorCedula::esCancelar(apellido))
        return false;
    do
    {
        contrasenia = entrada.leerContraseniaOCancelar(
            "Contrasena (4 a 20 caracteres, 0 para cancelar): ");
        if (ValidadorCedula::esCancelar(contrasenia))
            return false;
        if (!ValidadorCedula::validarContrasenia(contrasenia))
            std::cout << "Contrasena no valida.\n";
    } while (!ValidadorCedula::validarContrasenia(contrasenia));
    mail.clear();
    return true;
}

std::string Docente::tipoPersona() const { return "Docente"; }
const std::string& Docente::getCorreo() const { return mail; }
const std::string& Docente::getIdDocente() const { return cedula; }
void Docente::setCorreo(const std::string& valor)
{
    if (!ValidadorCedula::validarCorreo(valor, "espe.edu.ec"))
        throw std::invalid_argument("Correo institucional invalido");
    mail = valor;
}
void Docente::setIdDocente(const std::string& valor) { setCedula(valor); }
bool Docente::operator==(const Docente& otro) const { return cedula == otro.cedula; }
std::ostream& operator<<(std::ostream& salida, const Docente& docente)
{
    return salida << docente.getCedula() << " | " << docente.nombreCompleto()
                  << " | " << docente.getCorreo();
}
