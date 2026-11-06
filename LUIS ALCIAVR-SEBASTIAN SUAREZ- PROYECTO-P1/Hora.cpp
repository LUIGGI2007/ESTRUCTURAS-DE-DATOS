#include "Hora.h"
#include "ValidadorCedula.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>

Hora::Hora() : hora(0), minuto(0), segundo(0) {}
Hora::Hora(int horaValor, int minutoValor, int segundoValor)
    : hora(horaValor), minuto(minutoValor), segundo(segundoValor)
{
    if (!ValidadorCedula::validarHoraCompleta(toString()))
        throw std::invalid_argument("Hora no valida");
}
int Hora::getHora() const { return hora; }
int Hora::getMinuto() const { return minuto; }
int Hora::getSegundo() const { return segundo; }
void Hora::setHora(int valor)
{
    std::ostringstream candidato;
    candidato << std::setfill('0') << std::setw(2) << valor << ':'
              << std::setw(2) << minuto << ':' << std::setw(2) << segundo;
    if (!ValidadorCedula::validarHoraCompleta(candidato.str()))
        throw std::invalid_argument("Hora fuera de rango");
    hora = valor;
}
void Hora::setMinuto(int valor)
{
    std::ostringstream candidato;
    candidato << std::setfill('0') << std::setw(2) << hora << ':'
              << std::setw(2) << valor << ':' << std::setw(2) << segundo;
    if (!ValidadorCedula::validarHoraCompleta(candidato.str()))
        throw std::invalid_argument("Minuto fuera de rango");
    minuto = valor;
}
void Hora::setSegundo(int valor)
{
    std::ostringstream candidato;
    candidato << std::setfill('0') << std::setw(2) << hora << ':'
              << std::setw(2) << minuto << ':' << std::setw(2) << valor;
    if (!ValidadorCedula::validarHoraCompleta(candidato.str()))
        throw std::invalid_argument("Segundo fuera de rango");
    segundo = valor;
}
int Hora::minutosDelDia() const { return hora * 60 + minuto; }
std::string Hora::toString() const
{
    std::ostringstream salida;
    salida << std::setfill('0') << std::setw(2) << hora << ':'
           << std::setw(2) << minuto << ':' << std::setw(2) << segundo;
    return salida.str();
}
std::string Hora::toStringHoraMinuto() const
{
    std::ostringstream salida;
    salida << std::setfill('0') << std::setw(2) << hora << ':'
           << std::setw(2) << minuto;
    return salida.str();
}
bool Hora::parsear(const std::string& texto, Hora& resultado)
{
    if (ValidadorCedula::validarHoraCompleta(texto))
    {
        resultado = Hora(std::stoi(texto.substr(0, 2)),
                         std::stoi(texto.substr(3, 2)),
                         std::stoi(texto.substr(6, 2)));
        return true;
    }
    if (ValidadorCedula::validarHora(texto))
    {
        resultado = Hora(std::stoi(texto.substr(0, 2)),
                         std::stoi(texto.substr(3, 2)), 0);
        return true;
    }
    return false;
}
bool Hora::operator==(const Hora& otra) const
{
    return hora == otra.hora && minuto == otra.minuto && segundo == otra.segundo;
}
bool Hora::operator<(const Hora& otra) const
{
    return minutosDelDia() < otra.minutosDelDia() ||
           (minutosDelDia() == otra.minutosDelDia() && segundo < otra.segundo);
}
bool Hora::operator<=(const Hora& otra) const { return *this < otra || *this == otra; }
bool Hora::operator>(const Hora& otra) const { return otra < *this; }
bool Hora::operator>=(const Hora& otra) const { return otra <= *this; }
