#include "ficheros.h"
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, RED "Error: Uso -> ./leer.c <nombre_dispositivo> <ninodo> \n" RESET);
        return FALLO;
    }

    unsigned int ninodo = atoi(argv[2]);
    if (bmount(argv[1]) < 0)
    {
        fprintf(stderr, RED "Error en el nombre del dispositivo [leer.c]\n" RESET);
        return FALLO;
    }

    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo) < 0)
    {
        fprintf(stderr, RED "Error al leer el inodo [leer.c]\n" RESET);
        bumount();
        return FALLO;
    }

    if ((inodo.permisos & 4) != 4)
    {
        fprintf(stderr, RED "No hay permisos de lectura [leer.c]\n" RESET);
        bumount();
        return FALLO;
    }

    unsigned char *buffer = malloc(BLOCKSIZE);
    if (buffer == NULL)
    {
        fprintf(stderr, RED "Error: No se pudo reservar memoria para el buffer [leer.c]\n" RESET);
        bumount();
        return FALLO;
    }

    unsigned int offset = 0;
    int bytes_leidos;
    int total_bytes = 0;
    memset(buffer, 0, BLOCKSIZE);

    bytes_leidos = mi_read_f(ninodo, buffer, offset, BLOCKSIZE);
    total_bytes += bytes_leidos;
    while (bytes_leidos > 0)
    {
        if (write(1, buffer, bytes_leidos) < 0)
        {
            fprintf(stderr, RED "Error al escribir en stdout [leer.c]\n" RESET);
            free(buffer);
            bumount();
            return FALLO;
        }
        memset(buffer, 0, BLOCKSIZE);
        offset += BLOCKSIZE;
        bytes_leidos = mi_read_f(ninodo, buffer, offset, BLOCKSIZE);
        total_bytes += bytes_leidos;
        if (bytes_leidos < 0)
        {
            fprintf(stderr, RED "Error al leer del inodo [leer.c]\n" RESET);
            free(buffer);
            bumount();
            return FALLO;
        }
    }
    fprintf(stderr,"\nTotal de bytes leidos: %d\n", total_bytes);
    free(buffer);
    
    if (bumount() < 0)
    {
        fprintf(stderr, RED "Error al desmontar el dispositivo [leer.c]\n" RESET);
        return FALLO;
    }

    return EXITO;
}