#include "Periodo.h"
#include "ValidadorCedula.h"

#include <ostream>
#include <stdexcept>

Periodo::Periodo() = default;
Periodo::Periodo(const std::string& codigoValor, const std::string& descripcionValor)
    : codigo(codigoValor), descripcion(descripcionValor)
{
    if (!ValidadorCedula::validarCodigoPeriodo(codigo) ||
        !ValidadorCedula::validarDescripcionPeriodo(descripcion))
        throw std::invalid_argument("Datos de periodo invalidos");
}
const std::string& Periodo::getCodigo() const { return codigo; }
const std::string& Periodo::getDescripcion() const { return descripcion; }
void Periodo::setCodigo(const std::string& valor)
{
    if (!ValidadorCedula::validarCodigoPeriodo(valor))
        throw std::invalid_argument("Codigo de periodo invalido");
    codigo = valor;
}
void Periodo::setDescripcion(const std::string& valor)
{
    if (!ValidadorCedula::validarDescripcionPeriodo(valor))
        throw std::invalid_argument("Descripcion de periodo invalida");
    descripcion = valor;
}
bool Periodo::operator==(const Periodo& otro) const { return codigo == otro.codigo; }
std::ostream& operator<<(std::ostream& salida, const Periodo& periodo)
{
    return salida << periodo.getCodigo() << " | " << periodo.getDescripcion();
}
