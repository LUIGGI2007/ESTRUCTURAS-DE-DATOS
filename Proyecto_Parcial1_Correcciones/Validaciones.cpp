#include "Validaciones.h"

#include "FranjaHoraria.h"

#include <conio.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <windows.h>

namespace {
bool esLetraLatin1(unsigned int puntoCodigo) {
    return (puntoCodigo >= 0x00C0 && puntoCodigo <= 0x00D6)
        || (puntoCodigo >= 0x00D8 && puntoCodigo <= 0x00F6)
        || (puntoCodigo >= 0x00F8 && puntoCodigo <= 0x00FF);
}

bool esCaracterIdentificador(unsigned char caracter) {
    return (caracter >= 'A' && caracter <= 'Z')
        || (caracter >= 'a' && caracter <= 'z')
        || (caracter >= '0' && caracter <= '9')
        || caracter == '@' || caracter == '.' || caracter == '_' || caracter == '-';
}

void borrarUltimoCaracter(std::string& texto) {
    if (texto.empty()) {
        return;
    }
    std::size_t inicio = texto.size() - 1;
    while (inicio > 0
           && (static_cast<unsigned char>(texto[inicio]) & 0xC0) == 0x80) {
        --inicio;
    }
    texto.erase(inicio);
    std::cout << "\b \b";
}

std::tm horaLocalActual() {
    const std::time_t ahora = std::time(nullptr);
    const std::tm* local = std::localtime(&ahora);
    return local == nullptr ? std::tm() : *local;
}

std::string quitarEspaciosExtremos(const std::string& texto) {
    const std::size_t primero = texto.find_first_not_of(' ');
    if (primero == std::string::npos) {
        return std::string();
    }
    const std::size_t ultimo = texto.find_last_not_of(' ');
    return texto.substr(primero, ultimo - primero + 1);
}
}

void configurarConsolaUTF8() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
}

void limpiarConsola() {
    std::system("cls");
}

namespace {
std::string leerTextoValidado(const std::string& mensaje, bool permitirEspacios) {
    std::cout << mensaje;
    std::string texto;
    std::string pendiente;
    int bytesEsperados = 0;

    while (true) {
        const int tecla = _getch();
        if (tecla == 13) {
            if (bytesEsperados != 0) {
                pendiente.clear();
                bytesEsperados = 0;
                std::cout << '\a';
            }
            std::cout << '\n';
            return quitarEspaciosExtremos(texto);
        }
        if (tecla == 8) {
            if (bytesEsperados != 0) {
                pendiente.clear();
                bytesEsperados = 0;
                std::cout << '\a';
            } else {
                borrarUltimoCaracter(texto);
            }
            continue;
        }
        if (tecla < 0 || tecla > 255) {
            continue;
        }

        const unsigned char caracter = static_cast<unsigned char>(tecla);
        if (bytesEsperados == 0) {
            if ((caracter >= 'A' && caracter <= 'Z')
                || (caracter >= 'a' && caracter <= 'z')
                || (permitirEspacios && caracter == ' ')) {
                texto += static_cast<char>(caracter);
                std::cout << static_cast<char>(caracter);
            } else if (caracter == 0xC2 || caracter == 0xC3) {
                pendiente = static_cast<char>(caracter);
                bytesEsperados = 2;
            } else {
                std::cout << '\a';
            }
            continue;
        }

        if ((caracter & 0xC0) != 0x80) {
            pendiente.clear();
            bytesEsperados = 0;
            std::cout << '\a';
            if ((caracter >= 'A' && caracter <= 'Z')
                || (caracter >= 'a' && caracter <= 'z')
                || (permitirEspacios && caracter == ' ')) {
                texto += static_cast<char>(caracter);
                std::cout << static_cast<char>(caracter);
            } else if (caracter == 0xC2 || caracter == 0xC3) {
                pendiente = static_cast<char>(caracter);
                bytesEsperados = 2;
            }
            continue;
        }

        pendiente += static_cast<char>(caracter);
        const unsigned int puntoCodigo =
            ((static_cast<unsigned char>(pendiente[0]) & 0x1F) << 6)
            | (static_cast<unsigned char>(pendiente[1]) & 0x3F);
        if (esLetraLatin1(puntoCodigo)) {
            texto += pendiente;
            std::cout << pendiente;
        } else {
            std::cout << '\a';
        }
        pendiente.clear();
        bytesEsperados = 0;
    }
}
}

std::string leerTexto(const std::string& mensaje) {
    return leerTextoValidado(mensaje, true);
}

std::string leerTextoObligatorio(const std::string& mensaje) {
    while (true) {
        const std::string valor = leerTexto(mensaje);
        if (!valor.empty() && valor.find_first_not_of(' ') != std::string::npos) {
            return valor;
        }
        std::cout << "El campo no puede quedar vacío ni contener solo espacios.\n";
    }
}

std::string leerTextoSinEspaciosObligatorio(const std::string& mensaje) {
    while (true) {
        const std::string valor = leerTextoValidado(mensaje, false);
        if (!valor.empty()) {
            return valor;
        }
        std::cout << "El campo no puede quedar vacío.\n";
    }
}

std::string leerIdentificador(const std::string& mensaje) {
    std::cout << mensaje;
    std::string valor;
    std::string pendiente;
    while (true) {
        const int tecla = _getch();
        if (tecla == 13) {
            if (!pendiente.empty()) {
                pendiente.clear();
                std::cout << '\a';
            }
            std::cout << '\n';
            return valor;
        }
        if (tecla == 8) {
            if (!pendiente.empty()) {
                pendiente.clear();
                std::cout << '\a';
            } else {
                borrarUltimoCaracter(valor);
            }
            continue;
        }
        if (tecla < 0 || tecla > 255) {
            continue;
        }

        const unsigned char caracter = static_cast<unsigned char>(tecla);
        if (pendiente.empty()) {
            if (esCaracterIdentificador(caracter)) {
                const bool esDigito = caracter >= '0' && caracter <= '9';
                const bool esCedulaEnCurso =
                    valor.empty() || valor.find_first_not_of("0123456789") == std::string::npos;
                if (!esDigito || !esCedulaEnCurso || valor.size() < 10) {
                    valor += static_cast<char>(caracter);
                    std::cout << static_cast<char>(caracter);
                } else {
                    std::cout << '\a';
                }
            } else if (caracter == 0xC2 || caracter == 0xC3) {
                pendiente = static_cast<char>(caracter);
            } else {
                std::cout << '\a';
            }
            continue;
        }

        if ((caracter & 0xC0) != 0x80) {
            pendiente.clear();
            std::cout << '\a';
            if (esCaracterIdentificador(caracter)) {
                const bool esDigito = caracter >= '0' && caracter <= '9';
                const bool esCedulaEnCurso =
                    valor.empty() || valor.find_first_not_of("0123456789") == std::string::npos;
                if (!esDigito || !esCedulaEnCurso || valor.size() < 10) {
                    valor += static_cast<char>(caracter);
                    std::cout << static_cast<char>(caracter);
                }
            } else if (caracter == 0xC2 || caracter == 0xC3) {
                pendiente = static_cast<char>(caracter);
            }
            continue;
        }

        pendiente += static_cast<char>(caracter);
        const unsigned int puntoCodigo =
            ((static_cast<unsigned char>(pendiente[0]) & 0x1F) << 6)
            | (static_cast<unsigned char>(pendiente[1]) & 0x3F);
        if (esLetraLatin1(puntoCodigo)) {
            valor += pendiente;
            std::cout << pendiente;
        } else {
            std::cout << '\a';
        }
        pendiente.clear();
    }
}

std::string leerNumerico(const std::string& mensaje) {
    std::cout << mensaje;
    std::string valor;
    while (true) {
        const int tecla = _getch();
        if (tecla == 13) {
            std::cout << '\n';
            return valor;
        }
        if (tecla == 8) {
            if (!valor.empty()) {
                valor.erase(valor.size() - 1);
                std::cout << "\b \b";
            }
        } else if (tecla >= '0' && tecla <= '9') {
            valor += static_cast<char>(tecla);
            std::cout << static_cast<char>(tecla);
        } else {
            std::cout << '\a';
        }
    }
}

std::string leerNumericoMaximo(const std::string& mensaje, std::size_t maximo) {
    std::cout << mensaje;
    std::string valor;
    while (true) {
        const int tecla = _getch();
        if (tecla == 13) {
            std::cout << '\n';
            return valor;
        }
        if (tecla == 8) {
            if (!valor.empty()) {
                valor.erase(valor.size() - 1);
                std::cout << "\b \b";
            }
        } else if (tecla >= '0' && tecla <= '9' && valor.size() < maximo) {
            valor += static_cast<char>(tecla);
            std::cout << static_cast<char>(tecla);
        } else {
            std::cout << '\a';
        }
    }
}

bool confirmarOpcion() {
    std::cout << "ENTER: ejecutar opción | ESC: volver al menú: ";
    const int tecla = _getch();
    std::cout << '\n';
    if (tecla == 27) {
        std::cout << "Opción cancelada; regresando al menú.\n";
        return false;
    }
    if (tecla == 13) {
        return true;
    }
    std::cout << "Tecla no válida; opción cancelada.\n";
    return false;
}

void esperarRetornoMenu() {
    std::cout << "\nPresione ENTER para volver al menú principal...";
    while (_getch() != 13) {
    }
}

std::string leerContrasena(const std::string& mensaje) {
    std::cout << mensaje;
    std::string valor;
    while (true) {
        const int tecla = _getch();
        if (tecla == 13) {
            std::cout << '\n';
            return valor;
        }
        if (tecla == 8) {
            if (!valor.empty()) {
                valor.erase(valor.size() - 1);
                std::cout << "\b \b";
            }
        } else if (tecla >= 32 && tecla <= 255) {
            valor += static_cast<char>(tecla);
            std::cout << '*';
        } else {
            std::cout << '\a';
        }
    }
}

std::string leerHoraManual(int& segundosDelDia) {
    int hora = 0;
    int minuto = 0;
    int segundo = 0;

    while (true) {
        const std::string valor = leerNumericoMaximo("Hora (00-23): ", 2);
        if (!valor.empty()
            && std::stoi(valor) >= 0 && std::stoi(valor) <= 23) {
            hora = std::stoi(valor);
            break;
        }
        std::cout << "Hora no válida; ingrese un valor entre 00 y 23.\n";
    }

    while (true) {
        const std::string valor = leerNumericoMaximo("Minuto (00-59): ", 2);
        if (!valor.empty()
            && std::stoi(valor) >= 0 && std::stoi(valor) <= 59) {
            minuto = std::stoi(valor);
            break;
        }
        std::cout << "Minuto no válido; ingrese un valor entre 00 y 59.\n";
    }

    while (true) {
        const std::string valor = leerNumericoMaximo("Segundo (00-59): ", 2);
        if (!valor.empty()
            && std::stoi(valor) >= 0 && std::stoi(valor) <= 59) {
            segundo = std::stoi(valor);
            break;
        }
        std::cout << "Segundo no válido; ingrese un valor entre 00 y 59.\n";
    }

    segundosDelDia = hora * 3600 + minuto * 60 + segundo;
    char texto[9];
    std::snprintf(texto, sizeof(texto), "%02d:%02d:%02d", hora, minuto, segundo);
    return texto;
}

std::string obtenerFechaActual() {
    const std::tm fecha = horaLocalActual();
    char texto[11];
    std::strftime(texto, sizeof(texto), "%Y-%m-%d", &fecha);
    return texto;
}

std::string obtenerHoraActual() {
    const std::tm hora = horaLocalActual();
    char texto[9];
    std::strftime(texto, sizeof(texto), "%H:%M:%S", &hora);
    return texto;
}

int obtenerMinutosActuales() {
    const std::tm hora = horaLocalActual();
    return hora.tm_hour * 60 + hora.tm_min;
}

int buscarFranjaDeIngreso(int minutosActuales) {
    int cantidad = 0;
    const FranjaHoraria* franjas = FranjaHoraria::obtenerTodas(cantidad);
    for (int i = 0; i < cantidad; ++i) {
        if (minutosActuales >= franjas[i].getInicioMinutos() - 10
            && minutosActuales <= franjas[i].getInicioMinutos() + 10) {
            return i;
        }
    }
    return -1;
}

int buscarFranjaDeIngresoSegundos(int segundosDelDia) {
    int cantidad = 0;
    const FranjaHoraria* franjas = FranjaHoraria::obtenerTodas(cantidad);
    for (int i = 0; i < cantidad; ++i) {
        const int inicioSegundos = franjas[i].getInicioMinutos() * 60;
        if (segundosDelDia >= inicioSegundos - 10 * 60
            && segundosDelDia <= inicioSegundos + 10 * 60) {
            return i;
        }
    }
    return -1;
}

bool estaEnVentanaDeSalida(int segundosDelDia, const FranjaHoraria& franja) {
    const int finSegundos = franja.getFinMinutos() * 60;
    return segundosDelDia >= finSegundos - 10 * 60
        && segundosDelDia <= finSegundos;
}
