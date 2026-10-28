#ifndef REGISTROMOVIMIENTOS_H
#define REGISTROMOVIMIENTOS_H

#include "Docente.h"
#include "FranjaHoraria.h"
#include "Materia.h"

#include <iosfwd>
#include <string>

class RegistroAsistencia {
private:
    Docente* docente;
    Materia* materia;
    FranjaHoraria franja;
    std::string fecha;
    std::string horaEntrada;
    std::string horaSalida;
    bool activo;

public:
    RegistroAsistencia(Docente* docente, Materia* materia, const FranjaHoraria& franja,
                       const std::string& fecha, const std::string& horaEntrada);

    Docente* getDocente() const;
    Materia* getMateria() const;
    const FranjaHoraria& getFranja() const;
    const std::string& getFecha() const;
    const std::string& getHoraEntrada() const;
    const std::string& getHoraSalida() const;
    bool estaActivo() const;

    void cerrar(const std::string& horaSalida);
    friend std::ostream& operator<<(std::ostream& salida,
                                    const RegistroAsistencia& registro);
};

class RegistroMovimientos {
private:
    struct Nodo {
        RegistroAsistencia movimiento;
        Nodo* anterior;
        Nodo* siguiente;

        explicit Nodo(const RegistroAsistencia& movimiento);
    };

    Nodo* inicio;
    int cantidad;

    bool sesionActivaRec(const Nodo* actual, const Nodo* primero,
                         const Docente* docente) const;
    bool cruceRec(const Nodo* actual, const Nodo* primero, const Docente* docente,
                  const std::string& fecha, int idFranja) const;
    RegistroAsistencia* sesionActivaRec(Nodo* actual, Nodo* primero,
                                        const Docente* docente) const;
    void imprimirRec(const Nodo* actual, const Nodo* primero,
                     std::ostream& salida) const;

    RegistroMovimientos(const RegistroMovimientos&);
    RegistroMovimientos& operator=(const RegistroMovimientos&);

public:
    RegistroMovimientos();
    ~RegistroMovimientos();

    bool agregar(Docente* docente, Materia* materia, const FranjaHoraria& franja,
                 const std::string& fecha, const std::string& horaEntrada);
    bool haySesionActiva(const Docente* docente) const;
    bool hayCruce(const Docente* docente, const std::string& fecha, int idFranja) const;
    RegistroAsistencia* obtenerSesionActiva(const Docente* docente) const;
    int getCantidad() const;
    void imprimir(std::ostream& salida) const;
};

#endif
