#ifndef REGISTROASISTENCIA_H
#define REGISTROASISTENCIA_H

#include "Fecha.h"
#include "FranjaHoraria.h"
#include "Hora.h"

#include <string>
#include <iosfwd>

class RegistroAsistencia
{
public:
    RegistroAsistencia();
    RegistroAsistencia(const std::string& id, const std::string& cedulaDocente,
                       const Fecha& fecha, const std::string& codigoNRC,
                       int idFranja);

    const std::string& getId() const;
    const std::string& getCedulaDocente() const;
    const Fecha& getFecha() const;
    const std::string& getCodigoNRC() const;
    int getIdFranja() const;
    const Hora& getEntrada() const;
    const Hora& getSalida() const;
    bool tieneEntrada() const;
    bool tieneSalida() const;
    void setEntrada(const Hora& hora);
    void setSalida(const Hora& hora);
    std::string determinarEstado(Hora horaEntrada, FranjaHoraria franja) const;
    bool registrarEntrada(Hora horaEntrada, FranjaHoraria franja);
    bool registrarSalida(Hora horaSalida, FranjaHoraria franja);
    bool operator==(const RegistroAsistencia& otro) const;
    friend std::ostream& operator<<(std::ostream& salida,
                                    const RegistroAsistencia& registro);

    class Docente** docente;
    class Hora** hora;
    class FranjaHoraria** franjaHoraria;
    class Fecha** fecha;

private:
    std::string id;
    std::string cedulaDocente;
    Fecha fechaRegistro;
    std::string codigoNRC;
    int idFranja;
    Hora horaEntrada;
    Hora horaSalida;
    bool entradaRegistrada;
    bool salidaRegistrada;
};

#endif
