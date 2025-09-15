#include "directorios.h"

int main(int argc, char *argv[])
{
    if(argc != 4)
    {
        fprintf(stderr,RED "[mi_mkdir] --> Error de sintaxis: ./mi_mkdir <disco> <permisos> </ruta> \n" RESET);
        return FALLO;
    }
    char *nombre_disco = argv[1];
    int permisos = atoi(argv[2]);
    char *camino = argv[3];

    if(permisos < 0 || permisos > 7)
    {
        fprintf(stderr,RED "[mi_mkdir] --> Permisos invalidos \n" RESET);
        return FALLO;
    }
    if(bmount(nombre_disco)<0){
        fprintf(stderr,RED"[mi_mkdir]--> ERROR AL MONTAR EEL DISCO");
        return FALLO;
    }
    
    int error = mi_creat(camino, permisos);
    if(error < 0)
    {
        mostrar_error_buscar_entrada(error);
        return FALLO;
    }
    bumount();
    return EXITO;
}