#include "directorios.h"
#define TAMFILA 100
#define TAMBUFFER (TAMFILA * 1000)


int main(int argc, char *argv[])
{
    if (argc < 3 || (strcmp(argv[1], "-l") == 0 && argc < 4))
    {
        fprintf(stderr, RED "ERROR, USO CORRECTO: ./mi_ls <disco> </ruta> o ./mi_ls -l <disco> </ruta>\n" RESET);
        return FALLO;
    }

    char flag = '\0';
    char *nombre_disco = argv[1];
    char *camino = argv[2];
    
    if (strcmp(argv[1], "-l") == 0)
    {
        flag = 'l';
        nombre_disco = argv[2];
        camino = argv[3];
    }
    if (bmount(nombre_disco) < 0)
    {
        fprintf(stderr, RED "[mi_ls.c] --> Error al montar el disco \n" RESET);
        return FALLO;
    }

    char buffer[TAMBUFFER];
    memset(buffer, 0, TAMBUFFER);
    int numentradas=mi_dir(camino, buffer,flag);
    if(numentradas<0){
        mostrar_error_buscar_entrada(numentradas);
    }
    fflush(stdout);
    fprintf(stdout, "%s\n", buffer);
    fprintf(stdout,"Numero de directorios: %d\n",numentradas);
    fflush(stdout);
    

    if(bumount()<0)
    {
        fprintf(stderr,RED "ERROR AL DESMONTAR EL DISCO\n" RESET);
        return FALLO;
    }
    return EXITO;
}