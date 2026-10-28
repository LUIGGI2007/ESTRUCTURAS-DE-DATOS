#include "Docente.h"
#include "FranjaHoraria.h"
#include "Materia.h"
#include "RegistroMovimientos.h"
#include "Validaciones.h"

#include <iostream>
#include <string>

namespace {
Docente* buscarDocente(Docente** docentes, int cantidad, const std::string& identificador) {
    for (int i = 0; i < cantidad; ++i) {
        if (docentes[i]->getCedula() == identificador
            || docentes[i]->getUsuario() == identificador) {
            return docentes[i];
        }
    }
    return nullptr;
}

Materia* buscarMateria(Materia** materias, int cantidad, int nrc) {
    for (int i = 0; i < cantidad; ++i) {
        if (materias[i]->getNrc() == nrc) {
            return materias[i];
        }
    }
    return nullptr;
}

void anexarDocente(Docente*** docentes, int& cantidad, Docente* docente) {
    Docente** nuevos = new Docente*[cantidad + 1];
    for (int i = 0; i < cantidad; ++i) {
        nuevos[i] = (*docentes)[i];
    }
    nuevos[cantidad] = docente;
    delete[] *docentes;
    *docentes = nuevos;
    ++cantidad;
}

void anexarMateria(Materia*** materias, int& cantidad, Materia* materia) {
    Materia** nuevos = new Materia*[cantidad + 1];
    for (int i = 0; i < cantidad; ++i) {
        nuevos[i] = (*materias)[i];
    }
    nuevos[cantidad] = materia;
    delete[] *materias;
    *materias = nuevos;
    ++cantidad;
}

int leerOpcion() {
    const std::string valor = leerNumerico("Opción: ");
    if (valor.empty()) {
        return -1;
    }
    try {
        return std::stoi(valor);
    } catch (const std::out_of_range&) {
        return -1;
    }
}

void registrarDocente(Docente*** docentes, int& cantidad) {
    std::cout << "\n--- Registro de docente ---\n";
    std::string cedula;
    while (true) {
        cedula = leerNumericoMaximo("Cédula (exactamente 10 dígitos): ", 10);
        if (cedula.size() != 10) {
            std::cout << "La cédula debe contener exactamente 10 dígitos.\n";
        } else if (buscarDocente(*docentes, cantidad, cedula) != nullptr) {
            std::cout << "Ya existe un docente con esa cédula.\n";
        } else {
            break;
        }
    }

    const std::string primerNombre =
        leerTextoSinEspaciosObligatorio("Primer nombre (sin espacios): ");
    const std::string segundoNombre =
        leerTextoObligatorio("Segundo nombre (obligatorio): ");
    const std::string apellido = leerTextoObligatorio("Apellido (obligatorio): ");
    const std::string contrasena = leerContrasena("Contraseña: ");
    if (contrasena.empty()) {
        std::cout << "La contraseña no puede quedar vacía. Registro cancelado.\n";
        return;
    }

    Docente* docente = new Docente(cedula, primerNombre, segundoNombre, apellido,
                                   contrasena);
    unsigned int correlativo = 0;
    std::string usuario = docente->generarUsuario(correlativo);
    while (buscarDocente(*docentes, cantidad, usuario) != nullptr) {
        usuario = docente->generarUsuario(++correlativo);
    }
    docente->setUsuario(usuario);
    anexarDocente(docentes, cantidad, docente);
    std::cout << "Docente registrado. Usuario generado: " << usuario << '\n';
}

void registrarMateria(Materia*** materias, int& cantidad) {
    std::cout << "\n--- Registro de materia ---\n";
    int nrc = 0;
    while (true) {
        const std::string valor = leerNumericoMaximo("NRC (5 dígitos): ", 5);
        if (valor.size() != 5) {
            std::cout << "El NRC debe tener exactamente 5 dígitos.\n";
            continue;
        }
        try {
            nrc = std::stoi(valor);
        } catch (const std::out_of_range&) {
            std::cout << "El NRC está fuera del rango permitido.\n";
            continue;
        }
        if (nrc <= 0) {
            std::cout << "El NRC debe ser mayor que cero.\n";
            continue;
        }
        if (buscarMateria(*materias, cantidad, nrc) != nullptr) {
            std::cout << "Ya existe una materia con ese NRC.\n";
            continue;
        }
        break;
    }

    const std::string nombre = leerTextoObligatorio("Nombre de la materia: ");
    anexarMateria(materias, cantidad, new Materia(nrc, nombre));
    std::cout << "Materia registrada.\n";
}

void listarDocentes(Docente** docentes, int cantidad) {
    std::cout << "\n--- Docentes ---\n";
    if (cantidad == 0) {
        std::cout << "No existen docentes registrados.\n";
        return;
    }
    for (int i = 0; i < cantidad; ++i) {
        std::cout << *docentes[i] << '\n';
    }
}

void listarMaterias(Materia** materias, int cantidad) {
    std::cout << "\n--- Materias ---\n";
    if (cantidad == 0) {
        std::cout << "No existen materias registradas.\n";
        return;
    }
    for (int i = 0; i < cantidad; ++i) {
        std::cout << *materias[i] << '\n';
    }
}

Docente* autenticar(Docente** docentes, int cantidad) {
    const std::string identificador = leerIdentificador("Usuario o cédula: ");
    const std::string contrasena = leerContrasena("Contraseña: ");
    Docente* docente = buscarDocente(docentes, cantidad, identificador);
    if (docente == nullptr || !docente->estaActivo()
        || docente->getContrasena() != contrasena) {
        std::cout << "Usuario/cédula o contraseña incorrectos, o docente inactivo.\n";
        return nullptr;
    }
    return docente;
}

void marcarIngreso(Docente** docentes, int cantidadDocentes, Materia** materias,
                   int cantidadMaterias, RegistroMovimientos& movimientos) {
    std::cout << "\n--- Marcar ingreso ---\n";
    Docente* docente = autenticar(docentes, cantidadDocentes);
    if (docente == nullptr) {
        return;
    }
    if (movimientos.haySesionActiva(docente)) {
        std::cout << "No puede iniciar otra clase mientras tenga una sesión abierta.\n";
        return;
    }

    int segundosDelDia = 0;
    const std::string hora = leerHoraManual(segundosDelDia);
    const int indiceFranja = buscarFranjaDeIngresoSegundos(segundosDelDia);
    if (indiceFranja < 0) {
        std::cout << "Fuera de la ventana de ingreso. Se permite marcar desde 10 "
                     "minutos antes hasta 10 minutos después del inicio de la franja. "
                     "Verifique la hora ingresada.\n";
        return;
    }

    const std::string textoNrc =
        leerNumericoMaximo("NRC de la materia (5 dígitos): ", 5);
    if (textoNrc.size() != 5) {
        std::cout << "El NRC debe tener exactamente 5 dígitos.\n";
        return;
    }
    int nrc = 0;
    try {
        nrc = std::stoi(textoNrc);
    } catch (const std::out_of_range&) {
        std::cout << "El NRC está fuera del rango permitido.\n";
        return;
    }
    Materia* materia = buscarMateria(materias, cantidadMaterias, nrc);
    if (materia == nullptr) {
        std::cout << "No existe una materia con ese NRC.\n";
        return;
    }

    int cantidadFranjas = 0;
    const FranjaHoraria* franjas = FranjaHoraria::obtenerTodas(cantidadFranjas);
    const FranjaHoraria& franja = franjas[indiceFranja];
    const std::string fecha = obtenerFechaActual();
    if (movimientos.hayCruce(docente, fecha, franja.getId())) {
        std::cout << "Conflicto: el docente ya tiene un registro en esta franja "
                     "para la fecha de hoy.\n";
        return;
    }
    if (movimientos.hayClaseAsignada(materia, fecha, franja.getId())) {
        std::cout << "Conflicto: esta materia ya tiene un docente asignado en la "
                     "franja y fecha seleccionadas.\n";
        return;
    }

    if (!movimientos.agregar(docente, materia, franja, fecha, hora)) {
        std::cout << "No se pudo registrar el ingreso.\n";
        return;
    }
    docente->actualizarAsistencia(hora, "");
    std::cout << "Ingreso registrado correctamente en la " << franja << ".\n";
}

void marcarSalida(Docente** docentes, int cantidadDocentes,
                  RegistroMovimientos& movimientos) {
    std::cout << "\n--- Marcar salida ---\n";
    Docente* docente = autenticar(docentes, cantidadDocentes);
    if (docente == nullptr) {
        return;
    }

    RegistroAsistencia* movimiento = movimientos.obtenerSesionActiva(docente);
    if (movimiento == nullptr) {
        std::cout << "El docente no tiene una clase abierta.\n";
        return;
    }
    int segundosDelDia = 0;
    const std::string hora = leerHoraManual(segundosDelDia);
    if (!estaEnVentanaDeSalida(segundosDelDia, movimiento->getFranja())) {
        std::cout << "Hora de salida no válida. Solo puede marcar durante los "
                  << "últimos 10 minutos de la franja ("
                  << movimiento->getFranja().getHoraFin()
                  << " menos 10 minutos hasta " << movimiento->getFranja().getHoraFin()
                  << ").\n";
        return;
    }

    movimiento->cerrar(hora);
    docente->actualizarAsistencia(docente->getHoraEntrada(), hora);
    std::cout << "Salida registrada correctamente a las " << hora << ".\n";
}

void mostrarMenu() {
    std::cout << "\n===== CONTROL DE DOCENTES Y ASISTENCIA =====\n"
              << "1. Registrar docente\n"
              << "2. Registrar materia\n"
              << "3. Listar docentes\n"
              << "4. Listar materias\n"
              << "5. Marcar ingreso\n"
              << "6. Marcar salida\n"
              << "7. Mostrar movimientos\n"
              << "8. Mostrar franjas horarias\n"
              << "ESC cancela la opción elegida y vuelve al menú.\n"
              << "Después de cada acción, presione ENTER para regresar al menú.\n"
              << "0. Salir\n";
}
}

int main() {
    configurarConsolaUTF8();
    Docente** docentes = nullptr;
    int cantidadDocentes = 0;
    Materia** materias = nullptr;
    int cantidadMaterias = 0;
    RegistroMovimientos movimientos;
    bool continuar = true;

    while (continuar) {
        limpiarConsola();
        mostrarMenu();
        const int opcion = leerOpcion();
        if (opcion >= 1 && opcion <= 8 && !confirmarOpcion()) {
            continue;
        }
        switch (opcion) {
        case 1:
            registrarDocente(&docentes, cantidadDocentes);
            break;
        case 2:
            registrarMateria(&materias, cantidadMaterias);
            break;
        case 3:
            listarDocentes(docentes, cantidadDocentes);
            break;
        case 4:
            listarMaterias(materias, cantidadMaterias);
            break;
        case 5:
            marcarIngreso(docentes, cantidadDocentes, materias, cantidadMaterias,
                          movimientos);
            break;
        case 6:
            marcarSalida(docentes, cantidadDocentes, movimientos);
            break;
        case 7:
            std::cout << "\n--- Registro de movimientos ---\n";
            movimientos.imprimir(std::cout);
            break;
        case 8: {
            int cantidadFranjas = 0;
            const FranjaHoraria* franjas = FranjaHoraria::obtenerTodas(cantidadFranjas);
            for (int i = 0; i < cantidadFranjas; ++i) {
                std::cout << franjas[i] << '\n';
            }
            break;
        }
        case 0:
            continuar = false;
            break;
        default:
            std::cout << "Opción no válida.\n";
            break;
        }
        if (continuar) {
            esperarRetornoMenu();
        }
    }

    for (int i = 0; i < cantidadDocentes; ++i) {
        delete docentes[i];
    }
    for (int i = 0; i < cantidadMaterias; ++i) {
        delete materias[i];
    }
    delete[] docentes;
    delete[] materias;
    std::cout << "Programa finalizado.\n";
    return 0;
}
