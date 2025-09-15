
#include "directorios.h"

int main(int argc, char *argv[]) {

    if (argc < 4) {
        fprintf(stderr, RED "Error de uso: disco /ruta_fichero_original /ruta_enlace\n" RESET);
        return FALLO;   
    }

    char *disco = argv[1];
    char *ruta_fichero_original = argv[2];
    char *ruta_enlace = argv[3];
    
    int txtlength=strlen(ruta_fichero_original);
    if(*(ruta_fichero_original+txtlength-1)=='/'){
        fprintf(stderr,RED"ERROR, SOLO SE ENLAZAN FICHEROS\n"RESET);
        return FALLO;
    }
    if (bmount(disco) == FALLO) {
        fprintf(stderr, RED "Error al montar el disco %s\n en [mi_link.c]" RESET, disco);
        return FALLO;
    }
    if (mi_link(ruta_fichero_original, ruta_enlace) == FALLO) {
        fprintf(stderr, RED "Error al crear el enlace %s o %s no existe\n en [mi_link.c]" RESET, ruta_enlace, ruta_enlace);
        bumount();
        return FALLO;   
    }

    if (bumount() == FALLO) {
        fprintf(stderr, RED "Error al desmontar el disco %s\n en [mi_link.c]\n" RESET,disco );
        return FALLO;   
    }
    
    return EXITO;
}  

