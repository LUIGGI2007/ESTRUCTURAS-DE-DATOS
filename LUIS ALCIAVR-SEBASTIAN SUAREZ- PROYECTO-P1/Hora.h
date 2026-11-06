#ifndef HORA_H
#define HORA_H

#include <string>

class Hora
{
public:
    Hora();
    Hora(int hora, int minuto, int segundo = 0);
    int getHora() const;
    int getMinuto() const;
    int getSegundo() const;
    void setHora(int valor);
    void setMinuto(int valor);
    void setSegundo(int valor);
    int minutosDelDia() const;
    std::string toString() const;
    std::string toStringHoraMinuto() const;
    static bool parsear(const std::string& texto, Hora& resultado);
    bool operator==(const Hora& otra) const;
    bool operator<(const Hora& otra) const;
    bool operator<=(const Hora& otra) const;
    bool operator>(const Hora& otra) const;
    bool operator>=(const Hora& otra) const;

private:
    int hora;
    int minuto;
    int segundo;
};

#endif
