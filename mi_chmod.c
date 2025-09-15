#include "directorios.h"

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Sintaxis: ./mi_chmod <disco> <permisos> </ruta> \n");
        return FALLO;
    }
    unsigned char permisos =(unsigned char) atoi(argv[2]);
    const char *entrada = argv[3];
    if (permisos < 0 || permisos > 7)
    {
        fprintf(stderr, RED "[mi_mkdir] --> Permisos invalidos \n" RESET);
        return FALLO;
    }
    if (bmount(argv[1]) == FALLO)
    {
        fprintf(stderr, "Error al montar el disco %s\n"RESET, argv[1] );
        return FALLO;
    }
    mi_chmod(entrada,permisos);
    
    bumount();
    return EXITO;
}