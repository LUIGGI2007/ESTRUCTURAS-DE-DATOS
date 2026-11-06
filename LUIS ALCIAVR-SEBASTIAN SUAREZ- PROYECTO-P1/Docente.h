#ifndef DOCENTE_H
#define DOCENTE_H

#include "Persona.h"
#include <iosfwd>
#include <string>

class Docente : public Persona
{
public:
    Docente();
    Docente(const std::string& cedula, const std::string& nombre,
            const std::string& apellido, const std::string& contrasenia,
            const std::string& correo);
    bool ingresarDatos();
    std::string tipoPersona() const override;
    const std::string& getCorreo() const;
    const std::string& getIdDocente() const;
    void setCorreo(const std::string& valor);
    void setIdDocente(const std::string& valor);
    bool operator==(const Docente& otro) const;
    friend std::ostream& operator<<(std::ostream& salida,
                                    const Docente& docente);

    class NRC** nRC;

private:
    std::string mail;
};

#endif
