#include "directorios.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int eliminar_recursivo(const char *ruta) {
    struct STAT stat;
    if (mi_stat(ruta, &stat) == FALLO) {
        fprintf(stderr, RED "Error al obtener información de %s\n" RESET, ruta);
        return FALLO;
    }


    if (stat.tipo == 'd') {
        struct entrada entradas[BLOCKSIZE / sizeof(struct entrada)];
        int total_entradas = mi_read(ruta, entradas, 0, sizeof(entradas)) / sizeof(struct entrada);

        if (total_entradas < 0) {
            fprintf(stderr, RED "Error al leer el contenido del directorio %s\n" RESET, ruta);
            return FALLO;
        }

        for (int i = 0; i < total_entradas; i++) {
            if (strcmp(entradas[i].nombre, ".") != 0 && strcmp(entradas[i].nombre, "..") != 0) {
                char ruta_completa[1024];
                snprintf(ruta_completa, sizeof(ruta_completa), "%s/%s", ruta, entradas[i].nombre);

                if (eliminar_recursivo(ruta_completa) == FALLO) {
                    return FALLO;
                }
            }
        }
    }

    if (mi_unlink(ruta) == FALLO) {
        fprintf(stderr, RED "Error al eliminar %s\n" RESET, ruta);
        return FALLO;
    }

    return EXITO;
}


int main(int argc, char *argv[])  {
     if (argc < 3) {
        fprintf(stderr, RED "Error de uso: disco /ruta\n" RESET);
        return FALLO;
    }

    char *disco = argv[1];
    char *ruta = argv[2];

    if (strcmp(ruta, "/") == 0) {
        fprintf(stderr, RED "Error: No se puede eliminar el directorio raíz\n" RESET);
        return FALLO;
    }

    if (bmount(disco) == FALLO) {
        fprintf(stderr, RED "Error al montar el disco\n" RESET);
        return FALLO;
    }

    if (eliminar_recursivo(ruta) == FALLO) {
        bumount();
        return FALLO;
    }

    if (bumount() == FALLO) {
        fprintf(stderr, RED "Error al desmontar el disco\n" RESET);
        return FALLO;
    }

    return EXITO;
}
    