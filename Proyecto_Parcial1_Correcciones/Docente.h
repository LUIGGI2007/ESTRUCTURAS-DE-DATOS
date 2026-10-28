#ifndef DOCENTE_H
#define DOCENTE_H

#include "Persona.h"

#include <iosfwd>
#include <string>

class Docente : public Persona {
private:
    std::string usuario;
    std::string contrasena;
    std::string horaEntrada;
    std::string horaSalida;
    bool activo;

public:
    Docente();
    Docente(const std::string& cedula, const std::string& primerNombre,
            const std::string& segundoNombre, const std::string& apellido,
            const std::string& contrasena);
    ~Docente() override;

    const std::string& getUsuario() const;
    const std::string& getContrasena() const;
    const std::string& getHoraEntrada() const;
    const std::string& getHoraSalida() const;
    bool estaActivo() const;

    void setUsuario(const std::string& valor);
    void setContrasena(const std::string& valor);
    void actualizarAsistencia(const std::string& entrada, const std::string& salida);
    void setActivo(bool valor);
    std::string generarUsuario(unsigned int correlativo = 0) const;

    void imprimir(std::ostream& salida) const override;
    friend std::ostream& operator<<(std::ostream& salida, const Docente& docente);
};

#endif
