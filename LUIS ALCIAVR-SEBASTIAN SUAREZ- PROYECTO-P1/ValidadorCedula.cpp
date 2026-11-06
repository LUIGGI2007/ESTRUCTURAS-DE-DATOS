#include "ValidadorCedula.h"

#include <codecvt>
#include <locale>
#include <regex>
#include <stdexcept>

bool ValidadorCedula::validarCedula(const std::string& cedula) const
{
    static const std::regex formatoProvincia(
        "^(0[1-9]|1[0-9]|2[0-4]|30)[0-9]{8}$");
    static const std::regex formatoTercerDigito("^[0-9]{2}[0-5][0-9]{7}$");
    if (!std::regex_match(cedula, formatoProvincia) ||
        !std::regex_match(cedula, formatoTercerDigito))
        return false;

    int suma = 0;
    for (int i = 0; i < 9; ++i)
    {
        int valor = std::stoi(cedula.substr(static_cast<std::size_t>(i), 1));
        if (i % 2 == 0)
        {
            valor *= 2;
            if (valor > 9)
                valor -= 9;
        }
        suma += valor;
    }
    return (10 - suma % 10) % 10 == std::stoi(cedula.substr(9, 1));
}

bool ValidadorCedula::validarNombres(const std::string& texto)
{
    static const std::wregex formato(
        L"^[a-zA-ZáéíóúÁÉÍÓÚñÑ\\s]{1,40}$");
    static const std::wregex contieneLetra(
        L"[a-zA-ZáéíóúÁÉÍÓÚñÑ]");
    try
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conversor;
        const std::wstring nombre = conversor.from_bytes(texto);
        return std::regex_match(nombre, formato) &&
               std::regex_search(nombre, contieneLetra);
    }
    catch (const std::range_error&)
    {
        return false;
    }
}

bool ValidadorCedula::validarCorreo(const std::string& correo,
                                    const std::string& dominio)
{
    static const std::regex formato(
        "^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
    if (!std::regex_match(correo, formato))
        return false;
    if (dominio.empty())
        return true;
    const std::size_t arroba = correo.find('@');
    return arroba != std::string::npos &&
           correo.substr(arroba + 1) == dominio;
}

bool ValidadorCedula::validarHora(const std::string& hora)
{
    static const std::regex formato("^([01][0-9]|2[0-3]):[0-5][0-9]$");
    return std::regex_match(hora, formato);
}

bool ValidadorCedula::validarHoraPersistida(const std::string& hora)
{
    return validarHora(hora) || validarHoraCompleta(hora);
}

bool ValidadorCedula::validarHoraCompleta(const std::string& hora)
{
    static const std::regex formato(
        "^([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9]$");
    return std::regex_match(hora, formato);
}

bool ValidadorCedula::validarHoraCampo(const std::string& hora)
{
    static const std::regex formato("^([01][0-9]|2[0-3])$");
    return std::regex_match(hora, formato);
}

bool ValidadorCedula::validarMinutoSegundo(const std::string& valor)
{
    static const std::regex formato("^[0-5][0-9]$");
    return std::regex_match(valor, formato);
}

bool ValidadorCedula::validarFecha(const std::string& fecha)
{
    const std::string patronAnio =
        "([0-9]{3}[1-9]|[0-9]{2}[1-9][0-9]|[0-9][1-9][0-9]{2}|[1-9][0-9]{3})";
    static const std::regex formato(
        "^(0[1-9]|[12][0-9]|3[01])/(0[1-9]|1[0-2])/" + patronAnio + "$");
    const std::regex mesTreinta(
        "^(0[1-9]|[12][0-9]|30)/(04|06|09|11)/" + patronAnio + "$");
    const std::regex mesTreintaYUno(
        "^(0[1-9]|[12][0-9]|3[01])/(01|03|05|07|08|10|12)/" +
        patronAnio + "$");
    const std::regex febreroHasta28(
        "^(0[1-9]|1[0-9]|2[0-8])/02/" + patronAnio + "$");
    const std::regex febrero29("^29/02/(" + patronAnio + ")$");
    std::smatch partes;
    if (std::regex_match(fecha, mesTreinta) ||
        std::regex_match(fecha, mesTreintaYUno) ||
        std::regex_match(fecha, febreroHasta28))
        return true;
    if (!std::regex_match(fecha, formato) ||
        !std::regex_match(fecha, partes, febrero29))
        return false;
    const int anioNumerico = std::stoi(partes[1]);
    return (anioNumerico % 4 == 0 && anioNumerico % 100 != 0) ||
           anioNumerico % 400 == 0;
}

bool ValidadorCedula::validarFechaIso(const std::string& fecha)
{
    static const std::regex formato(
        "^([0-9]{3}[1-9]|[0-9]{2}[1-9][0-9]|[0-9][1-9][0-9]{2}|[1-9][0-9]{3})-"
        "(0[1-9]|1[0-2])-(0[1-9]|[12][0-9]|3[01])$");
    if (!std::regex_match(fecha, formato))
        return false;
    return validarFecha(fecha.substr(8, 2) + "/" + fecha.substr(5, 2) + "/" +
                        fecha.substr(0, 4));
}

bool ValidadorCedula::validarOpcion(const std::string& opcion)
{
    static const std::regex formato("^[0-9]+$");
    return std::regex_match(opcion, formato);
}

bool ValidadorCedula::validarOpcionMenu(const std::string& texto,
                                        const std::string& patron)
{
    return validarCampo(texto, patron);
}

bool ValidadorCedula::validarCodigoPeriodo(const std::string& texto)
{
    static const std::regex formato("^[A-Za-z0-9-]{3,15}$");
    return std::regex_match(texto, formato);
}

bool ValidadorCedula::validarDescripcionPeriodo(const std::string& texto)
{
    static const std::regex formato("^[0-9]{4}-[1-9]$");
    return std::regex_match(texto, formato);
}

bool ValidadorCedula::validarCodigoNRC(const std::string& texto)
{
    static const std::regex formato("^[0-9]{4,6}$");
    return std::regex_match(texto, formato);
}

bool ValidadorCedula::validarMateria(const std::string& texto)
{
    static const std::wregex formato(
        L"^[A-Za-zÁÉÍÓÚáéíóúÑñÜü0-9 ]{3,60}$");
    try
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conversor;
        return std::regex_match(conversor.from_bytes(texto), formato);
    }
    catch (const std::range_error&)
    {
        return false;
    }
}

bool ValidadorCedula::validarDia(const std::string& texto)
{
    return validarDiaLaborable(texto);
}

bool ValidadorCedula::validarDiaLaborable(const std::string& texto)
{
    static const std::wregex formato(
        L"^(lunes|martes|miercoles|miércoles|jueves|viernes)$",
        std::regex_constants::icase);
    static const std::wregex mayusculas[] = {
        std::wregex(L"Á"), std::wregex(L"É"), std::wregex(L"Í"),
        std::wregex(L"Ó"), std::wregex(L"Ú")};
    static const std::wstring minusculas[] = {L"á", L"é", L"í", L"ó", L"ú"};
    try
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conversor;
        std::wstring normalizado = conversor.from_bytes(texto);
        for (std::size_t indice = 0; indice < 5; ++indice)
            normalizado = std::regex_replace(
                normalizado, mayusculas[indice], minusculas[indice]);
        return std::regex_match(normalizado, formato);
    }
    catch (const std::range_error&)
    {
        return false;
    }
}

bool ValidadorCedula::validarBloqueHorario(const std::string& inicioFin)
{
    static const std::regex formato(
        "^(07:00-09:00|09:00-11:00|11:00-13:00|13:00-15:00|"
        "15:00-17:00|17:00-19:00|19:00-21:00)$");
    return std::regex_match(inicioFin, formato);
}

bool ValidadorCedula::validarId(const std::string& texto)
{
    static const std::regex formato("^[1-9][0-9]{0,8}$");
    return std::regex_match(texto, formato);
}

bool ValidadorCedula::validarUsuarioInstitucional(const std::string& texto)
{
    static const std::regex formato(
        "^([0-9]{10}|[A-Za-z0-9._%+-]+@espe\\.edu\\.ec)$");
    return std::regex_match(texto, formato);
}

bool ValidadorCedula::validarUsuarioOCancelar(const std::string& texto)
{
    static const std::regex formato(
        "^(0|[0-9]{10}|[A-Za-z0-9._%+-]+@espe\\.edu\\.ec)$");
    return std::regex_match(texto, formato);
}

bool ValidadorCedula::esCancelar(const std::string& texto)
{
    static const std::regex formato("^0$");
    return std::regex_match(texto, formato);
}

bool ValidadorCedula::validarSeleccionLista(const std::string& texto,
                                           std::size_t cantidad)
{
    static const std::regex formato("^(0|[1-9][0-9]?)$");
    if (!std::regex_match(texto, formato))
        return false;
    if (esCancelar(texto))
        return true;
    return static_cast<std::size_t>(std::stoi(texto)) <= cantidad;
}

bool ValidadorCedula::validarContrasenia(const std::string& texto)
{
    static const std::regex formato("^[^|\\s]{4,20}$");
    return std::regex_match(texto, formato);
}

bool ValidadorCedula::validarOpcionSiNo(const std::string& texto)
{
    static const std::regex formato("^[SsNn]$");
    return std::regex_match(texto, formato);
}

bool ValidadorCedula::validarCampo(const std::string& texto,
                                   const std::string& patron)
{
    if (patron ==
        "^(lunes|martes|miercoles|miércoles|jueves|viernes|sabado|sábado|domingo)$")
        return validarDia(texto);
    try
    {
        if (std::regex_match(texto, std::regex(patron)))
            return true;
    }
    catch (const std::regex_error&)
    {
    }
    try
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conversor;
        return std::regex_match(conversor.from_bytes(texto),
                                std::wregex(conversor.from_bytes(patron)));
    }
    catch (const std::range_error&)
    {
        return false;
    }
    catch (const std::regex_error&)
    {
        return false;
    }
}

std::string ValidadorCedula::recortarEspacios(const std::string& texto)
{
    static const std::regex iniciales("^[ \\t]+");
    static const std::regex finales("[ \\t]+$");
    return std::regex_replace(std::regex_replace(texto, iniciales, ""), finales, "");
}

bool ValidadorCedula::validarTecla(const std::string& tecla,
                                   const std::string& patron)
{
    if (validarCampo(tecla, patron))
        return true;
    try
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conversor;
        const std::wstring teclaAncha = conversor.from_bytes(tecla);
        const std::wstring patronAncho = conversor.from_bytes(patron);
        return std::regex_match(teclaAncha, std::wregex(patronAncho));
    }
    catch (const std::range_error&)
    {
        return false;
    }
    catch (const std::regex_error&)
    {
        return false;
    }
}

bool ValidadorCedula::validarLongitudCampo(const std::string& texto,
                                           const std::string& patron)
{
    if (patron ==
        "^(0|[0-9]{10}|[A-Za-z0-9._%+-]+@espe\\.edu\\.ec)$")
        return validarCampo(texto, "^.{0,60}$");
    if (patron == "^(0|[1-9]|1[0-8])$")
        return validarCampo(texto, "^.{0,2}$");
    if (patron == "^[0-3]$" || patron == "^[0-7]$")
        return validarCampo(texto, "^.{0,1}$");
    if (patron ==
        "^(0|[0-9]{10}-[0-9]{8}-[0-9]{4,6}-[1-9][0-9]{0,8})$")
        return validarCampo(texto, "^.{0,60}$");
    if (patron == "^([01][0-9]|2[0-3])$" ||
        patron == "^[0-5][0-9]$")
        return validarCampo(texto, "^.{0,2}$");
    if (patron == "^[0-9]{4}-[1-9]$")
        return validarCampo(texto, "^.{0,6}$");
    if (patron == "^[1-9][0-9]{0,8}$")
        return validarCampo(texto, "^.{0,9}$");
    if (patron ==
        "^(0[1-9]|[12][0-9]|3[01])/(0[1-9]|1[0-2])/[0-9]{4}$")
        return validarCampo(texto, "^.{0,10}$");
    if (patron ==
        "^(lunes|martes|miercoles|miércoles|jueves|viernes|sabado|sábado|domingo)$")
        return validarCampo(texto, "^.{0,10}$");
    if (patron == "^([01][0-9]|2[0-3]):[0-5][0-9]$")
        return validarCampo(texto, "^.{0,5}$");
    static const std::regex longitudPatron(
        "\\{([0-9]+)(,([0-9]+))?\\}");
    std::smatch coincidencia;
    if (!std::regex_search(patron, coincidencia, longitudPatron))
        return validarCampo(texto, "^.{0,512}$");
    const std::string maximo = coincidencia[3].matched
        ? coincidencia[3].str() : coincidencia[1].str();
    return validarCampo(texto, "^.{0," + maximo + "}$");
}

bool ValidadorCedula::esTeclaControl(const std::string& tecla)
{
    static const std::regex controles(
        "^[\\x00-\\x08\\x0B\\x0C\\x0E-\\x1F\\x7F]$");
    return std::regex_match(tecla, controles);
}

int ValidadorCedula::bytesUtf8(const std::string& byteInicial)
{
    static const std::regex dosBytes("^[\\xC2-\\xDF]$");
    static const std::regex tresBytes("^[\\xE0-\\xEF]$");
    static const std::regex cuatroBytes("^[\\xF0-\\xF4]$");
    if (std::regex_match(byteInicial, dosBytes))
        return 2;
    if (std::regex_match(byteInicial, tresBytes))
        return 3;
    if (std::regex_match(byteInicial, cuatroBytes))
        return 4;
    return 1;
}
