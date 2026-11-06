#ifndef PERIODO_H
#define PERIODO_H

#include <string>
#include <iosfwd>

class Periodo
{
public:
    Periodo();
    Periodo(const std::string& codigo, const std::string& descripcion);
    const std::string& getCodigo() const;
    const std::string& getDescripcion() const;
    void setCodigo(const std::string& valor);
    void setDescripcion(const std::string& valor);
    bool operator==(const Periodo& otro) const;
    friend std::ostream& operator<<(std::ostream& salida,
                                    const Periodo& periodo);

private:
    std::string codigo;
    std::string descripcion;
};

#endif
