#include "Docente.h"

#include <cctype>
#include <ostream>

namespace {
std::string primeraLetra(const std::string& texto) {
    if (texto.empty()) {
        return std::string();
    }

    const unsigned char inicial = static_cast<unsigned char>(texto[0]);
    std::size_t longitud = 1;
    if ((inicial & 0xE0) == 0xC0) {
        longitud = 2;
    } else if ((inicial & 0xF0) == 0xE0) {
        longitud = 3;
    } else if ((inicial & 0xF8) == 0xF0) {
        longitud = 4;
    }
    return texto.substr(0, longitud);
}

std::string normalizarParaUsuario(const std::string& texto) {
    std::string resultado;
    for (std::size_t i = 0; i < texto.size(); ++i) {
        const unsigned char caracter = static_cast<unsigned char>(texto[i]);
        if (caracter == ' ' || caracter == '\t') {
            continue;
        }
        if (caracter == 0xC3 && i + 1 < texto.size()) {
            const unsigned char siguiente = static_cast<unsigned char>(texto[i + 1]);
            if (siguiente >= 0x80 && siguiente <= 0x9E && siguiente != 0x97) {
                resultado += static_cast<char>(caracter);
                resultado += static_cast<char>(siguiente + 0x20);
                ++i;
                continue;
            }
        }
        resultado += static_cast<char>(
            caracter < 0x80 ? std::tolower(caracter) : caracter);
    }
    return resultado;
}
}

Docente::Docente()
    : Persona(), usuario(), contrasena(), horaEntrada(), horaSalida(), activo(true) {}

Docente::Docente(const std::string& cedula, const std::string& primerNombre,
                 const std::string& segundoNombre, const std::string& apellido,
                 const std::string& contrasena)
    : Persona(cedula, primerNombre, segundoNombre, apellido), usuario(),
      contrasena(contrasena), horaEntrada(), horaSalida(), activo(true) {}

Docente::~Docente() {}

const std::string& Docente::getUsuario() const {
    return usuario;
}

const std::string& Docente::getContrasena() const {
    return contrasena;
}

const std::string& Docente::getHoraEntrada() const {
    return horaEntrada;
}

const std::string& Docente::getHoraSalida() const {
    return horaSalida;
}

bool Docente::estaActivo() const {
    return activo;
}

void Docente::setUsuario(const std::string& valor) {
    usuario = valor;
}

void Docente::setContrasena(const std::string& valor) {
    contrasena = valor;
}

void Docente::actualizarAsistencia(const std::string& entrada, const std::string& salida) {
    horaEntrada = entrada;
    horaSalida = salida;
}

void Docente::setActivo(bool valor) {
    activo = valor;
}

std::string Docente::generarUsuario(unsigned int correlativo) const {
    const std::string iniciales = primeraLetra(primerNombre) + primeraLetra(segundoNombre);
    std::string base = normalizarParaUsuario(iniciales + apellido);
    if (base.empty()) {
        return std::string();
    }

    if (correlativo != 0) {
        base += std::to_string(correlativo);
    }
    return base + "@espe.edu.ec";
}

void Docente::imprimir(std::ostream& salida) const {
    Persona::imprimir(salida);
    salida << " | Usuario: " << usuario << " | Activo: " << (activo ? "Sí" : "No");
}

std::ostream& operator<<(std::ostream& salida, const Docente& docente) {
    docente.imprimir(salida);
    return salida;
}
