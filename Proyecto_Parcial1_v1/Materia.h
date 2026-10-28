#ifndef MATERIA_H
#define MATERIA_H

#include <iosfwd>
#include <string>

class Materia {
private:
    int nrc;
    std::string nombreMateria;

public:
    Materia();
    Materia(int nrc, const std::string& nombreMateria);

    int getNrc() const;
    const std::string& getNombreMateria() const;
    void setNombreMateria(const std::string& valor);

    bool operator==(const Materia& otra) const;
    friend std::ostream& operator<<(std::ostream& salida, const Materia& materia);
};

#endif
