#include "ficheros.h"
int main(int argc, char *argv[]){
    if (argc!=4) {
       fprintf(stderr, RED "Sintaxis: escribir <nombre_dispositivo> <\"$(cat fichero)\"> <diferentes_inodos>\n");
       fprintf(stderr, "Offsets: 9000, 209000, 30725000, 409605000, 480000000\n");
       fprintf(stderr, "Si diferentes_inodos=0 se reserva un solo inodo para todos los offsets\n" RESET);
       exit(-1);
    }
 
    unsigned int offset[5]={9000, 209000, 30725000, 409605000, 480000000};
    int diferentes_inodos=atoi(argv[3]);
    int ninodo;
    struct STAT stat;
 
    //montamos el dispositivo y comprobamos que está formateado
    if (bmount(argv[1]) < 0)
    {
       fprintf(stderr, RED "Error en el nombre del dispositivo [leer.c]\n" RESET);
       return FALLO;
    }
 
    //obtenemos el texto y su longitud
    char *buffer_texto = argv[2];
    int longitud=strlen(buffer_texto);
    fprintf(stderr, ROSE "longitud texto: %d\n" RESET, longitud);
 
    for (int i=0; i<5; i++) { //iterar para cada offset
       if (diferentes_inodos || i==0) ninodo=reservar_inodo('f',6);
       fprintf (stderr, ROSE "\nNº inodo reservado: %d\n" RESET,ninodo);
       fprintf(stderr, ROSE "offset: %d\n" RESET, offset[i]);
       int escritos = mi_write_f(ninodo, buffer_texto, offset[i], longitud);
       fprintf(stderr, ROSE "Bytes escritos: %d\n" RESET, escritos);
 
       /* Visualización del stat */
       mi_stat_f(ninodo, &stat);
 
       printf("stat.tamEnBytesLog=%d\n",stat.tamEnBytesLog);
       printf("stat.numBloquesOcupados=%d\n",stat.numBloquesOcupados);
    }
 
    bumount();
 }
