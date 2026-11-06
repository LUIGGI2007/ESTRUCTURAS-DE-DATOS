#include "RegistroAsistencia.h"
#include <ostream>

RegistroAsistencia::RegistroAsistencia()
    : docente(nullptr), hora(nullptr), franjaHoraria(nullptr), fecha(nullptr),
      idFranja(0), horaEntrada(), horaSalida(), entradaRegistrada(false),
      salidaRegistrada(false) {}

RegistroAsistencia::RegistroAsistencia(const std::string& idValor,
                                       const std::string& cedula,
                                       const Fecha& fechaValor,
                                       const std::string& nrc, int franjaId)
    : docente(nullptr), hora(nullptr), franjaHoraria(nullptr), fecha(nullptr),
      id(idValor), cedulaDocente(cedula), fechaRegistro(fechaValor),
      codigoNRC(nrc), idFranja(franjaId), horaEntrada(), horaSalida(),
      entradaRegistrada(false), salidaRegistrada(false) {}

const std::string& RegistroAsistencia::getId() const { return id; }
const std::string& RegistroAsistencia::getCedulaDocente() const { return cedulaDocente; }
const Fecha& RegistroAsistencia::getFecha() const { return fechaRegistro; }
const std::string& RegistroAsistencia::getCodigoNRC() const { return codigoNRC; }
int RegistroAsistencia::getIdFranja() const { return idFranja; }
const Hora& RegistroAsistencia::getEntrada() const { return horaEntrada; }
const Hora& RegistroAsistencia::getSalida() const { return horaSalida; }
bool RegistroAsistencia::tieneEntrada() const { return entradaRegistrada; }
bool RegistroAsistencia::tieneSalida() const { return salidaRegistrada; }
void RegistroAsistencia::setEntrada(const Hora& horaValor)
{
    horaEntrada = horaValor;
    entradaRegistrada = true;
}
void RegistroAsistencia::setSalida(const Hora& horaValor)
{
    horaSalida = horaValor;
    salidaRegistrada = true;
}
std::string RegistroAsistencia::determinarEstado(Hora horaValor,
                                                  FranjaHoraria franja) const
{
    const int marca = horaValor.minutosDelDia();
    const int inicio = franja.getHoraInicio().minutosDelDia();
    if (marca <= inicio + 10)
        return "PUNTUAL";
    return "ATRASADO";
}
bool RegistroAsistencia::registrarEntrada(Hora horaValor, FranjaHoraria franja)
{
    const int marca = horaValor.minutosDelDia();
    const int inicio = franja.getHoraInicio().minutosDelDia();
    if (entradaRegistrada || marca < inicio || marca > inicio + 10)
        return false;
    setEntrada(horaValor);
    return true;
}
bool RegistroAsistencia::registrarSalida(Hora horaValor, FranjaHoraria franja)
{
    const int marca = horaValor.minutosDelDia();
    const int fin = franja.getHoraFin().minutosDelDia();
    if (!entradaRegistrada || salidaRegistrada || marca < fin - 10 || marca > fin ||
        marca < horaEntrada.minutosDelDia())
        return false;
    setSalida(horaValor);
    return true;
}
bool RegistroAsistencia::operator==(const RegistroAsistencia& otro) const
{
    return id == otro.id;
}
std::ostream& operator<<(std::ostream& salida,
                         const RegistroAsistencia& registro)
{
    return salida << registro.getId() << " | " << registro.getCedulaDocente()
                  << " | " << registro.getFecha().toString() << " | NRC "
                  << registro.getCodigoNRC() << " | Franja "
                  << registro.getIdFranja() << " | Entrada "
                  << (registro.tieneEntrada() ? registro.getEntrada().toString() :
                                                  "--:--:--")
                  << " | Salida "
                  << (registro.tieneSalida() ? registro.getSalida().toString() :
                                                 "--:--:--");
}
