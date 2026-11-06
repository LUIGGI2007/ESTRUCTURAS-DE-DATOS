#include "FranjaHoraria.h"
#include "ValidadorCedula.h"

#include <algorithm>
#include <cctype>
#include <ostream>
#include <stdexcept>

namespace
{
std::string diaCanonico(std::string dia)
{
    std::transform(dia.begin(), dia.end(), dia.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    const std::pair<const char*, const char*> equivalencias[] = {
        {"á", "a"}, {"é", "e"}, {"í", "i"}, {"ó", "o"}, {"ú", "u"},
        {"Á", "a"}, {"É", "e"}, {"Í", "i"}, {"Ó", "o"}, {"Ú", "u"}};
    for (const auto& equivalencia : equivalencias)
    {
        std::size_t posicion = dia.find(equivalencia.first);
        while (posicion != std::string::npos)
        {
            dia.replace(posicion, std::char_traits<char>::length(equivalencia.first),
                        equivalencia.second);
            posicion = dia.find(equivalencia.first, posicion + 1);
        }
    }
    return dia;
}
}

FranjaHoraria::FranjaHoraria()
    : hora(nullptr), horaInicio(), horaFin(), diaSemana(), id(0), codigoNRC() {}
FranjaHoraria::FranjaHoraria(int idValor, const std::string& dia,
                             const Hora& inicio, const Hora& fin,
                             const std::string& nrc)
    : hora(nullptr), horaInicio(inicio), horaFin(fin), diaSemana(diaCanonico(dia)),
      id(idValor), codigoNRC(nrc)
{
    if (!ValidadorCedula::validarId(std::to_string(id)) ||
        !ValidadorCedula::validarCodigoNRC(nrc))
        throw std::invalid_argument("Identificador o codigo NRC de franja invalido");
    if (!diaValido(dia))
        throw std::invalid_argument(
            "DÍA NO LABORABLE: las clases solo se dictan de lunes a viernes. "
            "No es posible asignar sábado ni domingo.");
    if (!ValidadorCedula::validarBloqueHorario(
            inicio.toStringHoraMinuto() + "-" + fin.toStringHoraMinuto()))
        throw std::invalid_argument(
            "HORARIO NO PERMITIDO: las clases solo pueden ser 07-09, 09-11, "
            "11-13, 13-15, 15-17, 17-19 o 19-21.");
}
int FranjaHoraria::getId() const { return id; }
const std::string& FranjaHoraria::getDiaSemana() const { return diaSemana; }
const Hora& FranjaHoraria::getHoraInicio() const { return horaInicio; }
const Hora& FranjaHoraria::getHoraFin() const { return horaFin; }
const std::string& FranjaHoraria::getCodigoNRC() const { return codigoNRC; }
void FranjaHoraria::setId(int valor)
{
    if (!ValidadorCedula::validarId(std::to_string(valor)))
        throw std::invalid_argument("Identificador de franja invalido");
    id = valor;
}
void FranjaHoraria::setDiaSemana(const std::string& valor)
{
    if (!diaValido(valor))
        throw std::invalid_argument(
            "DÍA NO LABORABLE: las clases solo se dictan de lunes a viernes. "
            "No es posible asignar sábado ni domingo.");
    diaSemana = diaCanonico(valor);
}
void FranjaHoraria::setHoraInicio(const Hora& valor)
{
    if (!ValidadorCedula::validarBloqueHorario(
            valor.toStringHoraMinuto() + "-" + horaFin.toStringHoraMinuto()))
        throw std::invalid_argument(
            "HORARIO NO PERMITIDO: las clases solo pueden ser 07-09, 09-11, "
            "11-13, 13-15, 15-17, 17-19 o 19-21.");
    horaInicio = valor;
}
void FranjaHoraria::setHoraFin(const Hora& valor)
{
    if (!ValidadorCedula::validarBloqueHorario(
            horaInicio.toStringHoraMinuto() + "-" + valor.toStringHoraMinuto()))
        throw std::invalid_argument(
            "HORARIO NO PERMITIDO: las clases solo pueden ser 07-09, 09-11, "
            "11-13, 13-15, 15-17, 17-19 o 19-21.");
    horaFin = valor;
}
void FranjaHoraria::setCodigoNRC(const std::string& valor)
{
    if (!ValidadorCedula::validarCodigoNRC(valor))
        throw std::invalid_argument("Codigo NRC de franja invalido");
    codigoNRC = valor;
}
bool FranjaHoraria::operator==(const FranjaHoraria& otra) const { return id == otra.id; }
std::ostream& operator<<(std::ostream& salida, const FranjaHoraria& franja)
{
    return salida << franja.getId() << " | NRC " << franja.getCodigoNRC()
                  << " | " << franja.getDiaSemana() << ' '
                  << franja.getHoraInicio().toStringHoraMinuto() << '-'
                  << franja.getHoraFin().toStringHoraMinuto();
}
bool FranjaHoraria::diaValido(const std::string& dia)
{
    return ValidadorCedula::validarDia(dia);
}
