#ifndef VALIDACIONES_H
#define VALIDACIONES_H

#include <cstddef>
#include <string>

class FranjaHoraria;

void configurarConsolaUTF8();
void limpiarConsola();
std::string leerTexto(const std::string& mensaje);
std::string leerTextoObligatorio(const std::string& mensaje);
std::string leerTextoSinEspaciosObligatorio(const std::string& mensaje);
std::string leerIdentificador(const std::string& mensaje);
std::string leerNumerico(const std::string& mensaje);
std::string leerNumericoMaximo(const std::string& mensaje, std::size_t maximo);
std::string leerContrasena(const std::string& mensaje);
bool confirmarOpcion();
void esperarRetornoMenu();
std::string leerHoraManual(int& segundosDelDia);
std::string obtenerFechaActual();
std::string obtenerHoraActual();
int obtenerMinutosActuales();
int buscarFranjaDeIngreso(int minutosActuales);
int buscarFranjaDeIngresoSegundos(int segundosDelDia);
bool estaEnVentanaDeSalida(int segundosDelDia, const FranjaHoraria& franja);

#endif
