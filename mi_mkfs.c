#include "directorios.h"
#define DEBUGN2 1

int mostrarInfoSB();
int test_n4();

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        fprintf(stderr, RED "Error al inicializar la memoria, uso correcto :mi_mkfs <nombre_dispositivo> <nbloques> \n" RESET);
        return FALLO;
    }

    if (bmount(*(argv + 1)) < 0)
    {
        fprintf(stderr, RED "ERROR AL CREAR EL FICHERO\n" RESET);
        return FALLO;
    }

    char *buffer = (char *)malloc(BLOCKSIZE);
    if (buffer == NULL)
    {
        fprintf(stderr, RED "Error al crear el buffer de bytes: %s\n" RESET, strerror(errno));
        return FALLO;
    }

    memset(buffer, 0, BLOCKSIZE);
    int nbloques = atoi(argv[2]);

    for (int i = 0; i < nbloques; i++)
    {
        if (bwrite(i, buffer) < 0)
        {
            return FALLO;
        }
        //fprintf(stdout, "\r" BLUE " Bloque nº %i de %i escrito" RESET, i, nbloques);
    }
    printf("\n");
    free(buffer);

    if (initSB(nbloques, nbloques / 4) < 0)
    {
        return FALLO;
    }
    if (initMB() < 0)
    {
        return FALLO;
    }
    if (initAI() < 0)
    {
        return FALLO;
    }
    int ninodo=reservar_inodo('d', 7); 
    fprintf(stdin,BLUE"Inodo raíz: %d\n"RESET,ninodo);
    if (ninodo< 0)
    {
        fprintf(stderr, "ERROR AL RESERVAR EL DIRECTORIO RAÍZ\n" RESET);
    }
    if (bumount() < 0)
    {
        return FALLO;
    }

    return 0;
}

int mostrarInfoSB()
{
    struct superbloque SB;
    if (bread(POSSB, &SB) < 0)
    {
        fprintf(stderr, RED "ERROR AL LEER SUPER BLOQUE[mostrarInfoSB()]\n" RESET);
        return FALLO;
    }
    fprintf(stdout, BLUE "CANTIDAD DE BLOQUES LIBRES: %d\n" RESET, SB.cantBloquesLibres);
    fprintf(stdout, BLUE "CANTIDAD DE INODOS LIBRES: %d\n" RESET, SB.cantInodosLibres);
    fprintf(stdout, BLUE "POSICION DEL INODO RAIZ: %d\n" RESET, SB.posInodoRaiz);
    fprintf(stdout, BLUE "POSICION DEL PRIMER BLOQUE DEL MAPA DE BITS: %d\n" RESET, SB.posPrimerBloqueMB);
    fprintf(stdout, BLUE "POSICION DEL ULTIMO BLOQUE DEL MAPA DE BITS: %d\n" RESET, SB.posUltimoBloqueMB);
    fprintf(stdout, BLUE "POSICION DEL PRIMER BLOQUE DE LOS INODOS: %d\n" RESET, SB.posPrimerBloqueAI);
    fprintf(stdout, BLUE "POSICION DEL ULTIMO BLOQUE DE LOS INODOS: %d\n" RESET, SB.posUltimoBloqueAI);
    fprintf(stdout, BLUE "POSICION DEL PRIMER BLOQUE DE DATOS: %d\n" RESET, SB.posPrimerBloqueDatos);
    fprintf(stdout, BLUE "POSICION DEL ULTIMO BLOQUE DE DATOS: %d\n" RESET, SB.posUltimoBloqueDatos);
    fprintf(stdout, BLUE "POSICION DEL PRIMER INODO LIBRE: %d\n" RESET, SB.posPrimerInodoLibre);
    fprintf(stdout, BLUE "TOTAL DE BLOQUES: %d\n" RESET, SB.totBloques);
    fprintf(stdout, BLUE "TOTAL DE INODOS: %d\n" RESET, SB.totInodos);
    return EXITO;
}

int test_n4()
{
    unsigned int casos_prueba[5] = {8, 204, 30004, 400004, 468750};

    int n_inodo = reservar_inodo('f', 6);

    // mostrarInfoSB();

    if (n_inodo < 0)
    {
        fprintf(stderr, "(test_n4) Error reservando inodo.\n");
        return FALLO;
    }

    for (int i = 0; i < 5; i++)
    {
        unsigned int bloque = traducir_bloque_inodo(n_inodo, casos_prueba[i], 1);
        if (bloque < 0)
        {
            fprintf(stderr, "(test_n4) Error reservando bloque.\n");
        }
    }

    return EXITO;
}