#ifndef FECHA_H
#define FECHA_H

#include <string>

class Fecha
{
public:
    Fecha();
    Fecha(int dia, int mes, int anio);
    int getDia() const;
    int getMes() const;
    int getAnio() const;
    void setDia(int valor);
    void setMes(int valor);
    void setAnio(int valor);
    std::string toString() const;
    static Fecha hoy();
    static bool parsear(const std::string& texto, Fecha& resultado);
    bool operator==(const Fecha& otra) const;

    class Hora** hora;

private:
    int dia;
    int mes;
    int anio;
};

#endif
