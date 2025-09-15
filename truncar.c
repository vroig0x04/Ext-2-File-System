#include "ficheros.h"
int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        fprintf(stderr, RED "ERROR DE SINTAXIS. SINTAXIS CORRECTA--> truncar <nombre_dispositivo> <ninodo> <nbytes>" RESET);
        return FALLO;
    }
    if (bmount(argv[1]) < 0)
    {
        fprintf(stderr, RED "ERROR AL LEER EL DISPOSITIVO" RESET);
        return FALLO;
    }
    int ninodo = atoi(argv[2]);
    int nbytes = atoi(argv[3]);
    if (nbytes < 0 || ninodo < 0)
    {
        fprintf(stderr, RED "ERROR AL LEER INODO. SINTAXIS CORRECTA -->truncar <nombre_dispositivo> <ninodo> <nbytes>");
    }
    else if (nbytes == 0)
    {

        liberar_inodo(ninodo);
    }
    else
    {
        mi_truncar_f(ninodo, nbytes);
    }
    struct inodo inodo;
    leer_inodo(ninodo,&inodo);
    ver_inodo(&inodo);
    return 0;
}