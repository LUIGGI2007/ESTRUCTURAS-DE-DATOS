#ifndef INGRESODATOS_H
#define INGRESODATOS_H

#include <string>

class IngresoDatos
{
public:
    std::string leerSoloNumeros() const;
    std::string leerSoloNumeros(const std::string& etiqueta,
                                const std::string& patron) const;
    std::string leerSoloLetras() const;
    std::string leerCorreo(const std::string& dominio) const;
    std::string leerHora() const;
    std::string leerTexto(const std::string& etiqueta) const;
    std::string leerContrasenia(const std::string& etiqueta) const;
    std::string leerContraseniaOCancelar(const std::string& etiqueta) const;
    std::string leerCodigo(const std::string& etiqueta) const;
    std::string leerTextoLibre(const std::string& etiqueta,
                               const std::string& patron,
                               const std::string& patronTecla,
                               bool ocultar = false,
                               bool opcional = false) const;
    std::string leerCampo(const std::string& etiqueta,
                          const std::string& patron,
                          const std::string& patronTecla,
                          bool ocultar = false,
                          bool opcional = false) const;
    static void instalarProteccionesTerminal();
    static bool validarNombre(const std::string& valor);
    static bool validarCorreoInstitucional(const std::string& valor,
                                            const std::string& dominio);
    static bool validarHoraFormato(const std::string& valor);
};

#endif
