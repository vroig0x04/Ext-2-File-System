#include "directorios.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    
    if (argc < 3) {
        fprintf(stderr, "Error de uso: disco /ruta\n" RESET);
        return FALLO;
    }

    char *disco = argv[1];
    char *ruta = argv[2];
        
    if ((strlen(ruta)==2)&& *ruta== '/') {
        fprintf(stderr, "Error: No se puede eliminar el directorio raíz\n" RESET);
        return FALLO;
    }

    if (bmount(disco) == FALLO) {
        fprintf(stderr, "Error al montar el disco en [mi_rm.c]\n" RESET);
        return FALLO;
    }

    if (mi_unlink(ruta) == FALLO) {
        fprintf(stderr, "Error al eliminar el archivo/directorio %s\n"RESET ,ruta );
        bumount();
        return FALLO;
    }

    if (bumount() == FALLO) {
        fprintf(stderr, "Error al desmontar el disco en [mi_rm.c]\n" RESET);
        return FALLO;
    }

    return EXITO;
}