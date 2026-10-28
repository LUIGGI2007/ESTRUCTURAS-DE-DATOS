#ifndef VALIDACIONES_H
#define VALIDACIONES_H

#include <string>

void configurarConsolaUTF8();
std::string leerTexto(const std::string& mensaje);
std::string leerTextoObligatorio(const std::string& mensaje);
std::string leerIdentificador(const std::string& mensaje);
std::string leerNumerico(const std::string& mensaje);
std::string leerContrasena(const std::string& mensaje);
std::string obtenerFechaActual();
std::string obtenerHoraActual();
int obtenerMinutosActuales();
int buscarFranjaDeIngreso(int minutosActuales);

#endif
