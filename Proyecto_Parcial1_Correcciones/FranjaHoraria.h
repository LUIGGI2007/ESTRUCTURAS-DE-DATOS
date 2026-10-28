#ifndef FRANJAHORARIA_H
#define FRANJAHORARIA_H

#include <iosfwd>
#include <string>

class FranjaHoraria {
private:
    int id;
    std::string horaInicio;
    std::string horaFin;
    int inicioMinutos;
    int finMinutos;

    FranjaHoraria(int id, const std::string& horaInicio, const std::string& horaFin,
                  int inicioMinutos, int finMinutos);

public:
    FranjaHoraria();

    int getId() const;
    const std::string& getHoraInicio() const;
    const std::string& getHoraFin() const;
    int getInicioMinutos() const;
    int getFinMinutos() const;

    static const FranjaHoraria* obtenerTodas(int& cantidad);
    static const FranjaHoraria* buscarPorId(int id);
    friend std::ostream& operator<<(std::ostream& salida, const FranjaHoraria& franja);
};

#endif
