#include "directorios.h"

int main(int argc, char *argv[]) {

    if (argc != 5) {
        fprintf(stderr, RED "Uso: ./mi_escribir <nombre_dispositivo> <ruta_fichero> <texto> <offset>\n" RESET);
        return FALLO;
    }

    if (bmount(argv[1]) == FALLO) {
        fprintf(stderr, RED "Error al montar el dispositivo en mi_escribir()\n" RESET);
        return FALLO;
    }

    const char *ruta = argv[2];
    const char *texto = argv[3];
    unsigned int offset = atoi(argv[4]);
    unsigned int nbytes = strlen(texto);

    int bytes_escritos = mi_write(ruta, texto, offset, nbytes);
    if (bytes_escritos < 0) {
        fprintf(stderr, RED "Error al escribir en el fichero: %s\n" RESET, ruta);
        bumount();
        return FALLO;
    }

    printf("Se han escrito %d bytes\n",bytes_escritos);
    if (bumount() == FALLO) {
        fprintf(stderr, RED "Error al desmontar el dispositivo en mi_escribir()\n" RESET);
        return FALLO;
    }
    return EXITO;

}