#include "directorios.h"

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        fprintf(stderr, "Sintaxis: mi_stat.c <dsco> <ruta> \n");
        return FALLO;
    }


    if (bmount(argv[1]) == FALLO) {
        fprintf(stderr, RED "Error al montar el dispositivo en mi_stat()\n" RESET);
        return  FALLO;
    }

    struct STAT stat;
    const char *ruta = argv[2];
    int resultado = mi_stat(ruta, &stat);
    
    if (resultado < 0) {
        fprintf(stderr, RED "Error al obtener información del inodo para la ruta %s\n" RESET, ruta);
        bumount();
        return FALLO;
    }
    
    printf("Nº de inodo: %d \n", resultado);
    printf("tipo: %c \n", stat.tipo);
    printf("permisos: %d \n", stat.permisos);
    
    char fecha[80];
    struct tm *tm;

    tm = localtime(&stat.atime);
    strftime(fecha, sizeof(fecha), "%c", tm);
    printf("atime: %s\n", fecha);

    tm = localtime(&stat.mtime);
    strftime(fecha, sizeof(fecha), "%c", tm);
    printf("mtime: %s\n", fecha);

    tm = localtime(&stat.ctime);
    strftime(fecha, sizeof(fecha), "%c", tm);
    printf("ctime: %s\n", fecha);

    tm = localtime(&stat.btime);
    strftime(fecha, sizeof(fecha), "%c", tm);
    printf("btime: %s\n", fecha);
    
    printf("nlinks: %d \n", stat.nlinks);
    printf("Tamaño en bytes lógicos: %d\n", stat.tamEnBytesLog);
    printf("Número de bloques ocupados: %d\n", stat.numBloquesOcupados);

    if (bumount() == FALLO) {
        fprintf(stderr, RED "Error al desmontar el dispositivo en mi_stat()\n" RESET);
        return FALLO;
    }
    return EXITO;
}