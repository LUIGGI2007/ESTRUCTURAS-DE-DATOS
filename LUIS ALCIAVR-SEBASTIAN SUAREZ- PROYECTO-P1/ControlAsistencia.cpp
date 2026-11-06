#include "ControlAsistencia.h"
#include "ValidadorCedula.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cctype>
#include <utility>

namespace
{
std::size_t separar(const std::string& linea, std::string campos[],
                    std::size_t capacidad)
{
    std::size_t cantidad = 0;
    std::size_t inicio = 0;
    while (inicio <= linea.size())
    {
        if (cantidad == capacidad)
            return capacidad + 1;
        const std::size_t separador = linea.find('|', inicio);
        campos[cantidad++] = linea.substr(
            inicio, separador == std::string::npos ? std::string::npos :
                                                     separador - inicio);
        if (!campos[cantidad - 1].empty() &&
            campos[cantidad - 1].back() == '\r')
            campos[cantidad - 1].pop_back();
        if (separador == std::string::npos)
            break;
        inicio = separador + 1;
    }
    return cantidad;
}

std::string rutaArchivo(const std::string& directorio, const std::string& nombre)
{
    return (std::filesystem::path(directorio) / nombre).string();
}

std::string diaDeLaFecha(const Fecha& fecha)
{
    std::tm valor = {};
    valor.tm_mday = fecha.getDia();
    valor.tm_mon = fecha.getMes() - 1;
    valor.tm_year = fecha.getAnio() - 1900;
    valor.tm_isdst = -1;
    if (std::mktime(&valor) == static_cast<std::time_t>(-1))
        return "";
    static const char* dias[] = {
        "domingo", "lunes", "martes", "miercoles", "jueves", "viernes", "sabado"};
    return dias[valor.tm_wday];
}

std::string estadoRegistro(const RegistroAsistencia& registro)
{
    if (!registro.tieneEntrada())
        return "SIN ENTRADA";
    if (!registro.tieneSalida())
        return "PUNTUAL - SALIDA PENDIENTE";
    return "PRESENTE";
}

std::string normalizarNombreCorreo(std::string texto)
{
    const std::pair<const char*, const char*> reemplazos[] = {
        {"á", "a"}, {"é", "e"}, {"í", "i"}, {"ó", "o"}, {"ú", "u"},
        {"Á", "a"}, {"É", "e"}, {"Í", "i"}, {"Ó", "o"}, {"Ú", "u"},
        {"ñ", "n"}, {"Ñ", "n"}};
    for (const auto& reemplazo : reemplazos)
    {
        std::size_t posicion = texto.find(reemplazo.first);
        while (posicion != std::string::npos)
        {
            texto.replace(posicion, std::char_traits<char>::length(reemplazo.first),
                          reemplazo.second);
            posicion = texto.find(reemplazo.first, posicion + 1);
        }
    }
    for (char& caracter : texto)
        caracter = static_cast<char>(
            std::tolower(static_cast<unsigned char>(caracter)));
    return texto;
}

std::string crearCorreoBase(const Docente& docente)
{
    const std::string nombres = normalizarNombreCorreo(docente.getNombre());
    const std::string apellidos = normalizarNombreCorreo(docente.getApellido());
    const std::size_t inicioPrimerNombre = nombres.find_first_not_of(' ');
    const std::size_t finPrimerNombre = nombres.find(' ', inicioPrimerNombre);
    const std::size_t inicioPrimerApellido = apellidos.find_first_not_of(' ');
    const std::size_t finPrimerApellido = apellidos.find(' ', inicioPrimerApellido);
    const std::string primerNombre = nombres.substr(
        inicioPrimerNombre,
        finPrimerNombre == std::string::npos ? std::string::npos :
                                               finPrimerNombre - inicioPrimerNombre);
    const std::string primerApellido = apellidos.substr(
        inicioPrimerApellido,
        finPrimerApellido == std::string::npos ? std::string::npos :
                                                 finPrimerApellido - inicioPrimerApellido);
    return primerNombre.substr(0, 1) + primerApellido + "@espe.edu.ec";
}

std::string quitarEspaciosExtremos(const std::string& texto)
{
    return ValidadorCedula::recortarEspacios(texto);
}

std::string correoDisponible(const Docente& docente,
                             const ListasDoble_T<Docente>& docentes)
{
    const std::string base = crearCorreoBase(docente);
    std::string correo = base;
    unsigned int sufijo = 1;
    bool ocupado = true;
    while (ocupado)
    {
        ocupado = false;
        docentes.recorrer([&](const Docente& actual) {
            if (actual.getCorreo() == correo)
                ocupado = true;
        });
        if (ocupado)
        {
            const std::size_t arroba = base.find('@');
            correo = base.substr(0, arroba) + std::to_string(sufijo++) +
                     base.substr(arroba);
        }
    }
    return correo;
}

void asegurarArchivo(const std::string& ruta)
{
    std::error_code error;
    if (std::filesystem::exists(ruta, error))
        return;
    if (error)
        throw std::runtime_error("No se pudo consultar el archivo: " + ruta);
    std::fstream nuevo(ruta, std::ios::out);
    if (!nuevo)
        throw std::runtime_error("No se pudo crear el archivo: " + ruta);
}
}

ControlAsistencia::ControlAsistencia(const std::string& directorio)
    : directorioDatos(directorio), dominioInstitucional("espe.edu.ec")
{
    std::filesystem::create_directories(directorioDatos);
    const char* archivos[] = {"docentes.txt", "nrc.txt", "periodos.txt",
                              "franjas_horarias.txt", "asistencias.txt"};
    for (const char* archivo : archivos)
        asegurarArchivo(rutaArchivo(directorioDatos, archivo));
    cargarDatos();
}

const std::string& ControlAsistencia::getDominioInstitucional() const
{
    return dominioInstitucional;
}

std::string ControlAsistencia::correoSugerido(const Docente& docente) const
{
    return correoDisponible(docente, listaDocentes);
}

void ControlAsistencia::cargarDatos()
{
    bool correoDocenteMigrado = false;
    bool espaciosDocenteRecortados = false;
    const auto leerLineas = [this](const std::string& nombre,
                                   const auto& procesar) {
        std::ifstream archivo(rutaArchivo(directorioDatos, nombre));
        if (!archivo)
            throw std::runtime_error("No se pudo leer " + nombre);
        bool archivoModificado = false;
        std::string linea;
        std::size_t numeroLinea = 0;
        while (std::getline(archivo, linea))
        {
            ++numeroLinea;
            if (!linea.empty() && linea.back() == '\r')
                linea.pop_back();
            if (linea.empty())
                continue;
            try
            {
                if (!procesar(linea))
                {
                    std::cerr << nombre << ", linea " << numeroLinea
                              << " ignorada: datos invalidos o referencia inexistente.\n";
                    archivoModificado = true;
                }
            }
            catch (const std::exception& error)
            {
                std::cerr << nombre << ", linea " << numeroLinea
                          << " ignorada: " << error.what() << '\n';
                archivoModificado = true;
            }
        }
        return archivoModificado;
    };

    bool docentesModificados = leerLineas(
        "docentes.txt", [this, &correoDocenteMigrado,
                         &espaciosDocenteRecortados](const std::string& linea) {
            std::string campos[8];
            if (separar(linea, campos, 8) != 5)
                return false;
            const std::string nombreOriginal = campos[1];
            const std::string apellidoOriginal = campos[2];
            campos[1] = quitarEspaciosExtremos(campos[1]);
            campos[2] = quitarEspaciosExtremos(campos[2]);
            if (campos[1] != nombreOriginal || campos[2] != apellidoOriginal)
                espaciosDocenteRecortados = true;
            ValidadorCedula validador;
            if (!validador.validarCedula(campos[0]) ||
                !ValidadorCedula::validarNombres(campos[1]) ||
                !ValidadorCedula::validarNombres(campos[2]) ||
                !ValidadorCedula::validarContrasenia(campos[4]) ||
                buscarDocente(campos[0]) != nullptr)
                return false;
            Docente docente(campos[0], campos[1], campos[2], campos[4], "");
            bool correoDuplicado = false;
            listaDocentes.recorrer([&](const Docente& existente) {
                if (existente.getCorreo() == campos[3])
                    correoDuplicado = true;
            });
            if (!ValidadorCedula::validarCorreo(campos[3], dominioInstitucional) ||
                correoDuplicado)
            {
                docente.setCorreo(correoDisponible(docente, listaDocentes));
                correoDocenteMigrado = true;
            }
            else
                docente.setCorreo(campos[3]);
            listaDocentes.insertar(docente);
            return true;
        });
    docentesModificados = docentesModificados || correoDocenteMigrado ||
                          espaciosDocenteRecortados;

    bool periodosModificados = leerLineas(
        "periodos.txt", [this](const std::string& linea) {
            std::string campos[8];
            if (separar(linea, campos, 8) != 2 ||
                !ValidadorCedula::validarCodigoPeriodo(campos[0]) ||
                !ValidadorCedula::validarDescripcionPeriodo(campos[1]) ||
                buscarPeriodo(campos[0]) != nullptr)
                return false;
            listaPeriodos.insertar(Periodo(campos[0], campos[1]));
            return true;
        });

    bool nrcModificados = leerLineas(
        "nrc.txt", [this](const std::string& linea) {
            std::string campos[8];
            if (separar(linea, campos, 8) != 4 ||
                !ValidadorCedula::validarCodigoNRC(campos[0]) ||
                !ValidadorCedula::validarMateria(campos[1]) ||
                buscarDocente(campos[2]) == nullptr ||
                buscarPeriodo(campos[3]) == nullptr ||
                buscarNRC(campos[0]) != nullptr)
                return false;
            listaNRC.insertar(NRC(campos[0], campos[1], campos[2], campos[3]));
            return true;
        });

    bool franjasModificadas = leerLineas(
        "franjas_horarias.txt", [this](const std::string& linea) {
            std::string campos[8];
            if (separar(linea, campos, 8) != 5)
                return false;
            const std::string bloque = campos[3] + "-" + campos[4];
            if (!ValidadorCedula::validarDiaLaborable(campos[2]))
                throw std::invalid_argument(
                    "Franja " + campos[0] +
                    " ignorada: día/horario no permitido. "
                    "DÍA NO LABORABLE: las clases solo se dictan de lunes a "
                    "viernes. No es posible asignar sábado ni domingo.");
            if (!ValidadorCedula::validarBloqueHorario(bloque))
                throw std::invalid_argument(
                    "Franja " + campos[0] +
                    " ignorada: día/horario no permitido. "
                    "HORARIO NO PERMITIDO: las clases solo pueden ser "
                    "07-09, 09-11, 11-13, 13-15, 15-17, 17-19 o 19-21.");
            Hora inicio;
            Hora fin;
            const NRC* nrc = buscarNRC(campos[1]);
            if (!ValidadorCedula::validarId(campos[0]) || nrc == nullptr ||
                !Hora::parsear(campos[3], inicio) ||
                !Hora::parsear(campos[4], fin))
                return false;
            const int id = std::stoi(campos[0]);
            if (buscarFranja(id) != nullptr)
                return false;
            listaFranjas.insertar(
                FranjaHoraria(id, campos[2], inicio, fin, campos[1]));
            return true;
        });

    bool asistenciasModificadas = leerLineas(
        "asistencias.txt", [this](const std::string& linea) {
            std::string campos[8];
            Fecha fecha;
            Hora entrada;
            Hora salida;
            if (separar(linea, campos, 8) != 7 ||
                !Fecha::parsear(campos[2], fecha) ||
                (!campos[5].empty() && !Hora::parsear(campos[5], entrada)) ||
                (!campos[6].empty() && !Hora::parsear(campos[6], salida)) ||
                !ValidadorCedula::validarCodigoNRC(campos[3]) ||
                !ValidadorCedula::validarId(campos[4]))
                return false;
            const int idFranja = std::stoi(campos[4]);
            const NRC* nrc = buscarNRC(campos[3]);
            const FranjaHoraria* franja = buscarFranja(idFranja);
            const std::string idEsperado =
                crearIdRegistro(campos[1], fecha, campos[3], idFranja);
            if (buscarDocente(campos[1]) == nullptr || nrc == nullptr ||
                franja == nullptr || nrc->getCedulaDocente() != campos[1] ||
                franja->getCodigoNRC() != campos[3] ||
                campos[0] != idEsperado ||
                buscarRegistro(campos[0]) != nullptr)
                return false;
            if ((!campos[5].empty() &&
                 (entrada.minutosDelDia() < franja->getHoraInicio().minutosDelDia() ||
                  entrada.minutosDelDia() >
                      franja->getHoraInicio().minutosDelDia() + 10)) ||
                (!campos[6].empty() &&
                 (campos[5].empty() ||
                  salida.minutosDelDia() <
                      franja->getHoraFin().minutosDelDia() - 10 ||
                  salida.minutosDelDia() >
                      franja->getHoraFin().minutosDelDia() ||
                  salida.minutosDelDia() < entrada.minutosDelDia())) ||
                diaDeLaFecha(fecha) != franja->getDiaSemana())
                return false;
            RegistroAsistencia registro(
                crearIdRegistro(campos[1], fecha, campos[3], idFranja),
                campos[1], fecha, campos[3], idFranja);
            if (!campos[5].empty())
                registro.setEntrada(entrada);
            if (!campos[6].empty())
                registro.setSalida(salida);
            listaAsistencias.insertar(registro);
            return true;
        });

    if ((docentesModificados && !guardarDocentes()) ||
        (periodosModificados && !guardarPeriodos()) ||
        (nrcModificados && !guardarNRC()) ||
        (franjasModificadas && !guardarFranjas()) ||
        (asistenciasModificadas && !guardarAsistencias()))
        throw std::runtime_error("No se pudieron limpiar los archivos TXT invalidos.");
}

bool ControlAsistencia::guardarDocentes() const
{
    std::fstream salida(rutaArchivo(directorioDatos, "docentes.txt"),
                        std::ios::out | std::ios::trunc);
    if (!salida)
        return false;
    listaDocentes.recorrer([&salida](const Docente& docente) {
        salida << docente.getCedula() << '|' << docente.getNombre() << '|'
               << docente.getApellido() << '|' << docente.getCorreo() << '|'
               << docente.getContrasenia() << '\n';
    });
    return static_cast<bool>(salida);
}

bool ControlAsistencia::guardarNRC() const
{
    std::fstream salida(rutaArchivo(directorioDatos, "nrc.txt"),
                        std::ios::out | std::ios::trunc);
    if (!salida)
        return false;
    listaNRC.recorrer([&salida](const NRC& nrc) {
        salida << nrc.getCodigo() << '|' << nrc.getNombre() << '|'
               << nrc.getCedulaDocente() << '|' << nrc.getCodigoPeriodo() << '\n';
    });
    return static_cast<bool>(salida);
}

bool ControlAsistencia::guardarPeriodos() const
{
    std::fstream salida(rutaArchivo(directorioDatos, "periodos.txt"),
                        std::ios::out | std::ios::trunc);
    if (!salida)
        return false;
    listaPeriodos.recorrer([&salida](const Periodo& periodo) {
        salida << periodo.getCodigo() << '|' << periodo.getDescripcion() << '\n';
    });
    return static_cast<bool>(salida);
}

bool ControlAsistencia::guardarFranjas() const
{
    std::fstream salida(rutaArchivo(directorioDatos, "franjas_horarias.txt"),
                        std::ios::out | std::ios::trunc);
    if (!salida)
        return false;
    listaFranjas.recorrer([&salida](const FranjaHoraria& franja) {
        salida << franja.getId() << '|' << franja.getCodigoNRC() << '|'
               << franja.getDiaSemana() << '|'
               << franja.getHoraInicio().toStringHoraMinuto() << '|'
               << franja.getHoraFin().toStringHoraMinuto() << '\n';
    });
    return static_cast<bool>(salida);
}

bool ControlAsistencia::guardarAsistencias() const
{
    std::fstream salida(rutaArchivo(directorioDatos, "asistencias.txt"),
                        std::ios::out | std::ios::trunc);
    if (!salida)
        return false;
    listaAsistencias.recorrer([&salida](const RegistroAsistencia& registro) {
        salida << registro.getId() << '|' << registro.getCedulaDocente() << '|'
               << registro.getFecha().toString() << '|' << registro.getCodigoNRC()
               << '|' << registro.getIdFranja() << '|';
        if (registro.tieneEntrada())
            salida << registro.getEntrada().toString();
        salida << '|';
        if (registro.tieneSalida())
            salida << registro.getSalida().toString();
        salida << '\n';
    });
    return static_cast<bool>(salida);
}

Docente* ControlAsistencia::buscarDocente(const std::string& cedula)
{
    Nodo_T<Docente>* nodo = listaDocentes.buscar(
        [&cedula](const Docente& docente) {
            return docente.getCedula() == cedula ||
                   docente.getCorreo() == cedula;
        });
    return nodo == nullptr ? nullptr : &nodo->getDato();
}
const Docente* ControlAsistencia::buscarDocente(const std::string& cedula) const
{
    const Nodo_T<Docente>* nodo = listaDocentes.buscar(
        [&cedula](const Docente& docente) {
            return docente.getCedula() == cedula ||
                   docente.getCorreo() == cedula;
        });
    return nodo == nullptr ? nullptr : &nodo->getDato();
}
NRC* ControlAsistencia::buscarNRC(const std::string& codigo)
{
    Nodo_T<NRC>* nodo = listaNRC.buscar(
        [&codigo](const NRC& nrc) { return nrc.getCodigo() == codigo; });
    return nodo == nullptr ? nullptr : &nodo->getDato();
}
const NRC* ControlAsistencia::buscarNRC(const std::string& codigo) const
{
    const Nodo_T<NRC>* nodo = listaNRC.buscar(
        [&codigo](const NRC& nrc) { return nrc.getCodigo() == codigo; });
    return nodo == nullptr ? nullptr : &nodo->getDato();
}
Periodo* ControlAsistencia::buscarPeriodo(const std::string& codigo)
{
    Nodo_T<Periodo>* nodo = listaPeriodos.buscar(
        [&codigo](const Periodo& periodo) {
            return periodo.getCodigo() == codigo;
        });
    return nodo == nullptr ? nullptr : &nodo->getDato();
}
const Periodo* ControlAsistencia::buscarPeriodo(const std::string& codigo) const
{
    const Nodo_T<Periodo>* nodo = listaPeriodos.buscar(
        [&codigo](const Periodo& periodo) {
            return periodo.getCodigo() == codigo;
        });
    return nodo == nullptr ? nullptr : &nodo->getDato();
}
FranjaHoraria* ControlAsistencia::buscarFranja(int id)
{
    Nodo_T<FranjaHoraria>* nodo = listaFranjas.buscar(
        [id](const FranjaHoraria& franja) { return franja.getId() == id; });
    return nodo == nullptr ? nullptr : &nodo->getDato();
}
const FranjaHoraria* ControlAsistencia::buscarFranja(int id) const
{
    const Nodo_T<FranjaHoraria>* nodo = listaFranjas.buscar(
        [id](const FranjaHoraria& franja) { return franja.getId() == id; });
    return nodo == nullptr ? nullptr : &nodo->getDato();
}
RegistroAsistencia* ControlAsistencia::buscarRegistro(const std::string& id)
{
    Nodo_T<RegistroAsistencia>* nodo =
        listaAsistencias.buscar(RegistroAsistencia(id, "", Fecha(), "", 0));
    return nodo == nullptr ? nullptr : &nodo->getDato();
}

bool ControlAsistencia::insertarRegistro(const RegistroAsistencia& registro,
                                          std::string& mensaje)
{
    const NRC* nrc = buscarNRC(registro.getCodigoNRC());
    const FranjaHoraria* franja = buscarFranja(registro.getIdFranja());
    if (nrc == nullptr || franja == nullptr ||
        nrc->getCedulaDocente() != registro.getCedulaDocente() ||
        franja->getCodigoNRC() != registro.getCodigoNRC() ||
        !ValidadorCedula::validarId(std::to_string(registro.getIdFranja())) ||
        !(registro.getId() == crearIdRegistro(
              registro.getCedulaDocente(), registro.getFecha(),
              registro.getCodigoNRC(), registro.getIdFranja())) ||
        diaDeLaFecha(registro.getFecha()) != franja->getDiaSemana())
    {
        mensaje = "Registro invalido o con referencias inexistentes.";
        return false;
    }
    if (buscarRegistro(registro.getId()) != nullptr)
    {
        mensaje = "El registro de asistencia ya existe.";
        return false;
    }
    const int inicio = franja->getHoraInicio().minutosDelDia();
    const int fin = franja->getHoraFin().minutosDelDia();
    if ((registro.tieneEntrada() &&
         (registro.getEntrada().minutosDelDia() < inicio ||
          registro.getEntrada().minutosDelDia() > inicio + 10)) ||
        (registro.tieneSalida() &&
         (!registro.tieneEntrada() ||
          registro.getSalida().minutosDelDia() < fin - 10 ||
          registro.getSalida().minutosDelDia() > fin ||
          registro.getSalida().minutosDelDia() <
              registro.getEntrada().minutosDelDia())))
    {
        mensaje = "Las marcas no cumplen las ventanas permitidas.";
        return false;
    }
    listaAsistencias.insertar(registro);
    if (!guardarAsistencias())
    {
        listaAsistencias.eliminar(registro);
        mensaje = "No se pudo guardar asistencias.txt.";
        return false;
    }
    mensaje = "Registro de asistencia insertado.";
    return true;
}

void ControlAsistencia::imprimirDocente(const std::string& cedulaOCorreo) const
{
    const Docente* docente = buscarDocente(cedulaOCorreo);
    if (docente == nullptr)
        std::cout << "Docente no encontrado.\n";
    else
        std::cout << *docente << '\n';
}

bool ControlAsistencia::eliminarDocente(const std::string& cedula,
                                         std::string& mensaje)
{
    Docente* docente = buscarDocente(cedula);
    if (docente == nullptr || docente->getCedula() != cedula)
    {
        mensaje = "Docente no encontrado por cedula.";
        return false;
    }
    const Docente copiaDocente = *docente;

    ListasDoble_T<NRC> nrcEliminados;
    ListasDoble_T<FranjaHoraria> franjasEliminadas;
    ListasDoble_T<RegistroAsistencia> registrosEliminados;
    listaNRC.recorrer([&](const NRC& nrc) {
        if (nrc.getCedulaDocente() == cedula)
            nrcEliminados.insertar(nrc);
    });
    listaFranjas.recorrer([&](const FranjaHoraria& franja) {
        bool pertenece = false;
        nrcEliminados.recorrer([&](const NRC& nrc) {
            if (nrc.getCodigo() == franja.getCodigoNRC())
                pertenece = true;
        });
        if (pertenece)
            franjasEliminadas.insertar(franja);
    });
    listaAsistencias.recorrer([&](const RegistroAsistencia& registro) {
        if (registro.getCedulaDocente() == cedula)
            registrosEliminados.insertar(registro);
    });

    registrosEliminados.recorrer([&](const RegistroAsistencia& registro) {
        listaAsistencias.eliminar(registro);
    });
    franjasEliminadas.recorrer([&](const FranjaHoraria& franja) {
        listaFranjas.eliminar(franja);
    });
    nrcEliminados.recorrer([&](const NRC& nrc) { listaNRC.eliminar(nrc); });
    listaDocentes.eliminar(copiaDocente);

    if (!guardarAsistencias() || !guardarFranjas() || !guardarNRC() ||
        !guardarDocentes())
    {
        registrosEliminados.recorrer([&](const RegistroAsistencia& item) {
            listaAsistencias.insertar(item);
        });
        franjasEliminadas.recorrer([&](const FranjaHoraria& item) {
            listaFranjas.insertar(item);
        });
        nrcEliminados.recorrer([&](const NRC& item) { listaNRC.insertar(item); });
        listaDocentes.insertar(copiaDocente);
        guardarAsistencias();
        guardarFranjas();
        guardarNRC();
        guardarDocentes();
        mensaje = "No se pudo persistir la eliminacion del docente.";
        return false;
    }
    mensaje = "Docente y sus asignaciones/asistencias eliminados.";
    return true;
}

void ControlAsistencia::imprimirNRC(const std::string& codigo) const
{
    const NRC* nrc = buscarNRC(codigo);
    if (nrc == nullptr)
        std::cout << "NRC no encontrado.\n";
    else
        std::cout << *nrc << '\n';
}

bool ControlAsistencia::eliminarNRC(const std::string& codigo,
                                     std::string& mensaje)
{
    NRC* nrc = buscarNRC(codigo);
    if (nrc == nullptr)
    {
        mensaje = "NRC no encontrado.";
        return false;
    }
    const NRC copia = *nrc;
    ListasDoble_T<FranjaHoraria> franjasEliminadas;
    ListasDoble_T<RegistroAsistencia> registrosEliminados;
    listaFranjas.recorrer([&](const FranjaHoraria& franja) {
        if (franja.getCodigoNRC() == codigo)
            franjasEliminadas.insertar(franja);
    });
    listaAsistencias.recorrer([&](const RegistroAsistencia& registro) {
        if (registro.getCodigoNRC() == codigo)
            registrosEliminados.insertar(registro);
    });
    registrosEliminados.recorrer([&](const RegistroAsistencia& item) {
        listaAsistencias.eliminar(item);
    });
    franjasEliminadas.recorrer([&](const FranjaHoraria& item) {
        listaFranjas.eliminar(item);
    });
    listaNRC.eliminar(copia);
    if (!guardarAsistencias() || !guardarFranjas() || !guardarNRC())
    {
        listaNRC.insertar(copia);
        franjasEliminadas.recorrer([&](const FranjaHoraria& item) {
            listaFranjas.insertar(item);
        });
        registrosEliminados.recorrer([&](const RegistroAsistencia& item) {
            listaAsistencias.insertar(item);
        });
        guardarNRC();
        guardarFranjas();
        guardarAsistencias();
        mensaje = "No se pudo persistir la eliminacion del NRC.";
        return false;
    }
    mensaje = "NRC, franjas y asistencias dependientes eliminados.";
    return true;
}

void ControlAsistencia::imprimirPeriodo(const std::string& codigo) const
{
    const Periodo* periodo = buscarPeriodo(codigo);
    if (periodo == nullptr)
        std::cout << "Periodo no encontrado.\n";
    else
        std::cout << *periodo << '\n';
}

bool ControlAsistencia::eliminarPeriodo(const std::string& codigo,
                                         std::string& mensaje)
{
    Periodo* periodo = buscarPeriodo(codigo);
    if (periodo == nullptr)
    {
        mensaje = "Periodo no encontrado.";
        return false;
    }
    const Periodo copiaPeriodo = *periodo;
    ListasDoble_T<NRC> nrcEliminados;
    ListasDoble_T<FranjaHoraria> franjasEliminadas;
    ListasDoble_T<RegistroAsistencia> registrosEliminados;
    listaNRC.recorrer([&](const NRC& nrc) {
        if (nrc.getCodigoPeriodo() == codigo)
            nrcEliminados.insertar(nrc);
    });
    listaFranjas.recorrer([&](const FranjaHoraria& franja) {
        nrcEliminados.recorrer([&](const NRC& nrc) {
            if (nrc.getCodigo() == franja.getCodigoNRC())
                franjasEliminadas.insertar(franja);
        });
    });
    listaAsistencias.recorrer([&](const RegistroAsistencia& registro) {
        nrcEliminados.recorrer([&](const NRC& nrc) {
            if (nrc.getCodigo() == registro.getCodigoNRC())
                registrosEliminados.insertar(registro);
        });
    });
    registrosEliminados.recorrer([&](const RegistroAsistencia& item) {
        listaAsistencias.eliminar(item);
    });
    franjasEliminadas.recorrer([&](const FranjaHoraria& item) {
        listaFranjas.eliminar(item);
    });
    nrcEliminados.recorrer([&](const NRC& item) { listaNRC.eliminar(item); });
    listaPeriodos.eliminar(copiaPeriodo);
    if (!guardarAsistencias() || !guardarFranjas() || !guardarNRC() ||
        !guardarPeriodos())
    {
        listaPeriodos.insertar(copiaPeriodo);
        nrcEliminados.recorrer([&](const NRC& item) { listaNRC.insertar(item); });
        franjasEliminadas.recorrer([&](const FranjaHoraria& item) {
            listaFranjas.insertar(item);
        });
        registrosEliminados.recorrer([&](const RegistroAsistencia& item) {
            listaAsistencias.insertar(item);
        });
        guardarPeriodos();
        guardarNRC();
        guardarFranjas();
        guardarAsistencias();
        mensaje = "No se pudo persistir la eliminacion del periodo.";
        return false;
    }
    mensaje = "Periodo y datos dependientes eliminados.";
    return true;
}

void ControlAsistencia::imprimirFranja(int id) const
{
    const FranjaHoraria* franja = buscarFranja(id);
    if (franja == nullptr)
        std::cout << "Franja no encontrada.\n";
    else
        std::cout << *franja << '\n';
}

bool ControlAsistencia::eliminarFranja(int id, std::string& mensaje)
{
    FranjaHoraria* franja = buscarFranja(id);
    if (franja == nullptr)
    {
        mensaje = "Franja no encontrada.";
        return false;
    }
    const FranjaHoraria copia = *franja;
    ListasDoble_T<RegistroAsistencia> registrosEliminados;
    listaAsistencias.recorrer([&](const RegistroAsistencia& registro) {
        if (registro.getIdFranja() == id)
            registrosEliminados.insertar(registro);
    });
    registrosEliminados.recorrer([&](const RegistroAsistencia& item) {
        listaAsistencias.eliminar(item);
    });
    listaFranjas.eliminar(copia);
    if (!guardarAsistencias() || !guardarFranjas())
    {
        listaFranjas.insertar(copia);
        registrosEliminados.recorrer([&](const RegistroAsistencia& item) {
            listaAsistencias.insertar(item);
        });
        guardarFranjas();
        guardarAsistencias();
        mensaje = "No se pudo persistir la eliminacion de la franja.";
        return false;
    }
    mensaje = "Franja y asistencias dependientes eliminadas.";
    return true;
}

bool ControlAsistencia::registrarDocente(const Docente& docente, std::string& mensaje)
{
    ValidadorCedula validador;
    if (!validador.validarCedula(docente.getCedula()))
    {
        mensaje = "Cedula ecuatoriana invalida.";
        return false;
    }
    if (!ValidadorCedula::validarNombres(docente.getNombre()) ||
        !ValidadorCedula::validarNombres(docente.getApellido()))
    {
        mensaje = "Nombres o apellidos invalidos.";
        return false;
    }
    if (!ValidadorCedula::validarContrasenia(docente.getContrasenia()))
    {
        mensaje = "La contrasena debe tener entre 4 y 20 caracteres sin espacios ni |.";
        return false;
    }
    if (buscarDocente(docente.getCedula()) != nullptr)
    {
        mensaje = "Ya existe un docente con esa cedula.";
        return false;
    }
    Docente docenteRegistrado = docente;
    const std::string correo = correoDisponible(docente, listaDocentes);
    docenteRegistrado.setCorreo(correo);
    listaDocentes.insertar(docenteRegistrado);
    if (!guardarDocentes())
    {
        listaDocentes.eliminar(docenteRegistrado);
        mensaje = "No se pudo guardar docentes.txt.";
        return false;
    }
    mensaje = "Docente registrado correctamente. Correo generado: " + correo;
    return true;
}

bool ControlAsistencia::insertarDocente(const Docente& docente,
                                         std::string& mensaje)
{
    return registrarDocente(docente, mensaje);
}

Docente* ControlAsistencia::iniciarSesion(const std::string& usuario,
                                           const std::string& contrasenia)
{
    Nodo_T<Docente>* actual = listaDocentes.getCabeza();
    for (std::size_t i = 0; i < listaDocentes.tamanio(); ++i)
    {
        Docente& docente = actual->getDato();
        if ((docente.getCedula() == usuario || docente.getCorreo() == usuario) &&
            docente.getContrasenia() == contrasenia)
            return &docente;
        actual = actual->getSiguiente();
    }
    return nullptr;
}

bool ControlAsistencia::agregarPeriodo(const Periodo& periodo, std::string& mensaje)
{
    if (!ValidadorCedula::validarCodigoPeriodo(periodo.getCodigo()) ||
        !ValidadorCedula::validarDescripcionPeriodo(periodo.getDescripcion()))
    {
        mensaje = "Codigo o descripcion del periodo invalido.";
        return false;
    }
    if (buscarPeriodo(periodo.getCodigo()) != nullptr)
    {
        mensaje = "El periodo ya existe.";
        return false;
    }
    listaPeriodos.insertar(periodo);
    if (!guardarPeriodos())
    {
        listaPeriodos.eliminar(periodo);
        mensaje = "No se pudo guardar periodos.txt.";
        return false;
    }
    mensaje = "Periodo registrado.";
    return true;
}

bool ControlAsistencia::insertarPeriodo(const Periodo& periodo,
                                         std::string& mensaje)
{
    return agregarPeriodo(periodo, mensaje);
}

bool ControlAsistencia::asignarNRC(const NRC& nrc, std::string& mensaje)
{
    if (!ValidadorCedula::validarCodigoNRC(nrc.getCodigo()) ||
        !ValidadorCedula::validarMateria(nrc.getNombre()))
    {
        mensaje = "Codigo NRC o materia invalido.";
        return false;
    }
    if (buscarDocente(nrc.getCedulaDocente()) == nullptr)
    {
        mensaje = "No existe el docente indicado.";
        return false;
    }
    if (buscarPeriodo(nrc.getCodigoPeriodo()) == nullptr)
    {
        mensaje = "No existe el periodo indicado.";
        return false;
    }
    if (buscarNRC(nrc.getCodigo()) != nullptr)
    {
        mensaje = "El NRC ya existe.";
        return false;
    }
    listaNRC.insertar(nrc);
    if (!guardarNRC())
    {
        listaNRC.eliminar(nrc);
        mensaje = "No se pudo guardar nrc.txt.";
        return false;
    }
    mensaje = "Materia/NRC asignado.";
    return true;
}

bool ControlAsistencia::insertarNRC(const NRC& nrc, std::string& mensaje)
{
    return asignarNRC(nrc, mensaje);
}

bool ControlAsistencia::asignacionPertenece(const std::string& cedula,
                                             const FranjaHoraria& franja) const
{
    bool pertenece = false;
    listaNRC.recorrer([&](const NRC& nrc) {
        if (nrc.getCodigo() == franja.getCodigoNRC() &&
            nrc.getCedulaDocente() == cedula)
            pertenece = true;
    });
    return pertenece;
}

bool ControlAsistencia::asignarFranja(const FranjaHoraria& franja,
                                       std::string& mensaje)
{
    if (buscarFranja(franja.getId()) != nullptr)
    {
        mensaje = "El identificador de franja ya existe.";
        return false;
    }
    NRC* nrcNuevo = buscarNRC(franja.getCodigoNRC());
    if (nrcNuevo == nullptr)
    {
        mensaje = "No existe el NRC de la franja.";
        return false;
    }
    if (!ValidadorCedula::validarDiaLaborable(franja.getDiaSemana()))
    {
        mensaje = "DÍA NO LABORABLE: las clases solo se dictan de lunes a "
                  "viernes. No es posible asignar sábado ni domingo.";
        return false;
    }
    if (!ValidadorCedula::validarBloqueHorario(
            franja.getHoraInicio().toStringHoraMinuto() + "-" +
            franja.getHoraFin().toStringHoraMinuto()))
    {
        mensaje = "HORARIO NO PERMITIDO: las clases solo pueden ser 07-09, "
                  "09-11, 11-13, 13-15, 15-17, 17-19 o 19-21.";
        return false;
    }
    bool cruce = false;
    listaFranjas.recorrer([&](const FranjaHoraria& existente) {
        NRC* nrcExistente = buscarNRC(existente.getCodigoNRC());
        if (nrcExistente == nullptr ||
            nrcExistente->getCedulaDocente() != nrcNuevo->getCedulaDocente() ||
            existente.getDiaSemana() != franja.getDiaSemana())
            return;
        const bool seSuperpone =
            franja.getHoraInicio() < existente.getHoraFin() &&
            existente.getHoraInicio() < franja.getHoraFin();
        if (seSuperpone)
            cruce = true;
    });
    if (cruce)
    {
        mensaje = "CRUCE DE HORARIOS: la franja se superpone con otra asignacion.";
        return false;
    }

    const std::string dia = franja.getDiaSemana();
    {
        std::size_t materiasDiferentes = 0;
        Nodo_T<FranjaHoraria>* nodo = listaFranjas.getCabeza();
        for (std::size_t i = 0; i < listaFranjas.tamanio(); ++i)
        {
            const FranjaHoraria& existente = nodo->getDato();
            NRC* nrcExistente = buscarNRC(existente.getCodigoNRC());
            if (nrcExistente != nullptr && nrcExistente->getCedulaDocente() ==
                    nrcNuevo->getCedulaDocente() &&
                existente.getDiaSemana() == dia &&
                normalizarNombreCorreo(nrcExistente->getNombre()) !=
                    normalizarNombreCorreo(nrcNuevo->getNombre()))
            {
                bool repetida = false;
                Nodo_T<FranjaHoraria>* previo = listaFranjas.getCabeza();
                for (std::size_t j = 0; j < i; ++j)
                {
                    NRC* nrcPrevio = buscarNRC(previo->getDato().getCodigoNRC());
                    if (nrcPrevio != nullptr &&
                        nrcPrevio->getCedulaDocente() == nrcNuevo->getCedulaDocente() &&
                        previo->getDato().getDiaSemana() == dia &&
                        normalizarNombreCorreo(nrcPrevio->getNombre()) ==
                            normalizarNombreCorreo(nrcExistente->getNombre()))
                    {
                        repetida = true;
                        break;
                    }
                    previo = previo->getSiguiente();
                }
                if (!repetida)
                    ++materiasDiferentes;
            }
            nodo = nodo->getSiguiente();
        }
        bool nuevaMateriaYaContada = false;
        nodo = listaFranjas.getCabeza();
        for (std::size_t i = 0; i < listaFranjas.tamanio(); ++i)
        {
            NRC* existente = buscarNRC(nodo->getDato().getCodigoNRC());
            if (existente != nullptr &&
                existente->getCedulaDocente() == nrcNuevo->getCedulaDocente() &&
                nodo->getDato().getDiaSemana() == dia &&
                normalizarNombreCorreo(existente->getNombre()) ==
                    normalizarNombreCorreo(nrcNuevo->getNombre()))
                nuevaMateriaYaContada = true;
            nodo = nodo->getSiguiente();
        }
        if (!nuevaMateriaYaContada)
            ++materiasDiferentes;
        if (materiasDiferentes > 3)
        {
            mensaje = "SOBRECARGA HORARIA: El docente ya tiene asignadas tres "
                      "materias para este día. No es posible registrar una "
                      "cuarta materia.";
            return false;
        }
    }

    listaFranjas.insertar(franja);
    if (!guardarFranjas())
    {
        listaFranjas.eliminar(franja);
        mensaje = "No se pudo guardar franjas_horarias.txt.";
        return false;
    }
    mensaje = "Franja horaria asignada.";
    return true;
}

bool ControlAsistencia::insertarFranja(const FranjaHoraria& franja,
                                        std::string& mensaje)
{
    return asignarFranja(franja, mensaje);
}

std::string ControlAsistencia::crearIdRegistro(const std::string& cedula,
                                                const Fecha& fecha,
                                                const std::string& nrc,
                                                int franja) const
{
    const std::string fechaId =
        (fecha.getDia() < 10 ? "0" : "") + std::to_string(fecha.getDia()) +
        (fecha.getMes() < 10 ? "0" : "") + std::to_string(fecha.getMes()) +
        std::to_string(fecha.getAnio());
    return cedula + "-" + fechaId + "-" + nrc + "-" +
           std::to_string(franja);
}

bool ControlAsistencia::registrarEntrada(const std::string& cedula,
                                          const std::string& codigoNRC,
                                          int idFranja, const Fecha& fecha,
                                          const Hora& horaActual,
                                          std::string& mensaje)
{
    FranjaHoraria* franja = buscarFranja(idFranja);
    if (franja == nullptr || franja->getCodigoNRC() != codigoNRC ||
        !asignacionPertenece(cedula, *franja))
    {
        mensaje = "La franja no pertenece a una materia asignada al docente.";
        return false;
    }
    if (diaDeLaFecha(fecha) != franja->getDiaSemana())
    {
        mensaje = "La franja no corresponde al dia de la fecha indicada.";
        return false;
    }
    const int marca = horaActual.minutosDelDia();
    const int inicio = franja->getHoraInicio().minutosDelDia();
    if (marca < inicio || marca > inicio + 10)
    {
        mensaje = "Entrada fuera de la ventana permitida (inicio a +10 minutos).";
        return false;
    }
    const std::string id = crearIdRegistro(cedula, fecha, codigoNRC, idFranja);
    if (buscarRegistro(id) != nullptr)
    {
        mensaje = "La entrada para esta clase ya fue registrada.";
        return false;
    }
    RegistroAsistencia registro(id, cedula, fecha, codigoNRC, idFranja);
    if (!registro.registrarEntrada(horaActual, *franja))
    {
        mensaje = "No se pudo registrar la entrada.";
        return false;
    }
    if (!insertarRegistro(registro, mensaje))
        return false;
    mensaje = "Entrada registrada correctamente.";
    return true;
}

bool ControlAsistencia::registrarSalida(const std::string& cedula,
                                         const std::string& codigoNRC,
                                         int idFranja, const Fecha& fecha,
                                         const Hora& horaActual,
                                         std::string& mensaje)
{
    FranjaHoraria* franja = buscarFranja(idFranja);
    if (franja == nullptr || franja->getCodigoNRC() != codigoNRC ||
        !asignacionPertenece(cedula, *franja))
    {
        mensaje = "La franja no pertenece a una materia asignada al docente.";
        return false;
    }
    if (diaDeLaFecha(fecha) != franja->getDiaSemana())
    {
        mensaje = "La franja no corresponde al dia de la fecha indicada.";
        return false;
    }
    const int marca = horaActual.minutosDelDia();
    const int fin = franja->getHoraFin().minutosDelDia();
    if (marca < fin - 10 || marca > fin)
    {
        mensaje = "Salida fuera de la ventana permitida (ultimos 10 minutos).";
        return false;
    }
    RegistroAsistencia* registro =
        buscarRegistro(crearIdRegistro(cedula, fecha, codigoNRC, idFranja));
    if (registro == nullptr)
    {
        mensaje = "Debe registrar la entrada antes de la salida.";
        return false;
    }
    const RegistroAsistencia copia = *registro;
    if (!registro->registrarSalida(horaActual, *franja))
    {
        mensaje = "Salida duplicada o invalida.";
        return false;
    }
    if (!guardarAsistencias())
    {
        *registro = copia;
        mensaje = "No se pudo guardar asistencias.txt.";
        return false;
    }
    mensaje = "Salida registrada correctamente.";
    return true;
}

bool ControlAsistencia::corregirEntrada(const std::string& idRegistro,
                                         const std::string& cedula,
                                         const Hora& nuevaHora,
                                         const Hora& horaActual,
                                         std::string& mensaje)
{
    RegistroAsistencia* registro = buscarRegistro(idRegistro);
    if (registro == nullptr || registro->getCedulaDocente() != cedula)
    {
        mensaje = "El registro no existe o no pertenece al docente.";
        return false;
    }
    FranjaHoraria* franja = buscarFranja(registro->getIdFranja());
    if (franja == nullptr || !(registro->getFecha() == Fecha::hoy()))
    {
        mensaje = "La correccion solo se permite el mismo dia de la clase.";
        return false;
    }
    const int ahora = horaActual.minutosDelDia();
    const int inicio = franja->getHoraInicio().minutosDelDia();
    if (ahora < inicio || ahora > inicio + 10 ||
        nuevaHora.minutosDelDia() < inicio ||
        nuevaHora.minutosDelDia() > inicio + 10 ||
        (registro->tieneSalida() &&
         nuevaHora.minutosDelDia() > registro->getSalida().minutosDelDia()))
    {
        mensaje = "La correccion esta fuera de la ventana permitida.";
        return false;
    }
    const RegistroAsistencia copia = *registro;
    registro->setEntrada(nuevaHora);
    if (!guardarAsistencias())
    {
        *registro = copia;
        mensaje = "No se pudo guardar asistencias.txt.";
        return false;
    }
    mensaje = "Entrada corregida.";
    return true;
}

bool ControlAsistencia::corregirSalida(const std::string& idRegistro,
                                        const std::string& cedula,
                                        const Hora& nuevaHora,
                                        const Hora& horaActual,
                                        std::string& mensaje)
{
    RegistroAsistencia* registro = buscarRegistro(idRegistro);
    if (registro == nullptr || registro->getCedulaDocente() != cedula)
    {
        mensaje = "El registro no existe o no pertenece al docente.";
        return false;
    }
    FranjaHoraria* franja = buscarFranja(registro->getIdFranja());
    if (franja == nullptr || !(registro->getFecha() == Fecha::hoy()))
    {
        mensaje = "La correccion solo se permite el mismo dia de la clase.";
        return false;
    }
    const int ahora = horaActual.minutosDelDia();
    const int fin = franja->getHoraFin().minutosDelDia();
    if (!registro->tieneEntrada() || !registro->tieneSalida() ||
        ahora < fin - 10 || ahora > fin ||
        nuevaHora.minutosDelDia() < fin - 10 ||
        nuevaHora.minutosDelDia() > fin ||
        nuevaHora.minutosDelDia() < registro->getEntrada().minutosDelDia())
    {
        mensaje = "La correccion esta fuera de la ventana permitida.";
        return false;
    }
    const RegistroAsistencia copia = *registro;
    registro->setSalida(nuevaHora);
    if (!guardarAsistencias())
    {
        *registro = copia;
        mensaje = "No se pudo guardar asistencias.txt.";
        return false;
    }
    mensaje = "Salida corregida.";
    return true;
}

bool ControlAsistencia::eliminarRegistro(const std::string& idRegistro,
                                          std::string& mensaje)
{
    RegistroAsistencia* registro = buscarRegistro(idRegistro);
    if (registro == nullptr)
    {
        mensaje = "Registro no encontrado.";
        return false;
    }
    const RegistroAsistencia copia = *registro;
    listaAsistencias.eliminar(copia);
    if (!guardarAsistencias())
    {
        listaAsistencias.insertar(copia);
        mensaje = "No se pudo guardar asistencias.txt.";
        return false;
    }
    mensaje = "Registro eliminado.";
    return true;
}

void ControlAsistencia::imprimirHorarios(const std::string& cedula) const
{
    bool encontrado = false;
    listaFranjas.recorrer([&](const FranjaHoraria& franja) {
        const NRC* nrc = buscarNRC(franja.getCodigoNRC());
        if (nrc != nullptr && nrc->getCedulaDocente() == cedula)
        {
            encontrado = true;
            const Periodo* periodo = buscarPeriodo(nrc->getCodigoPeriodo());
            std::cout << "Materia: " << nrc->getNombre() << " | NRC: "
                      << nrc->getCodigo() << " | Periodo: "
                      << (periodo == nullptr ? "N/D" : periodo->getDescripcion())
                      << " | " << franja.getDiaSemana() << ' '
                      << franja.getHoraInicio().toStringHoraMinuto() << '-'
                      << franja.getHoraFin().toStringHoraMinuto() << " | Franja "
                      << franja.getId() << '\n';
        }
    });
    if (!encontrado)
        std::cout << "No tiene materias ni franjas asignadas.\n";
}

void ControlAsistencia::imprimirHistorial(const std::string& cedula) const
{
    bool encontrado = false;
    listaAsistencias.recorrer([&](const RegistroAsistencia& registro) {
        if (!cedula.empty() && registro.getCedulaDocente() != cedula)
            return;
        encontrado = true;
        const Docente* docente = buscarDocente(registro.getCedulaDocente());
        const NRC* nrc = buscarNRC(registro.getCodigoNRC());
        const FranjaHoraria* franja = buscarFranja(registro.getIdFranja());
        const Periodo* periodo =
            nrc == nullptr ? nullptr : buscarPeriodo(nrc->getCodigoPeriodo());
        std::cout << "ID " << registro.getId() << " | "
                  << registro.getFecha().toString() << " | "
                  << (docente == nullptr ? registro.getCedulaDocente() :
                                           docente->nombreCompleto())
                  << " | " << (nrc == nullptr ? registro.getCodigoNRC() :
                                               nrc->getNombre())
                  << " | NRC " << registro.getCodigoNRC() << " | "
                  << (periodo == nullptr ? "N/D" : periodo->getDescripcion())
                  << " | Franja "
                  << (franja == nullptr ? "N/D" :
                      std::to_string(franja->getId()) + " " +
                      franja->getDiaSemana() + " " +
                      franja->getHoraInicio().toStringHoraMinuto() + "-" +
                      franja->getHoraFin().toStringHoraMinuto())
                  << " | Entrada "
                  << (registro.tieneEntrada() ? registro.getEntrada().toString() :
                                                  "--:--:--")
                  << " | Salida "
                  << (registro.tieneSalida() ? registro.getSalida().toString() :
                                                 "--:--:--")
                  << " | " << estadoRegistro(registro) << '\n';
    });
    if (!encontrado)
        std::cout << "No hay registros de asistencia.\n";
}

void ControlAsistencia::imprimirHistorialPorFecha(const std::string& cedula,
                                                   const Fecha& fecha) const
{
    bool encontrado = false;
    listaAsistencias.recorrer([&](const RegistroAsistencia& registro) {
        if (registro.getCedulaDocente() != cedula ||
            !(registro.getFecha() == fecha))
            return;
        encontrado = true;
        const NRC* nrc = buscarNRC(registro.getCodigoNRC());
        const FranjaHoraria* franja = buscarFranja(registro.getIdFranja());
        const Periodo* periodo =
            nrc == nullptr ? nullptr : buscarPeriodo(nrc->getCodigoPeriodo());
        std::cout << "ID " << registro.getId() << " | "
                  << registro.getFecha().toString() << " | "
                  << (nrc == nullptr ? registro.getCodigoNRC() : nrc->getNombre())
                  << " | NRC " << registro.getCodigoNRC() << " | "
                  << (periodo == nullptr ? "N/D" : periodo->getDescripcion())
                  << " | Franja "
                  << (franja == nullptr ? "N/D" :
                      std::to_string(franja->getId()) + " " +
                      franja->getDiaSemana() + " " +
                      franja->getHoraInicio().toStringHoraMinuto() + "-" +
                      franja->getHoraFin().toStringHoraMinuto())
                  << " | Entrada "
                  << (registro.tieneEntrada() ? registro.getEntrada().toString() :
                                                  "--:--:--")
                  << " | Salida "
                  << (registro.tieneSalida() ? registro.getSalida().toString() :
                                                 "--:--:--")
                  << " | " << estadoRegistro(registro) << '\n';
    });
    if (!encontrado)
        std::cout << "No hay asistencias para esa fecha.\n";
}

void ControlAsistencia::imprimirAsistenciasPorFecha(const Fecha& fecha) const
{
    bool encontrado = false;
    listaAsistencias.recorrer([&](const RegistroAsistencia& registro) {
        if (registro.getFecha() == fecha)
        {
            std::cout << registro << '\n';
            encontrado = true;
        }
    });
    if (!encontrado)
        std::cout << "No hay asistencias para esa fecha.\n";
}

void ControlAsistencia::imprimirRegistro(const std::string& id) const
{
    const RegistroAsistencia* registro = nullptr;
    listaAsistencias.recorrer([&](const RegistroAsistencia& actual) {
        if (actual.getId() == id)
            registro = &actual;
    });
    if (registro == nullptr)
        std::cout << "Registro no encontrado.\n";
    else
        std::cout << *registro << '\n';
}

void ControlAsistencia::imprimirRegistros() const
{
    if (listaAsistencias.estaVacia())
        std::cout << "No hay registros de asistencia.\n";
    else
        listaAsistencias.imprimir();
}

void ControlAsistencia::imprimirDocentes() const
{
    if (listaDocentes.estaVacia())
        std::cout << "No hay docentes registrados.\n";
    else
        listaDocentes.imprimir();
}
void ControlAsistencia::imprimirNRC() const
{
    if (listaNRC.estaVacia())
        std::cout << "No hay NRC registrados.\n";
    else
        listaNRC.imprimir();
}
void ControlAsistencia::imprimirNRCs() const
{
    imprimirNRC();
}
void ControlAsistencia::imprimirPeriodos() const
{
    if (listaPeriodos.estaVacia())
        std::cout << "No hay periodos registrados.\n";
    else
        listaPeriodos.imprimir();
}
void ControlAsistencia::imprimirFranjas() const
{
    if (listaFranjas.estaVacia())
        std::cout << "No hay franjas asignadas.\n";
    else
        listaFranjas.imprimir();
}

ListasDoble_T<Docente>& ControlAsistencia::docentes() { return listaDocentes; }
ListasDoble_T<NRC>& ControlAsistencia::cursos() { return listaNRC; }
ListasDoble_T<Periodo>& ControlAsistencia::periodos() { return listaPeriodos; }
ListasDoble_T<FranjaHoraria>& ControlAsistencia::franjas() { return listaFranjas; }
ListasDoble_T<RegistroAsistencia>& ControlAsistencia::registros()
{
    return listaAsistencias;
}
