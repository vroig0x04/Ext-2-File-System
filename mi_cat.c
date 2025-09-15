#include "directorios.h"
#define TAMBUF BLOCKSIZE
int main(int argc, char const *argv[])
{
    if(argc!= 3){
        fprintf(stderr,RED"ERROR SYNTAXIS, ./mi_cat <nombre dispositivo> </ruta>\n"RESET);
    }
    const char *nombre_disco = argv[1];
    const char *camino = argv[2];

    if (bmount(nombre_disco) < 0) {
        fprintf(stderr, RED "ERROR AL MONTAR EL DISCO en [mi_cat]\n" RESET);
        return FALLO;
    }

    int slength=strlen(camino);
    if(*(camino+(slength-1))=='/'){
        fprintf(stderr,RED"ERROR, NO SE HA INTRODUCIDO UN FICHERO\n"RESET);
    }
    
    unsigned char *buffer = malloc(TAMBUF);
    if (buffer == NULL)
    {
        fprintf(stderr, RED "Error: No se pudo reservar memoria para el buffer [leer.c]\n" RESET);
        bumount();
        return FALLO;
    }
    unsigned int offset = 0;
    int bytes_leidos;
    int total_bytes = 0;
    memset(buffer, 0, TAMBUF);

    bytes_leidos = mi_read(camino, buffer, offset, TAMBUF);
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
        memset(buffer, 0, TAMBUF);
        offset += TAMBUF;
        bytes_leidos = mi_read(camino, buffer, offset, TAMBUF);
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
