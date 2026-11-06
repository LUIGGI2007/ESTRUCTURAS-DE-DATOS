#ifndef PERSONA_H
#define PERSONA_H

#include <string>

class Persona
{
public:
    Persona();
    Persona(const std::string& cedula, const std::string& nombre,
            const std::string& apellido, const std::string& contrasenia);
    virtual ~Persona();

    const std::string& getCedula() const;
    const std::string& getNombre() const;
    const std::string& getApellido() const;
    const std::string& getContrasenia() const;
    void setCedula(const std::string& valor);
    void setNombre(const std::string& valor);
    void setApellido(const std::string& valor);
    void setContrasenia(const std::string& valor);
    std::string nombreCompleto() const;
    virtual std::string tipoPersona() const;

protected:
    std::string cedula;
    std::string nombre;
    std::string apellido;
    std::string contrasenia;
};

#endif
