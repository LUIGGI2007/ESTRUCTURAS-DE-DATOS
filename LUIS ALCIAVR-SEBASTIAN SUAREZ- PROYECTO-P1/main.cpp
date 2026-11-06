#include "ControlAsistencia.h"
#include "IngresoDatos.h"
#include "ValidadorCedula.h"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <string>

namespace
{
void limpiarPantalla()
{
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

int leerOpcion(const std::string& mensaje, const std::string& patron)
{
    IngresoDatos entrada;
    while (true)
    {
        const std::string texto = entrada.leerCampo(
            mensaje, patron, "^[0-9]$");
        if (ValidadorCedula::validarOpcionMenu(texto, patron))
            return std::stoi(texto);
        std::cout << "Opcion fuera de rango. Intente nuevamente.\n";
    }
}

int leerSeleccionLista(std::size_t cantidad)
{
    IngresoDatos entrada;
    while (true)
    {
        const std::string texto = entrada.leerCampo(
            "Seleccione un elemento (0 para cancelar): ",
            "^(0|[1-9][0-9]?)$", "^[0-9]$");
        if (ValidadorCedula::validarSeleccionLista(texto, cantidad))
            return std::stoi(texto);
        std::cout << "Seleccion fuera del rango mostrado.\n";
    }
}

void pausar()
{
    IngresoDatos entrada;
    entrada.leerCampo("Presione Enter para continuar...", "^$",
                      "^[\\x20-\\x7E]$");
}

void resultado(bool correcto, const std::string& mensaje)
{
    std::cout << (correcto ? "[OK] " : "[ERROR] ") << mensaje << '\n';
    pausar();
}

bool confirmarAccion(const std::string& mensaje)
{
    IngresoDatos entrada;
    const std::string opcion = entrada.leerCampo(
        mensaje + " (S/N): ", "^[SsNn]$", "^[SsNn]$");
    return ValidadorCedula::validarOpcionSiNo(opcion) &&
           ValidadorCedula::validarCampo(opcion, "^[Ss]$");
}

Hora leerHoraManual(const std::string& encabezado)
{
    IngresoDatos entrada;
    while (true)
    {
        std::cout << encabezado << '\n';
        const std::string hora = entrada.leerSoloNumeros(
            "Hora (00-23): ", "^([01][0-9]|2[0-3])$");
        const std::string minuto = entrada.leerSoloNumeros(
            "Minuto (00-59): ", "^[0-5][0-9]$");
        const std::string segundo = entrada.leerSoloNumeros(
            "Segundo (00-59): ", "^[0-5][0-9]$");
        const std::string completa = hora + ":" + minuto + ":" + segundo;
        if (ValidadorCedula::validarHoraCompleta(completa))
            return Hora(std::stoi(hora), std::stoi(minuto), std::stoi(segundo));
        std::cout << "Hora completa invalida; vuelva a ingresarla.\n";
    }
}

Fecha leerFecha()
{
    IngresoDatos entrada;
    while (true)
    {
        const std::string texto = entrada.leerCampo(
            "Fecha (DD/MM/AAAA): ",
            "^(0[1-9]|[12][0-9]|3[01])/(0[1-9]|1[0-2])/[0-9]{4}$",
            "^[0-9/]$");
        Fecha fecha;
        if (Fecha::parsear(texto, fecha))
            return fecha;
        std::cout << "Fecha inexistente. Intente nuevamente.\n";
    }
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

Hora horaActualSistema()
{
    const std::time_t ahora = std::time(nullptr);
    const std::tm* local = std::localtime(&ahora);
    if (local == nullptr)
        throw std::runtime_error("No se pudo obtener la hora actual");
    return Hora(local->tm_hour, local->tm_min, local->tm_sec);
}

const Docente* seleccionarDocente(ControlAsistencia& sistema)
{
    if (sistema.docentes().estaVacia())
    {
        std::cout << "No hay docentes disponibles.\n";
        return nullptr;
    }
    IngresoDatos entrada;
    ValidadorCedula validador;
    while (true)
    {
        sistema.imprimirDocentes();
        const std::string cedula = entrada.leerCampo(
            "Cedula (0 para cancelar): ", "^(0|[0-9]{10})$", "^[0-9]$");
        if (ValidadorCedula::esCancelar(cedula))
            return nullptr;
        if (!validador.validarCedula(cedula))
        {
            std::cout << "Cedula ecuatoriana invalida.\n";
            continue;
        }
        const Docente* docente = sistema.buscarDocente(cedula);
        if (docente != nullptr)
            return docente;
        std::cout << "Docente no encontrado en la lista.\n";
    }
}

const Periodo* seleccionarPeriodo(ControlAsistencia& sistema)
{
    if (sistema.periodos().estaVacia())
    {
        std::cout << "No hay periodos disponibles.\n";
        return nullptr;
    }
    IngresoDatos entrada;
    while (true)
    {
        sistema.imprimirPeriodos();
        const std::string codigo = entrada.leerCampo(
            "Codigo del periodo (0 para cancelar): ",
            "^(0|[A-Za-z0-9-]{3,15})$", "^[A-Za-z0-9-]$");
        if (ValidadorCedula::esCancelar(codigo))
            return nullptr;
        const Periodo* periodo = sistema.buscarPeriodo(codigo);
        if (periodo != nullptr)
            return periodo;
        std::cout << "Periodo no encontrado en la lista.\n";
    }
}

const NRC* seleccionarNRC(ControlAsistencia& sistema)
{
    if (sistema.cursos().estaVacia())
    {
        std::cout << "No hay materias/NRC disponibles.\n";
        return nullptr;
    }
    IngresoDatos entrada;
    while (true)
    {
        sistema.imprimirNRCs();
        const std::string codigo = entrada.leerCampo(
            "Codigo NRC (0 para cancelar): ",
            "^(0|[0-9]{4,6})$", "^[0-9]$");
        if (ValidadorCedula::esCancelar(codigo))
            return nullptr;
        const NRC* nrc = sistema.buscarNRC(codigo);
        if (nrc != nullptr)
            return nrc;
        std::cout << "NRC no encontrado en la lista.\n";
    }
}

const FranjaHoraria* seleccionarFranja(ControlAsistencia& sistema)
{
    if (sistema.franjas().estaVacia())
    {
        std::cout << "No hay franjas disponibles.\n";
        return nullptr;
    }
    IngresoDatos entrada;
    while (true)
    {
        sistema.imprimirFranjas();
        const std::string texto = entrada.leerCampo(
            "ID de franja (0 para cancelar): ",
            "^(0|[1-9][0-9]{0,8})$", "^[0-9]$");
        if (ValidadorCedula::esCancelar(texto))
            return nullptr;
        const FranjaHoraria* franja =
            sistema.buscarFranja(std::stoi(texto));
        if (franja != nullptr)
            return franja;
        std::cout << "Franja no encontrada en la lista.\n";
    }
}

const RegistroAsistencia* seleccionarRegistro(ControlAsistencia& sistema)
{
    if (sistema.registros().estaVacia())
    {
        std::cout << "No hay registros disponibles.\n";
        return nullptr;
    }
    IngresoDatos entrada;
    while (true)
    {
        sistema.imprimirRegistros();
        const std::string id = entrada.leerCampo(
            "ID del registro (0 para cancelar): ",
            "^(0|[0-9]{10}-[0-9]{8}-[0-9]{4,6}-[1-9][0-9]{0,8})$",
            "^[0-9-]$");
        if (ValidadorCedula::esCancelar(id))
            return nullptr;
        const RegistroAsistencia* registro = nullptr;
        sistema.registros().recorrer([&](const RegistroAsistencia& actual) {
            if (actual.getId() == id)
                registro = &actual;
        });
        if (registro != nullptr)
            return registro;
        std::cout << "Registro no encontrado en la lista.\n";
    }
}

FranjaHoraria* seleccionarClaseHoy(ControlAsistencia& sistema,
                                   const Docente& docente)
{
    const Fecha hoy = Fecha::hoy();
    const std::string dia = diaDeLaFecha(hoy);
    if (!ValidadorCedula::validarDiaLaborable(dia))
    {
        std::cout << "Hoy no tiene clases asignadas.\n";
        return nullptr;
    }

    std::size_t cantidad = 0;
    std::cout << "Mis clases de hoy (" << dia << "):\n";
    sistema.franjas().recorrer([&](const FranjaHoraria& franja) {
        const NRC* nrc = sistema.buscarNRC(franja.getCodigoNRC());
        if (franja.getDiaSemana() != dia || nrc == nullptr ||
            nrc->getCedulaDocente() != docente.getCedula())
            return;
        ++cantidad;
        std::cout << cantidad << ". " << nrc->getNombre() << " | NRC "
                  << nrc->getCodigo() << " | "
                  << franja.getHoraInicio().toStringHoraMinuto() << '-'
                  << franja.getHoraFin().toStringHoraMinuto() << '\n';
    });
    if (cantidad == 0)
    {
        std::cout << "Hoy no tiene clases asignadas.\n";
        return nullptr;
    }

    const int seleccion = leerSeleccionLista(cantidad);
    if (seleccion == 0)
        return nullptr;
    std::size_t indice = 0;
    FranjaHoraria* elegida = nullptr;
    sistema.franjas().recorrer([&](FranjaHoraria& franja) {
        const NRC* nrc = sistema.buscarNRC(franja.getCodigoNRC());
        if (franja.getDiaSemana() == dia && nrc != nullptr &&
            nrc->getCedulaDocente() == docente.getCedula())
        {
            ++indice;
            if (indice == static_cast<std::size_t>(seleccion))
                elegida = &franja;
        }
    });
    return elegida;
}

std::string seleccionarRegistroDeHoy(ControlAsistencia& sistema,
                                     const Docente& docente)
{
    const Fecha hoy = Fecha::hoy();
    std::size_t cantidad = 0;
    std::cout << "Mis registros de hoy:\n";
    sistema.registros().recorrer([&](const RegistroAsistencia& registro) {
        if (registro.getCedulaDocente() == docente.getCedula() &&
            registro.getFecha() == hoy)
        {
            ++cantidad;
            std::cout << cantidad << ". " << registro.getId() << " | NRC "
                      << registro.getCodigoNRC() << " | Entrada "
                      << (registro.tieneEntrada() ?
                              registro.getEntrada().toString() : "--:--:--")
                      << " | Salida "
                      << (registro.tieneSalida() ?
                              registro.getSalida().toString() : "--:--:--")
                      << '\n';
        }
    });
    if (cantidad == 0)
        return "";
    const int seleccion = leerSeleccionLista(cantidad);
    if (seleccion == 0)
        return "";
    std::size_t indice = 0;
    std::string idElegido;
    sistema.registros().recorrer([&](const RegistroAsistencia& registro) {
        if (registro.getCedulaDocente() == docente.getCedula() &&
            registro.getFecha() == hoy)
        {
            ++indice;
            if (indice == static_cast<std::size_t>(seleccion))
                idElegido = registro.getId();
        }
    });
    return idElegido;
}

void registrarDocente(ControlAsistencia& sistema)
{
    Docente docente;
    if (!docente.ingresarDatos())
    {
        resultado(true, "Registro cancelado.");
        return;
    }
    const std::string correo = sistema.correoSugerido(docente);
    std::cout << "\nResumen del registro\n"
              << "Cedula: " << docente.getCedula() << '\n'
              << "Nombre: " << docente.nombreCompleto() << '\n'
              << "Correo: " << correo << '\n';
    if (!confirmarAccion("Confirmar el registro"))
    {
        resultado(true, "Registro cancelado.");
        return;
    }
    std::string mensaje;
    resultado(sistema.insertarDocente(docente, mensaje), mensaje);
}

void menuDocentes(ControlAsistencia& sistema)
{
    while (true)
    {
        limpiarPantalla();
        std::cout << "---- DOCENTES ----\n"
                  << "Insertar, buscar, eliminar o consultar docentes.\n"
                  << "1. Insertar docente\n"
                  << "2. Buscar docente\n"
                  << "3. Eliminar docente\n"
                  << "4. Imprimir un docente\n"
                  << "5. Imprimir todos los docentes\n"
                  << "0. Volver\n";
        const int opcion = leerOpcion("Opcion: ", "^[0-5]$");
        if (opcion == 0)
            return;
        if (opcion == 1)
        {
            registrarDocente(sistema);
            continue;
        }
        if (opcion == 5)
        {
            const bool hayDatos = !sistema.docentes().estaVacia();
            sistema.imprimirDocentes();
            resultado(hayDatos, hayDatos ? "Docentes listados." :
                                            "No hay docentes registrados.");
            continue;
        }
        const Docente* docente = seleccionarDocente(sistema);
        if (docente == nullptr)
        {
            resultado(false, "No se selecciono un docente.");
            continue;
        }
        if (opcion == 2 || opcion == 4)
        {
            sistema.imprimirDocente(docente->getCedula());
            resultado(true, opcion == 2 ? "Docente encontrado." :
                                          "Docente impreso.");
        }
        else if (confirmarAccion(
                     "Se eliminaran tambien sus NRC, franjas y asistencias"))
        {
            std::string mensaje;
            resultado(sistema.eliminarDocente(docente->getCedula(), mensaje),
                      mensaje);
        }
        else
            resultado(true, "Eliminacion cancelada.");
    }
}

void menuPeriodos(ControlAsistencia& sistema)
{
    while (true)
    {
        limpiarPantalla();
        std::cout << "---- PERIODOS ACADEMICOS ----\n"
                  << "Administre los periodos del sistema.\n"
                  << "1. Insertar periodo\n"
                  << "2. Buscar periodo\n"
                  << "3. Eliminar periodo\n"
                  << "4. Imprimir un periodo\n"
                  << "5. Imprimir todos los periodos\n"
                  << "0. Volver\n";
        const int opcion = leerOpcion("Opcion: ", "^[0-5]$");
        if (opcion == 0)
            return;
        if (opcion == 1)
        {
            IngresoDatos entrada;
            const std::string codigo = entrada.leerCampo(
                "Codigo del periodo: ", "^[A-Za-z0-9-]{3,15}$",
                "^[A-Za-z0-9-]$");
            const std::string descripcion = entrada.leerCampo(
                "Descripcion (ej. 2026-2): ", "^[0-9]{4}-[1-9]$",
                "^[0-9-]$");
            std::string mensaje;
            resultado(sistema.insertarPeriodo(
                          Periodo(codigo, descripcion), mensaje), mensaje);
            continue;
        }
        if (opcion == 5)
        {
            const bool hayDatos = !sistema.periodos().estaVacia();
            sistema.imprimirPeriodos();
            resultado(hayDatos, hayDatos ? "Periodos listados." :
                                            "No hay periodos registrados.");
            continue;
        }
        const Periodo* periodo = seleccionarPeriodo(sistema);
        if (periodo == nullptr)
        {
            resultado(false, "No se selecciono un periodo.");
            continue;
        }
        if (opcion == 2 || opcion == 4)
        {
            sistema.imprimirPeriodo(periodo->getCodigo());
            resultado(true, opcion == 2 ? "Periodo encontrado." :
                                          "Periodo impreso.");
        }
        else if (confirmarAccion(
                     "Se eliminaran tambien sus NRC, franjas y asistencias"))
        {
            std::string mensaje;
            resultado(sistema.eliminarPeriodo(periodo->getCodigo(), mensaje),
                      mensaje);
        }
        else
            resultado(true, "Eliminacion cancelada.");
    }
}

void menuNRC(ControlAsistencia& sistema)
{
    while (true)
    {
        limpiarPantalla();
        std::cout << "---- MATERIAS (NRC) ----\n"
                  << "Administre materias y su docente/periodo asignados.\n"
                  << "1. Insertar materia/NRC\n"
                  << "2. Buscar NRC\n"
                  << "3. Eliminar NRC\n"
                  << "4. Imprimir un NRC\n"
                  << "5. Imprimir todos los NRC\n"
                  << "0. Volver\n";
        const int opcion = leerOpcion("Opcion: ", "^[0-5]$");
        if (opcion == 0)
            return;
        if (opcion == 1)
        {
            IngresoDatos entrada;
            const std::string codigo = entrada.leerCampo(
                "Codigo NRC: ", "^[0-9]{4,6}$", "^[0-9]$");
            const std::string materia = entrada.leerCampo(
                "Nombre de la materia: ",
                "^[A-Za-zÁÉÍÓÚáéíóúÑñÜü0-9 ]{3,60}$",
                "^[A-Za-zÁÉÍÓÚáéíóúÑñÜü0-9 ]$");
            const Docente* docente = seleccionarDocente(sistema);
            if (docente == nullptr)
            {
                resultado(true, "Insercion cancelada.");
                continue;
            }
            const Periodo* periodo = seleccionarPeriodo(sistema);
            if (periodo == nullptr)
            {
                resultado(true, "Insercion cancelada.");
                continue;
            }
            std::string mensaje;
            resultado(sistema.insertarNRC(
                          NRC(codigo, materia, docente->getCedula(),
                              periodo->getCodigo()), mensaje), mensaje);
            continue;
        }
        if (opcion == 5)
        {
            const bool hayDatos = !sistema.cursos().estaVacia();
            sistema.imprimirNRCs();
            resultado(hayDatos, hayDatos ? "NRC listados." :
                                            "No hay NRC registrados.");
            continue;
        }
        const NRC* nrc = seleccionarNRC(sistema);
        if (nrc == nullptr)
        {
            resultado(false, "No se selecciono un NRC.");
            continue;
        }
        if (opcion == 2 || opcion == 4)
        {
            sistema.imprimirNRC(nrc->getCodigo());
            resultado(true, opcion == 2 ? "NRC encontrado." : "NRC impreso.");
        }
        else if (confirmarAccion(
                     "Se eliminaran tambien sus franjas y asistencias"))
        {
            std::string mensaje;
            resultado(sistema.eliminarNRC(nrc->getCodigo(), mensaje), mensaje);
        }
        else
            resultado(true, "Eliminacion cancelada.");
    }
}

void menuFranjas(ControlAsistencia& sistema)
{
    static const char* dias[] = {
        "lunes", "martes", "miercoles", "jueves", "viernes"};
    static const char* inicios[] = {
        "07:00", "09:00", "11:00", "13:00", "15:00", "17:00", "19:00"};
    static const char* finales[] = {
        "09:00", "11:00", "13:00", "15:00", "17:00", "19:00", "21:00"};
    while (true)
    {
        limpiarPantalla();
        std::cout << "---- FRANJAS HORARIAS ----\n"
                  << "Asigne clases de lunes a viernes en bloques fijos.\n"
                  << "1. Insertar franja\n"
                  << "2. Buscar franja\n"
                  << "3. Eliminar franja\n"
                  << "4. Imprimir una franja\n"
                  << "5. Imprimir todas las franjas\n"
                  << "0. Volver\n";
        const int opcion = leerOpcion("Opcion: ", "^[0-5]$");
        if (opcion == 0)
            return;
        if (opcion == 1)
        {
            const NRC* nrc = seleccionarNRC(sistema);
            if (nrc == nullptr)
            {
                resultado(true, "Insercion cancelada.");
                continue;
            }
            std::cout << "Dia de la clase\n"
                      << "1. Lunes\n2. Martes\n3. Miercoles\n"
                      << "4. Jueves\n5. Viernes\n0. Cancelar\n";
            const int opcionDia = leerOpcion("Opcion (1-5): ", "^[0-5]$");
            if (opcionDia == 0)
            {
                resultado(true, "Insercion cancelada.");
                continue;
            }
            std::cout << "Bloque horario\n"
                      << "1. 07:00-09:00\n2. 09:00-11:00\n"
                      << "3. 11:00-13:00\n4. 13:00-15:00\n"
                      << "5. 15:00-17:00\n6. 17:00-19:00\n"
                      << "7. 19:00-21:00\n0. Cancelar\n";
            const int opcionBloque = leerOpcion("Opcion (1-7): ", "^[0-7]$");
            if (opcionBloque == 0)
            {
                resultado(true, "Insercion cancelada.");
                continue;
            }
            int nuevoId = 1;
            sistema.franjas().recorrer([&nuevoId](const FranjaHoraria& franja) {
                if (franja.getId() >= nuevoId)
                    nuevoId = franja.getId() + 1;
            });
            Hora inicio;
            Hora fin;
            Hora::parsear(inicios[opcionBloque - 1], inicio);
            Hora::parsear(finales[opcionBloque - 1], fin);
            try
            {
                const FranjaHoraria franja(
                    nuevoId, dias[opcionDia - 1], inicio, fin, nrc->getCodigo());
                std::string mensaje;
                resultado(sistema.insertarFranja(franja, mensaje), mensaje);
            }
            catch (const std::invalid_argument& error)
            {
                resultado(false, error.what());
            }
            continue;
        }
        if (opcion == 5)
        {
            const bool hayDatos = !sistema.franjas().estaVacia();
            sistema.imprimirFranjas();
            resultado(hayDatos, hayDatos ? "Franjas listadas." :
                                            "No hay franjas asignadas.");
            continue;
        }
        const FranjaHoraria* franja = seleccionarFranja(sistema);
        if (franja == nullptr)
        {
            resultado(false, "No se selecciono una franja.");
            continue;
        }
        if (opcion == 2 || opcion == 4)
        {
            sistema.imprimirFranja(franja->getId());
            resultado(true, opcion == 2 ? "Franja encontrada." :
                                          "Franja impresa.");
        }
        else if (confirmarAccion(
                     "Se eliminaran tambien las asistencias de esta franja"))
        {
            std::string mensaje;
            resultado(sistema.eliminarFranja(franja->getId(), mensaje), mensaje);
        }
        else
            resultado(true, "Eliminacion cancelada.");
    }
}

void menuAsistencias(ControlAsistencia& sistema)
{
    while (true)
    {
        limpiarPantalla();
        std::cout << "---- ASISTENCIAS ----\n"
                  << "Consulte, imprima o elimine registros de asistencia.\n"
                  << "1. Insertar asistencia (registrar entrada)\n"
                  << "2. Buscar registro\n"
                  << "3. Eliminar registro\n"
                  << "4. Imprimir un registro\n"
                  << "5. Imprimir todos los registros\n"
                  << "6. Consultar por docente\n"
                  << "7. Consultar por fecha\n"
                  << "0. Volver\n";
        const int opcion = leerOpcion("Opcion: ", "^[0-7]$");
        if (opcion == 0)
            return;
        if (opcion == 1)
        {
            const Docente* docente = seleccionarDocente(sistema);
            if (docente == nullptr)
            {
                resultado(false, "No se selecciono un docente.");
                continue;
            }
            FranjaHoraria* franja = seleccionarClaseHoy(sistema, *docente);
            if (franja == nullptr)
            {
                resultado(false, "No se selecciono una clase de hoy.");
                continue;
            }
            const Hora marca = leerHoraManual("Registrar entrada");
            std::string mensaje;
            const bool correcto = sistema.registrarEntrada(
                docente->getCedula(), franja->getCodigoNRC(),
                franja->getId(), Fecha::hoy(), marca, mensaje);
            resultado(correcto, mensaje);
            continue;
        }
        if (opcion == 5)
        {
            const bool hayDatos = !sistema.registros().estaVacia();
            sistema.imprimirRegistros();
            resultado(hayDatos, hayDatos ? "Registros listados." :
                                            "No hay registros de asistencia.");
            continue;
        }
        if (opcion == 6)
        {
            const Docente* docente = seleccionarDocente(sistema);
            if (docente == nullptr)
            {
                resultado(false, "No se selecciono un docente.");
                continue;
            }
            sistema.imprimirHistorial(docente->getCedula());
            resultado(true, "Consulta por docente completada.");
            continue;
        }
        if (opcion == 7)
        {
            sistema.imprimirAsistenciasPorFecha(leerFecha());
            resultado(true, "Consulta por fecha completada.");
            continue;
        }
        const RegistroAsistencia* registro = seleccionarRegistro(sistema);
        if (registro == nullptr)
        {
            resultado(false, "No se selecciono un registro.");
            continue;
        }
        if (opcion == 2 || opcion == 4)
        {
            sistema.imprimirRegistro(registro->getId());
            resultado(true, opcion == 2 ? "Registro encontrado." :
                                          "Registro impreso.");
        }
        else if (confirmarAccion("Confirma eliminar este registro"))
        {
            std::string mensaje;
            resultado(sistema.eliminarRegistro(registro->getId(), mensaje),
                      mensaje);
        }
        else
            resultado(true, "Eliminacion cancelada.");
    }
}

void administrar(ControlAsistencia& sistema)
{
    const char* configurada = std::getenv("ADMIN_PASSWORD");
    const std::string clave = configurada == nullptr ? "admin" : configurada;
    if (configurada == nullptr)
        std::cout << "Aviso: configure ADMIN_PASSWORD para cambiar la clave demo "
                     "(clave actual: admin).\n";
    IngresoDatos entrada;
    if (entrada.leerContrasenia("Contrasena de administracion: ") != clave)
    {
        resultado(false, "Contrasena incorrecta.");
        return;
    }

    while (true)
    {
        limpiarPantalla();
        std::cout << "---- ADMINISTRACION ----\n"
                  << "Orden recomendado: periodo -> docente -> NRC -> franja.\n"
                  << "1. Docentes\n"
                  << "2. Periodos academicos\n"
                  << "3. Materias (NRC)\n"
                  << "4. Franjas horarias\n"
                  << "5. Asistencias\n"
                  << "0. Volver\n";
        const int opcion = leerOpcion("Opcion: ", "^[0-5]$");
        if (opcion == 0)
            return;
        if (opcion == 1)
            menuDocentes(sistema);
        else if (opcion == 2)
            menuPeriodos(sistema);
        else if (opcion == 3)
            menuNRC(sistema);
        else if (opcion == 4)
            menuFranjas(sistema);
        else
            menuAsistencias(sistema);
    }
}

void menuDocente(ControlAsistencia& sistema, Docente& docente)
{
    while (true)
    {
        limpiarPantalla();
        std::cout << "---- DOCENTE: " << docente.nombreCompleto() << " ----\n"
                  << "Consulte horarios y gestione sus marcas de asistencia.\n"
                  << "1. Ver mis materias y horarios\n"
                  << "2. Registrar ENTRADA\n"
                  << "3. Registrar SALIDA\n"
                  << "4. Corregir entrada\n"
                  << "5. Corregir salida\n"
                  << "6. Ver mi historial\n"
                  << "7. Ver asistencias por fecha\n"
                  << "0. Cerrar sesion\n";
        const int opcion = leerOpcion("Opcion: ", "^[0-7]$");
        if (opcion == 0)
            return;
        if (opcion == 1)
        {
            sistema.imprimirHorarios(docente.getCedula());
            resultado(true, "Consulta de horarios completada.");
            continue;
        }
        if (opcion == 6)
        {
            sistema.imprimirHistorial(docente.getCedula());
            resultado(true, "Historial consultado.");
            continue;
        }
        if (opcion == 7)
        {
            sistema.imprimirHistorialPorFecha(
                docente.getCedula(), leerFecha());
            resultado(true, "Consulta por fecha completada.");
            continue;
        }
        std::string mensaje;
        bool correcto = false;
        if (opcion == 2 || opcion == 3)
        {
            FranjaHoraria* franja = seleccionarClaseHoy(sistema, docente);
            if (franja == nullptr)
            {
                resultado(false, "No se selecciono una clase.");
                continue;
            }
            const Hora marca = leerHoraManual(
                opcion == 2 ? "Registrar entrada" : "Registrar salida");
            const Fecha fecha = Fecha::hoy();
            correcto = opcion == 2
                ? sistema.registrarEntrada(
                      docente.getCedula(), franja->getCodigoNRC(),
                      franja->getId(), fecha, marca, mensaje)
                : sistema.registrarSalida(
                      docente.getCedula(), franja->getCodigoNRC(),
                      franja->getId(), fecha, marca, mensaje);
        }
        else
        {
            const std::string id = seleccionarRegistroDeHoy(sistema, docente);
            if (id.empty())
            {
                resultado(false, sistema.registros().estaVacia() ?
                                     "No hay registros de hoy para corregir." :
                                     "No se selecciono un registro de hoy.");
                continue;
            }
            const Hora nueva = leerHoraManual(
                opcion == 4 ? "Corregir entrada" : "Corregir salida");
            const Hora ahora = horaActualSistema();
            correcto = opcion == 4
                ? sistema.corregirEntrada(
                      id, docente.getCedula(), nueva, ahora, mensaje)
                : sistema.corregirSalida(
                      id, docente.getCedula(), nueva, ahora, mensaje);
        }
        resultado(correcto, mensaje);
    }
}

void iniciarSesionDocente(ControlAsistencia& sistema)
{
    std::cout << "---- INICIAR SESION (DOCENTE) ----\n"
              << "Ingrese su cedula o correo institucional y su contrasena.\n"
              << "Si aun no esta registrado, escriba 0 para volver y elija "
                 "\"2. Registrarme como docente\".\n\n";
    IngresoDatos entrada;
    ValidadorCedula validador;
    for (int intento = 1; intento <= 3; ++intento)
    {
        std::string usuario;
        while (true)
        {
            usuario = entrada.leerCampo(
                "Cedula o correo institucional (0 = volver al menu): ",
                "^(0|[0-9]{10}|[A-Za-z0-9._%+-]+@espe\\.edu\\.ec)$",
                "^[A-Za-z0-9._%+@-]$");
            if (ValidadorCedula::esCancelar(usuario))
            {
                std::cout << "Operacion cancelada. Volviendo al menu principal.\n";
                return;
            }
            if (!ValidadorCedula::validarUsuarioOCancelar(usuario))
            {
                std::cout << "Ingrese una cedula de 10 digitos o un correo "
                             "@espe.edu.ec valido.\n";
                continue;
            }
            if (ValidadorCedula::validarCorreo(usuario, "espe.edu.ec") ||
                validador.validarCedula(usuario))
                break;
            std::cout << "Ingrese una cedula valida o un correo "
                         "@espe.edu.ec valido.\n";
        }

        const std::string contrasenia =
            entrada.leerContraseniaOCancelar(
                "Contrasena (0 = volver al menu): ");
        if (ValidadorCedula::esCancelar(contrasenia))
        {
            std::cout << "Operacion cancelada. Volviendo al menu principal.\n";
            return;
        }

        const bool usuarioExiste = sistema.buscarDocente(usuario) != nullptr;
        Docente* docente = sistema.iniciarSesion(usuario, contrasenia);
        if (docente != nullptr)
        {
            menuDocente(sistema, *docente);
            return;
        }

        std::cout << "[ERROR] Credenciales incorrectas (intento " << intento
                  << " de 3).\n"
                  << "Escriba 0 en cualquier campo para volver al menu principal.\n";
        if (!usuarioExiste)
            std::cout << "Si aun no esta registrado, vuelva al menu principal "
                         "(0) y use la opcion 2.\n";
    }
    std::cout << "[ERROR] Demasiados intentos fallidos. Volviendo al menu "
                 "principal.\n";
    pausar();
}
}

int main()
{
    IngresoDatos::instalarProteccionesTerminal();
    try
    {
        ControlAsistencia sistema;
        while (true)
        {
            limpiarPantalla();
            std::cout << "==== CONTROL DE ASISTENCIA DOCENTE ====\n"
                      << "Gestione su asistencia o las asignaciones del sistema.\n"
                      << "1. Soy docente (iniciar sesion)\n"
                      << "2. Registrarme como docente\n"
                      << "3. Administracion\n"
                      << "0. Salir\n";
            const int opcion = leerOpcion("Opcion: ", "^[0-3]$");
            if (opcion == 0)
                break;
            if (opcion == 2)
            {
                registrarDocente(sistema);
                continue;
            }
            if (opcion == 3)
            {
                administrar(sistema);
                continue;
            }

            iniciarSesionDocente(sistema);
        }
    }
    catch (const std::exception& error)
    {
        std::cerr << "Error fatal: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
