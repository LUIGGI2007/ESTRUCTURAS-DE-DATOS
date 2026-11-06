#ifndef VALIDADORCEDULA_H
#define VALIDADORCEDULA_H

#include <cstddef>
#include <string>

class ValidadorCedula
{
public:
    bool validarCedula(const std::string& cedula) const;
    static bool validarNombres(const std::string& texto);
    static bool validarCorreo(const std::string& correo,
                              const std::string& dominio = "");
    static bool validarHora(const std::string& hora);
    static bool validarHoraPersistida(const std::string& hora);
    static bool validarHoraCompleta(const std::string& hora);
    static bool validarHoraCampo(const std::string& hora);
    static bool validarMinutoSegundo(const std::string& valor);
    static bool validarFecha(const std::string& fecha);
    static bool validarFechaIso(const std::string& fecha);
    static bool validarOpcion(const std::string& opcion);
    static bool validarOpcionMenu(const std::string& texto,
                                  const std::string& patron);
    static bool validarCodigoPeriodo(const std::string& texto);
    static bool validarDescripcionPeriodo(const std::string& texto);
    static bool validarCodigoNRC(const std::string& texto);
    static bool validarMateria(const std::string& texto);
    static bool validarDia(const std::string& texto);
    static bool validarDiaLaborable(const std::string& texto);
    static bool validarBloqueHorario(const std::string& inicioFin);
    static bool validarId(const std::string& texto);
    static bool validarUsuarioInstitucional(const std::string& texto);
    static bool validarUsuarioOCancelar(const std::string& texto);
    static bool esCancelar(const std::string& texto);
    static bool validarSeleccionLista(const std::string& texto,
                                      std::size_t cantidad);
    static bool validarContrasenia(const std::string& texto);
    static bool validarOpcionSiNo(const std::string& texto);
    static bool validarCampo(const std::string& texto,
                             const std::string& patron);
    static std::string recortarEspacios(const std::string& texto);
    static bool validarLongitudCampo(const std::string& texto,
                                     const std::string& patron);
    static bool validarTecla(const std::string& tecla,
                             const std::string& patron);
    static bool esTeclaControl(const std::string& tecla);
    static int bytesUtf8(const std::string& byteInicial);
};

#endif
