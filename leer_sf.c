#include "directorios.h"
void mostrar_buscar_entrada(char *camino, char reservar)
{
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;
    printf("\ncamino: %s, reservar: %d\n", camino, reservar);
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, reservar, 6)) < 0)
    {
        mostrar_error_buscar_entrada(error);
    }
    printf("**********************************************************************\n");
    return;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, RED "Error: Uso  -> ./leer_sf  <nombre_dispositivo>\n");
        return FALLO;
    }
    if (bmount(argv[1]) < 0)
    {
        fprintf(stderr, RED "Error al montar el dispositivo en <leer_sf>\n" RESET);
    }
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

    /*
    mostrar_buscar_entrada("pruebas/", 1);           // ERROR_CAMINO_INCORRECTO
    mostrar_buscar_entrada("/pruebas/", 0);          // ERROR_NO_EXISTE_ENTRADA_CONSULTA
    mostrar_buscar_entrada("/pruebas/docs/", 1);     // ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO
    mostrar_buscar_entrada("/pruebas/", 1);          // creamos /pruebas/
    mostrar_buscar_entrada("/pruebas/docs/", 1);     // creamos /pruebas/docs/
    mostrar_buscar_entrada("/pruebas/docs/doc1", 1); // creamos /pruebas/docs/doc1
    mostrar_buscar_entrada("/pruebas/docs/doc1/doc11", 1);
    // ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO
    mostrar_buscar_entrada("/pruebas/", 1);          // ERROR_ENTRADA_YA_EXISTENTE
    mostrar_buscar_entrada("/pruebas/docs/doc1", 0); // consultamos /pruebas/docs/doc1
    mostrar_buscar_entrada("/pruebas/docs/doc1", 1); // ERROR_ENTRADA_YA_EXISTENTE
    mostrar_buscar_entrada("/pruebas/casos/", 1);    // creamos /pruebas/casos/
    mostrar_buscar_entrada("/pruebas/docs/doc2", 1); // creamos /pruebas/docs/doc2
    */
    
    return EXITO;
}