#ifndef PERSONA_H
#define PERSONA_H

#include <iosfwd>
#include <string>

class Persona {
protected:
    std::string cedula;
    std::string primerNombre;
    std::string segundoNombre;
    std::string apellido;

public:
    Persona();
    Persona(const std::string& cedula, const std::string& primerNombre,
            const std::string& segundoNombre, const std::string& apellido);
    virtual ~Persona();

    const std::string& getCedula() const;
    const std::string& getPrimerNombre() const;
    const std::string& getSegundoNombre() const;
    const std::string& getApellido() const;

    void setCedula(const std::string& valor);
    void setNombres(const std::string& primerNombre, const std::string& apellido);
    void setNombres(const std::string& primerNombre, const std::string& segundoNombre,
                    const std::string& apellido);

    bool operator==(const Persona& otra) const;
    virtual void imprimir(std::ostream& salida) const;
    friend std::ostream& operator<<(std::ostream& salida, const Persona& persona);
};

#endif
