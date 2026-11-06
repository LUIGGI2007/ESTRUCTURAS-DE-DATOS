#include "Fecha.h"
#include "ValidadorCedula.h"

#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace
{
std::string formatearFecha(int dia, int mes, int anio)
{
    std::ostringstream salida;
    salida << std::setfill('0') << std::setw(2) << dia << '/'
           << std::setw(2) << mes << '/' << std::setw(4) << anio;
    return salida.str();
}
}

Fecha::Fecha() : hora(nullptr), dia(1), mes(1), anio(2000) {}
Fecha::Fecha(int diaValor, int mesValor, int anioValor)
    : hora(nullptr), dia(diaValor), mes(mesValor), anio(anioValor)
{
    if (!ValidadorCedula::validarFecha(formatearFecha(dia, mes, anio)))
        throw std::invalid_argument("Fecha no valida");
}
int Fecha::getDia() const { return dia; }
int Fecha::getMes() const { return mes; }
int Fecha::getAnio() const { return anio; }
void Fecha::setDia(int valor)
{
    if (!ValidadorCedula::validarFecha(formatearFecha(valor, mes, anio)))
        throw std::invalid_argument("Dia no valido");
    dia = valor;
}
void Fecha::setMes(int valor)
{
    if (!ValidadorCedula::validarFecha(formatearFecha(dia, valor, anio)))
        throw std::invalid_argument("Mes no valido");
    mes = valor;
}
void Fecha::setAnio(int valor)
{
    if (!ValidadorCedula::validarFecha(formatearFecha(dia, mes, valor)))
        throw std::invalid_argument("Anio no valido");
    anio = valor;
}
std::string Fecha::toString() const
{
    return formatearFecha(dia, mes, anio);
}
Fecha Fecha::hoy()
{
    const std::time_t ahora = std::time(nullptr);
    const std::tm* local = std::localtime(&ahora);
    if (local == nullptr)
        throw std::runtime_error("No se pudo obtener la fecha actual");
    return Fecha(local->tm_mday, local->tm_mon + 1, local->tm_year + 1900);
}
bool Fecha::parsear(const std::string& texto, Fecha& resultado)
{
    if (ValidadorCedula::validarFecha(texto))
    {
        resultado = Fecha(std::stoi(texto.substr(0, 2)),
                          std::stoi(texto.substr(3, 2)),
                          std::stoi(texto.substr(6, 4)));
        return true;
    }

    if (!ValidadorCedula::validarFechaIso(texto))
        return false;
    resultado = Fecha(std::stoi(texto.substr(8, 2)),
                      std::stoi(texto.substr(5, 2)),
                      std::stoi(texto.substr(0, 4)));
    return true;
}
bool Fecha::operator==(const Fecha& otra) const
{
    return dia == otra.dia && mes == otra.mes && anio == otra.anio;
}
