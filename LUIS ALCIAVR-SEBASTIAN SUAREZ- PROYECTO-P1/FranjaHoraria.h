#ifndef FRANJAHORARIA_H
#define FRANJAHORARIA_H

#include "Hora.h"
#include <iosfwd>
#include <string>

class FranjaHoraria
{
public:
    FranjaHoraria();
    FranjaHoraria(int id, const std::string& dia, const Hora& inicio,
                  const Hora& fin, const std::string& codigoNRC);
    int getId() const;
    const std::string& getDiaSemana() const;
    const Hora& getHoraInicio() const;
    const Hora& getHoraFin() const;
    const std::string& getCodigoNRC() const;
    void setId(int valor);
    void setDiaSemana(const std::string& valor);
    void setHoraInicio(const Hora& valor);
    void setHoraFin(const Hora& valor);
    void setCodigoNRC(const std::string& valor);
    bool operator==(const FranjaHoraria& otra) const;
    friend std::ostream& operator<<(std::ostream& salida,
                                    const FranjaHoraria& franja);
    static bool diaValido(const std::string& dia);
    class Hora** hora;

private:
    Hora horaInicio;
    Hora horaFin;
    std::string diaSemana;
    int id;
    std::string codigoNRC;
};

#endif
