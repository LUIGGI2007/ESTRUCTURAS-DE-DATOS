#include "IngresoDatos.h"
#include "ValidadorCedula.h"

#include <iostream>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <csignal>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#endif

namespace
{
#ifdef _WIN32
BOOL WINAPI ignorarControlConsola(DWORD evento)
{
    return evento == CTRL_C_EVENT || evento == CTRL_BREAK_EVENT ||
                   evento == CTRL_CLOSE_EVENT || evento == CTRL_LOGOFF_EVENT ||
                   evento == CTRL_SHUTDOWN_EVENT
        ? TRUE : FALSE;
}
#endif

void borrarUltimoCaracterUtf8(std::string& texto)
{
    if (texto.empty())
        return;
    texto.pop_back();
    while (!texto.empty() &&
           (static_cast<unsigned char>(texto.back()) & 0xC0) == 0x80)
        texto.pop_back();
}

#ifdef _WIN32
std::string codificarUtf8(wchar_t codigo)
{
    char utf8[4] = {};
    const int longitud = WideCharToMultiByte(
        CP_UTF8, 0, &codigo, 1, utf8, sizeof(utf8), nullptr, nullptr);
    return longitud > 0 ? std::string(utf8, static_cast<std::size_t>(longitud)) :
                          std::string();
}
#endif

bool teclaPermitida(const std::string& tecla, const std::string& patron)
{
    return !ValidadorCedula::esTeclaControl(tecla) &&
           ValidadorCedula::validarTecla(tecla, patron);
}

std::string leerLinea(const std::string& patron,
                      const std::string& patronTecla,
                      bool ocultar,
                      bool opcional)
{
    while (true)
    {
        std::string valor;
#ifdef _WIN32
        HANDLE consola = GetStdHandle(STD_INPUT_HANDLE);
        DWORD modoAnterior = 0;
        const bool modoDisponible =
            consola != INVALID_HANDLE_VALUE && GetConsoleMode(consola, &modoAnterior);
        if (modoDisponible)
            SetConsoleMode(consola, modoAnterior & ~ENABLE_PROCESSED_INPUT);
        while (true)
        {
            const wchar_t tecla = _getwch();
            if (tecla == L'\r' || tecla == L'\n')
            {
                std::cout << '\n';
                break;
            }
            if (tecla == 0 || tecla == 0xE0)
            {
                static_cast<void>(_getwch());
                continue;
            }
            if (tecla == L'\b')
            {
                if (!valor.empty())
                {
                    borrarUltimoCaracterUtf8(valor);
                    std::cout << "\b \b" << std::flush;
                }
                continue;
            }
            std::string caracter = codificarUtf8(tecla);
            if (teclaPermitida(caracter, patronTecla) &&
                ValidadorCedula::validarLongitudCampo(valor + caracter, patron))
            {
                valor += caracter;
                std::cout << (ocultar ? "*" : caracter) << std::flush;
            }
        }
        if (modoDisponible)
            SetConsoleMode(consola, modoAnterior);
#else
        termios anterior = {};
        if (tcgetattr(STDIN_FILENO, &anterior) != 0)
            throw std::runtime_error("No se pudo configurar la entrada de teclado");
        termios temporal = anterior;
        temporal.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO | ISIG));
        if (tcsetattr(STDIN_FILENO, TCSANOW, &temporal) != 0)
            throw std::runtime_error("No se pudo configurar la entrada de teclado");
        while (true)
        {
            char byte = 0;
            if (read(STDIN_FILENO, &byte, 1) != 1)
            {
                tcsetattr(STDIN_FILENO, TCSANOW, &anterior);
                throw std::runtime_error("No se pudo leer del terminal");
            }
            if (byte == '\r' || byte == '\n')
            {
                std::cout << '\n';
                break;
            }
            if (byte == 8 || byte == 127)
            {
                if (!valor.empty())
                {
                    borrarUltimoCaracterUtf8(valor);
                    std::cout << "\b \b" << std::flush;
                }
                continue;
            }
            if (byte == 27)
            {
                fd_set lectura;
                FD_ZERO(&lectura);
                FD_SET(STDIN_FILENO, &lectura);
                timeval espera = {0, 10000};
                if (select(STDIN_FILENO + 1, &lectura, nullptr, nullptr, &espera) > 0)
                {
                    char secuencia = 0;
                    while (read(STDIN_FILENO, &secuencia, 1) == 1 &&
                           ValidadorCedula::validarCampo(
                               std::string(1, secuencia), "^[A-Za-z~]$") == false)
                    {
                    }
                }
                continue;
            }
            std::string caracter(1, byte);
            const int cantidad = ValidadorCedula::bytesUtf8(caracter);
            if (cantidad > 1)
            {
                for (int indice = 1; indice < cantidad; ++indice)
                {
                    char continuacion = 0;
                    if (read(STDIN_FILENO, &continuacion, 1) != 1)
                        break;
                    caracter.push_back(continuacion);
                }
            }
            if (teclaPermitida(caracter, patronTecla) &&
                ValidadorCedula::validarLongitudCampo(valor + caracter, patron))
            {
                valor += caracter;
                std::cout << (ocultar ? "*" : caracter) << std::flush;
            }
        }
        tcsetattr(STDIN_FILENO, TCSANOW, &anterior);
#endif
        if ((opcional && valor.empty()) ||
            ValidadorCedula::validarCampo(valor, patron))
            return valor;
        std::cout << "Entrada no valida. Intente nuevamente: " << std::flush;
    }
}
}

void IngresoDatos::instalarProteccionesTerminal()
{
#ifdef _WIN32
    SetConsoleCtrlHandler(ignorarControlConsola, TRUE);
    SetConsoleOutputCP(CP_UTF8);
#else
    std::signal(SIGINT, SIG_IGN);
    std::signal(SIGTSTP, SIG_IGN);
    std::signal(SIGQUIT, SIG_IGN);
#endif
}

std::string IngresoDatos::leerSoloNumeros() const
{
    return leerTextoLibre("", "^[0-9]{1,10}$", "^[0-9]$");
}

std::string IngresoDatos::leerSoloNumeros(const std::string& etiqueta,
                                          const std::string& patron) const
{
    return leerTextoLibre(etiqueta, patron, "^[0-9]$");
}

std::string IngresoDatos::leerSoloLetras() const
{
    return leerTextoLibre("", "^[a-zA-ZáéíóúÁÉÍÓÚñÑ\\s]{1,40}$",
                          "^[a-zA-ZáéíóúÁÉÍÓÚñÑ ]$");
}

std::string IngresoDatos::leerCorreo(const std::string& dominio) const
{
    while (true)
    {
        const std::string correo = leerTextoLibre(
            "", "^[A-Za-z0-9._%+@-]{1,60}$", "^[A-Za-z0-9._%+@-]$");
        if (validarCorreoInstitucional(correo, dominio))
            return correo;
        std::cout << "Correo institucional invalido. Intente nuevamente: ";
    }
}

std::string IngresoDatos::leerHora() const
{
    return leerTextoLibre("", "^([01][0-9]|2[0-3]):[0-5][0-9]$",
                          "^[0-9:]$");
}

std::string IngresoDatos::leerTexto(const std::string& etiqueta) const
{
    return leerTextoLibre(etiqueta, "^[^|\\r\\n]{1,120}$",
                          "^[^|\\r\\n]$");
}

std::string IngresoDatos::leerContrasenia(const std::string& etiqueta) const
{
    return leerTextoLibre(etiqueta, "^[^|\\s]{4,20}$",
                          "^[^|\\s]$", true);
}

std::string IngresoDatos::leerContraseniaOCancelar(
    const std::string& etiqueta) const
{
    return leerTextoLibre(etiqueta, "^(0|[^|\\s]{4,20})$",
                          "^[^|\\s]$", true);
}

std::string IngresoDatos::leerCodigo(const std::string& etiqueta) const
{
    return leerTextoLibre(etiqueta, "^[A-Za-z0-9]{1,10}$",
                          "^[A-Za-z0-9]$");
}

std::string IngresoDatos::leerTextoLibre(const std::string& etiqueta,
                                         const std::string& patron,
                                         const std::string& patronTecla,
                                         bool ocultar,
                                         bool opcional) const
{
    if (!etiqueta.empty())
        std::cout << etiqueta;
    return leerLinea(patron, patronTecla, ocultar, opcional);
}

std::string IngresoDatos::leerCampo(const std::string& etiqueta,
                                    const std::string& patron,
                                    const std::string& patronTecla,
                                    bool ocultar,
                                    bool opcional) const
{
    return leerTextoLibre(etiqueta, patron, patronTecla, ocultar, opcional);
}

bool IngresoDatos::validarNombre(const std::string& valor)
{
    return ValidadorCedula::validarNombres(valor);
}
bool IngresoDatos::validarCorreoInstitucional(const std::string& valor,
                                              const std::string& dominio)
{
    return ValidadorCedula::validarCorreo(valor, dominio);
}
bool IngresoDatos::validarHoraFormato(const std::string& valor)
{
    return ValidadorCedula::validarHora(valor);
}
