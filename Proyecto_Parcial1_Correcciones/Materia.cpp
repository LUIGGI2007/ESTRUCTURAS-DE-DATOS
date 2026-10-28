#include "Materia.h"

#include <ostream>

Materia::Materia() : nrc(0), nombreMateria() {}

Materia::Materia(int nrc, const std::string& nombreMateria)
    : nrc(nrc), nombreMateria(nombreMateria) {}

int Materia::getNrc() const {
    return nrc;
}

const std::string& Materia::getNombreMateria() const {
    return nombreMateria;
}

void Materia::setNombreMateria(const std::string& valor) {
    nombreMateria = valor;
}

bool Materia::operator==(const Materia& otra) const {
    return nrc == otra.nrc;
}

std::ostream& operator<<(std::ostream& salida, const Materia& materia) {
    salida << "NRC: " << materia.nrc << " | Materia: " << materia.nombreMateria;
    return salida;
}
