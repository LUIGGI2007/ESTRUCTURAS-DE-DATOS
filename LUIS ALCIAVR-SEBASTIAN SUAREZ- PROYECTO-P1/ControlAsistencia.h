#ifndef CONTROLASISTENCIA_H
#define CONTROLASISTENCIA_H

#include "Docente.h"
#include "FranjaHoraria.h"
#include "ListasDoble_T.h"
#include "NRC.h"
#include "Periodo.h"
#include "RegistroAsistencia.h"

#include <string>

class ControlAsistencia
{
public:
    explicit ControlAsistencia(const std::string& directorioDatos = ".");
    ~ControlAsistencia() = default;
    ControlAsistencia(const ControlAsistencia&) = delete;
    ControlAsistencia& operator=(const ControlAsistencia&) = delete;

    const std::string& getDominioInstitucional() const;
    std::string correoSugerido(const Docente& docente) const;
    bool insertarDocente(const Docente& docente, std::string& mensaje);
    bool registrarDocente(const Docente& docente, std::string& mensaje);
    Docente* buscarDocente(const std::string& cedulaOCorreo);
    const Docente* buscarDocente(const std::string& cedulaOCorreo) const;
    bool eliminarDocente(const std::string& cedula, std::string& mensaje);
    void imprimirDocente(const std::string& cedulaOCorreo) const;
    Docente* iniciarSesion(const std::string& usuario,
                           const std::string& contrasenia);
    bool agregarPeriodo(const Periodo& periodo, std::string& mensaje);
    bool insertarPeriodo(const Periodo& periodo, std::string& mensaje);
    NRC* buscarNRC(const std::string& codigo);
    bool eliminarNRC(const std::string& codigo, std::string& mensaje);
    void imprimirNRC(const std::string& codigo) const;
    Periodo* buscarPeriodo(const std::string& codigo);
    bool eliminarPeriodo(const std::string& codigo, std::string& mensaje);
    void imprimirPeriodo(const std::string& codigo) const;
    bool asignarNRC(const NRC& nrc, std::string& mensaje);
    bool insertarNRC(const NRC& nrc, std::string& mensaje);
    bool asignarFranja(const FranjaHoraria& franja, std::string& mensaje);
    bool insertarFranja(const FranjaHoraria& franja, std::string& mensaje);
    FranjaHoraria* buscarFranja(int id);
    bool eliminarFranja(int id, std::string& mensaje);
    void imprimirFranja(int id) const;
    bool registrarEntrada(const std::string& cedula, const std::string& codigoNRC,
                          int idFranja, const Fecha& fecha, const Hora& horaActual,
                          std::string& mensaje);
    bool registrarSalida(const std::string& cedula, const std::string& codigoNRC,
                         int idFranja, const Fecha& fecha, const Hora& horaActual,
                         std::string& mensaje);
    bool corregirEntrada(const std::string& idRegistro, const std::string& cedula,
                         const Hora& nuevaHora, const Hora& horaActual,
                         std::string& mensaje);
    bool corregirSalida(const std::string& idRegistro, const std::string& cedula,
                        const Hora& nuevaHora, const Hora& horaActual,
                        std::string& mensaje);
    bool eliminarRegistro(const std::string& idRegistro, std::string& mensaje);
    bool insertarRegistro(const RegistroAsistencia& registro,
                          std::string& mensaje);
    RegistroAsistencia* buscarRegistro(const std::string& id);

    void imprimirHorarios(const std::string& cedula) const;
    void imprimirHistorial(const std::string& cedula = "") const;
    void imprimirHistorialPorFecha(const std::string& cedula,
                                   const Fecha& fecha) const;
    void imprimirAsistenciasPorFecha(const Fecha& fecha) const;
    void imprimirRegistro(const std::string& id) const;
    void imprimirRegistros() const;
    void imprimirDocentes() const;
    void imprimirNRC() const;
    void imprimirNRCs() const;
    void imprimirPeriodos() const;
    void imprimirFranjas() const;

    ListasDoble_T<Docente>& docentes();
    ListasDoble_T<NRC>& cursos();
    ListasDoble_T<Periodo>& periodos();
    ListasDoble_T<FranjaHoraria>& franjas();
    ListasDoble_T<RegistroAsistencia>& registros();

private:
    void cargarDatos();
    bool guardarDocentes() const;
    bool guardarNRC() const;
    bool guardarPeriodos() const;
    bool guardarFranjas() const;
    bool guardarAsistencias() const;
    const NRC* buscarNRC(const std::string& codigo) const;
    const Periodo* buscarPeriodo(const std::string& codigo) const;
    const FranjaHoraria* buscarFranja(int id) const;
    bool asignacionPertenece(const std::string& cedula,
                             const FranjaHoraria& franja) const;
    std::string crearIdRegistro(const std::string& cedula, const Fecha& fecha,
                                const std::string& nrc, int franja) const;

    std::string directorioDatos;
    std::string dominioInstitucional;
    ListasDoble_T<Docente> listaDocentes;
    ListasDoble_T<NRC> listaNRC;
    ListasDoble_T<Periodo> listaPeriodos;
    ListasDoble_T<FranjaHoraria> listaFranjas;
    ListasDoble_T<RegistroAsistencia> listaAsistencias;
};

#endif
