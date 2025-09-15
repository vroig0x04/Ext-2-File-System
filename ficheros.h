// ficheros.h
#include <time.h>
#include "ficheros_basico.h"
#define DEBUGN8 0
struct STAT
{ // definir el struct STAT como inodo
    unsigned char tipo;
    unsigned char permisos;

    unsigned char alineacion[6]; // no considerar

    time_t atime;
    time_t mtime;
    time_t ctime;
    time_t btime;

    unsigned int nlinks;
    unsigned int tamEnBytesLog;
    unsigned int numBloquesOcupados;
};

int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes);
int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes);
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat);
int mi_chmod_f(unsigned int ninodo, unsigned char permisos);
int mi_truncar_f(unsigned int ninodo, unsigned int nbytes);