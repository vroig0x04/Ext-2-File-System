#include "bloques.h"
#include "semaforo_mutex_posix.h"

int static descriptor;
static sem_t *mutex;
static unsigned int inside_sc = 0;

void mi_waitSem() {
    if (inside_sc == 0) {
        waitSem(mutex);
    }
    inside_sc++;
}

void mi_signalSem() {
    inside_sc--;
    if (!inside_sc) {
        signalSem(mutex);
    }
}
int bmount(const char *camino){
    if(!mutex)
    {
        mutex = initSem();
        if(mutex == NULL)
        {
            return FALLO;
        }
    }
    descriptor=open(camino,O_RDWR|O_CREAT,0666);
    if(descriptor < 0){
        fprintf(stderr,RED "Error al abrir el archivo %s (bmount):%s \n" RESET, camino,strerror(errno));
        return FALLO;
    }
    return  descriptor;
}

int bumount(){
    deleteSem();
    descriptor = close(descriptor);
    if(descriptor <0){
        fprintf(stderr,RED "Error al cerrar el archivo (bumount):%s \n" RESET, strerror(errno));
        return FALLO;
    }
    return EXITO;
}

int bwrite(unsigned int nbloque, const void *buf){
   unsigned int position =(nbloque*BLOCKSIZE);
    if(lseek(descriptor,position,SEEK_SET)<0){        
    fprintf(stderr, RED "Error al buscar la posición %i en el archivo (bwrite):%s \n" RESET,position,strerror(errno));
    }
    size_t bytes = write(descriptor, buf, BLOCKSIZE);
    if (bytes < 0)
    {
        fprintf(stderr,RED "Error al escribir el bloque de bytes (bwrite): %s\n"RESET , strerror(errno));
        return FALLO;
    }
    return bytes; 
}

int bread(unsigned int nbloque, void *buf) {
    unsigned int position =(nbloque*BLOCKSIZE);
    if (lseek(descriptor, position, SEEK_SET) < 0) {
        fprintf(stderr, RED "Error al buscar la posición %i en el archivo (bread):%s \n" RESET,position,strerror(errno));
    }
    size_t bytes = read(descriptor, buf, BLOCKSIZE);
    if (bytes < 0)
    {
        fprintf(stderr,RED "Error al leer el bloque de bytes (bread): %s\n" RESET, strerror(errno));
        return FALLO;
    }
    return bytes;    
}
