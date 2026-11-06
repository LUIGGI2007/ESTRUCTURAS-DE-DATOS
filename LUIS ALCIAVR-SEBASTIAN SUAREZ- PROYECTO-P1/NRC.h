#ifndef NRC_H
#define NRC_H

#include <string>
#include <iosfwd>

class NRC
{
public:
    NRC();
    NRC(const std::string& codigo, const std::string& nombre,
        const std::string& cedulaDocente, const std::string& codigoPeriodo);
    const std::string& getCodigo() const;
    const std::string& getNombre() const;
    const std::string& getCedulaDocente() const;
    const std::string& getCodigoPeriodo() const;
    void setCodigo(const std::string& valor);
    void setNombre(const std::string& valor);
    void setCedulaDocente(const std::string& valor);
    void setCodigoPeriodo(const std::string& valor);
    bool operator==(const NRC& otro) const;
    friend std::ostream& operator<<(std::ostream& salida, const NRC& nrc);

    class Periodo** periodo;

private:
    std::string codigo;
    std::string nombre;
    std::string cedulaDocente;
    std::string codigoPeriodo;
};

#endif
