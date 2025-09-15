#include "ficheros.h"
int main(int argc, char *argv[]) {
    if(argc<4){
        fprintf(stderr, RED "Error, uso correcto :./permitir <nombre_dispositivo> <ninodo> <permisos> \n" RESET);
        return FALLO;
    }
    if(bmount(argv[1])<0){
        fprintf(stderr,RED"ERROR AL MONTAR EL SISTEMA DE ARCHIVOS\n"RESET);
        return FALLO;
    }
    int ninodo=atoi(argv[2]);
    unsigned char permisos = (unsigned char)atoi(argv[3]);
    mi_chmod_f(ninodo,permisos);
    if(bumount()<0)
    {
        return FALLO;
    }


    return 0;

}