#include "RegistroMovimientos.h"

#include <ostream>

RegistroAsistencia::RegistroAsistencia(Docente* docente, Materia* materia,
                                       const FranjaHoraria& franja,
                                       const std::string& fecha,
                                       const std::string& horaEntrada)
    : docente(docente), materia(materia), franja(franja), fecha(fecha),
      horaEntrada(horaEntrada), horaSalida(), activo(true) {}

Docente* RegistroAsistencia::getDocente() const {
    return docente;
}

Materia* RegistroAsistencia::getMateria() const {
    return materia;
}

const FranjaHoraria& RegistroAsistencia::getFranja() const {
    return franja;
}

const std::string& RegistroAsistencia::getFecha() const {
    return fecha;
}

const std::string& RegistroAsistencia::getHoraEntrada() const {
    return horaEntrada;
}

const std::string& RegistroAsistencia::getHoraSalida() const {
    return horaSalida;
}

bool RegistroAsistencia::estaActivo() const {
    return activo;
}

void RegistroAsistencia::cerrar(const std::string& horaSalida) {
    this->horaSalida = horaSalida;
    activo = false;
}

std::ostream& operator<<(std::ostream& salida, const RegistroAsistencia& registro) {
    salida << "Fecha: " << registro.fecha << " | " << *registro.docente
           << " | " << *registro.materia << " | " << registro.franja
           << " | Entrada: " << registro.horaEntrada
           << " | Salida: "
           << (registro.horaSalida.empty() ? "Pendiente" : registro.horaSalida)
           << " | Clase: " << (registro.activo ? "Activa" : "Cerrada");
    return salida;
}

RegistroMovimientos::Nodo::Nodo(const RegistroAsistencia& movimiento)
    : movimiento(movimiento), anterior(this), siguiente(this) {}

RegistroMovimientos::RegistroMovimientos() : inicio(nullptr), cantidad(0) {}

RegistroMovimientos::~RegistroMovimientos() {
    if (inicio == nullptr) {
        return;
    }

    Nodo* actual = inicio->siguiente;
    while (actual != inicio) {
        Nodo* siguiente = actual->siguiente;
        delete actual;
        actual = siguiente;
    }
    delete inicio;
}

bool RegistroMovimientos::agregar(Docente* docente, Materia* materia,
                                  const FranjaHoraria& franja,
                                  const std::string& fecha,
                                  const std::string& horaEntrada) {
    if (docente == nullptr || materia == nullptr || franja.getId() == 0) {
        return false;
    }

    Nodo* nuevo = new Nodo(RegistroAsistencia(docente, materia, franja, fecha, horaEntrada));
    if (inicio == nullptr) {
        inicio = nuevo;
    } else {
        Nodo* ultimo = inicio->anterior;
        nuevo->siguiente = inicio;
        nuevo->anterior = ultimo;
        ultimo->siguiente = nuevo;
        inicio->anterior = nuevo;
    }
    ++cantidad;
    return true;
}

bool RegistroMovimientos::sesionActivaRec(const Nodo* actual, const Nodo* primero,
                                          const Docente* docente) const {
    if (actual == nullptr || primero == nullptr) {
        return false;
    }
    if (actual->movimiento.getDocente() == docente && actual->movimiento.estaActivo()) {
        return true;
    }
    if (actual->siguiente == primero) {
        return false;
    }
    return sesionActivaRec(actual->siguiente, primero, docente);
}

bool RegistroMovimientos::haySesionActiva(const Docente* docente) const {
    return inicio != nullptr && docente != nullptr
        && sesionActivaRec(inicio, inicio, docente);
}

bool RegistroMovimientos::cruceRec(const Nodo* actual, const Nodo* primero,
                                   const Docente* docente, const std::string& fecha,
                                   int idFranja) const {
    if (actual == nullptr || primero == nullptr) {
        return false;
    }
    const RegistroAsistencia& movimiento = actual->movimiento;
    if (movimiento.getDocente() == docente && movimiento.getFecha() == fecha
        && movimiento.getFranja().getId() == idFranja) {
        return true;
    }
    if (actual->siguiente == primero) {
        return false;
    }
    return cruceRec(actual->siguiente, primero, docente, fecha, idFranja);
}

bool RegistroMovimientos::hayCruce(const Docente* docente, const std::string& fecha,
                                   int idFranja) const {
    return inicio != nullptr && docente != nullptr
        && cruceRec(inicio, inicio, docente, fecha, idFranja);
}

RegistroAsistencia* RegistroMovimientos::sesionActivaRec(
    Nodo* actual, Nodo* primero, const Docente* docente) const {
    if (actual == nullptr || primero == nullptr) {
        return nullptr;
    }
    if (actual->movimiento.getDocente() == docente && actual->movimiento.estaActivo()) {
        return &actual->movimiento;
    }
    if (actual->siguiente == primero) {
        return nullptr;
    }
    return sesionActivaRec(actual->siguiente, primero, docente);
}

RegistroAsistencia* RegistroMovimientos::obtenerSesionActiva(
    const Docente* docente) const {
    if (inicio == nullptr || docente == nullptr) {
        return nullptr;
    }
    return sesionActivaRec(inicio, inicio, docente);
}

int RegistroMovimientos::getCantidad() const {
    return cantidad;
}

void RegistroMovimientos::imprimirRec(const Nodo* actual, const Nodo* primero,
                                      std::ostream& salida) const {
    if (actual == nullptr || primero == nullptr) {
        return;
    }
    salida << actual->movimiento << '\n';
    if (actual->siguiente != primero) {
        imprimirRec(actual->siguiente, primero, salida);
    }
}

void RegistroMovimientos::imprimir(std::ostream& salida) const {
    if (inicio == nullptr) {
        salida << "No existen movimientos registrados.\n";
        return;
    }
    imprimirRec(inicio, inicio, salida);
}
