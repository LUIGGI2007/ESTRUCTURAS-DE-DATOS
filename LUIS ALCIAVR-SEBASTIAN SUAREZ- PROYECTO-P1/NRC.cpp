#include "NRC.h"
#include "ValidadorCedula.h"

#include <ostream>
#include <stdexcept>

NRC::NRC() : periodo(nullptr) {}
NRC::NRC(const std::string& codigoValor, const std::string& nombreValor,
         const std::string& cedulaDocenteValor, const std::string& codigoPeriodoValor)
    : periodo(nullptr), codigo(codigoValor), nombre(nombreValor),
      cedulaDocente(cedulaDocenteValor), codigoPeriodo(codigoPeriodoValor)
{
    ValidadorCedula validador;
    if (!ValidadorCedula::validarCodigoNRC(codigo) ||
        !ValidadorCedula::validarMateria(nombre) ||
        !validador.validarCedula(cedulaDocente) ||
        !ValidadorCedula::validarCodigoPeriodo(codigoPeriodo))
        throw std::invalid_argument("Datos de NRC invalidos");
}
const std::string& NRC::getCodigo() const { return codigo; }
const std::string& NRC::getNombre() const { return nombre; }
const std::string& NRC::getCedulaDocente() const { return cedulaDocente; }
const std::string& NRC::getCodigoPeriodo() const { return codigoPeriodo; }
void NRC::setCodigo(const std::string& valor)
{
    if (!ValidadorCedula::validarCodigoNRC(valor))
        throw std::invalid_argument("Codigo NRC invalido");
    codigo = valor;
}
void NRC::setNombre(const std::string& valor)
{
    if (!ValidadorCedula::validarMateria(valor))
        throw std::invalid_argument("Nombre de materia invalido");
    nombre = valor;
}
void NRC::setCedulaDocente(const std::string& valor)
{
    ValidadorCedula validador;
    if (!validador.validarCedula(valor))
        throw std::invalid_argument("Cedula docente invalida");
    cedulaDocente = valor;
}
void NRC::setCodigoPeriodo(const std::string& valor)
{
    if (!ValidadorCedula::validarCodigoPeriodo(valor))
        throw std::invalid_argument("Codigo de periodo invalido");
    codigoPeriodo = valor;
}
bool NRC::operator==(const NRC& otro) const { return codigo == otro.codigo; }
std::ostream& operator<<(std::ostream& salida, const NRC& nrc)
{
    return salida << nrc.getCodigo() << " | " << nrc.getNombre()
                  << " | Docente " << nrc.getCedulaDocente()
                  << " | Periodo " << nrc.getCodigoPeriodo();
}
