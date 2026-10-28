#include "FranjaHoraria.h"

#include <ostream>

FranjaHoraria::FranjaHoraria(int id, const std::string& horaInicio,
                             const std::string& horaFin, int inicioMinutos,
                             int finMinutos)
    : id(id), horaInicio(horaInicio), horaFin(horaFin),
      inicioMinutos(inicioMinutos), finMinutos(finMinutos) {}

FranjaHoraria::FranjaHoraria()
    : id(0), horaInicio(), horaFin(), inicioMinutos(0), finMinutos(0) {}

int FranjaHoraria::getId() const {
    return id;
}

const std::string& FranjaHoraria::getHoraInicio() const {
    return horaInicio;
}

const std::string& FranjaHoraria::getHoraFin() const {
    return horaFin;
}

int FranjaHoraria::getInicioMinutos() const {
    return inicioMinutos;
}

int FranjaHoraria::getFinMinutos() const {
    return finMinutos;
}

const FranjaHoraria* FranjaHoraria::obtenerTodas(int& cantidad) {
    static const FranjaHoraria franjas[7] = {
        FranjaHoraria(1, "07:00", "09:00", 7 * 60, 9 * 60),
        FranjaHoraria(2, "09:00", "11:00", 9 * 60, 11 * 60),
        FranjaHoraria(3, "11:00", "13:00", 11 * 60, 13 * 60),
        FranjaHoraria(4, "13:00", "15:00", 13 * 60, 15 * 60),
        FranjaHoraria(5, "15:00", "17:00", 15 * 60, 17 * 60),
        FranjaHoraria(6, "17:00", "19:00", 17 * 60, 19 * 60),
        FranjaHoraria(7, "19:00", "21:00", 19 * 60, 21 * 60)
    };
    cantidad = 7;
    return franjas;
}

const FranjaHoraria* FranjaHoraria::buscarPorId(int id) {
    int cantidad = 0;
    const FranjaHoraria* franjas = obtenerTodas(cantidad);
    for (int i = 0; i < cantidad; ++i) {
        if (franjas[i].getId() == id) {
            return &franjas[i];
        }
    }
    return nullptr;
}

std::ostream& operator<<(std::ostream& salida, const FranjaHoraria& franja) {
    salida << "Franja " << franja.id << ": " << franja.horaInicio << " - "
           << franja.horaFin;
    return salida;
}
