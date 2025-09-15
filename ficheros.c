#include "ficheros.h"
int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes)
{
    struct inodo inodo;
    int bytes_escritos = 0;
    if (leer_inodo(ninodo, &inodo) == FALLO)
    {
        fprintf(stderr, RED "[mi_write_f]-->No se pudo leer el inodo\n" RESET);
        return FALLO;
    }
    if ((inodo.permisos & 2) != 2)
    {
        fprintf(stderr, RED "[mi_write_f]-->No hay permisos de escritura\n" RESET);
        return FALLO;
    }
    unsigned int primerBL = offset / BLOCKSIZE;
    unsigned int ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;
    unsigned int desp1 = offset % BLOCKSIZE;
    unsigned int desp2 = (offset + nbytes - 1) % BLOCKSIZE;
    if (primerBL == ultimoBL)
    {
        unsigned int nbfisico = traducir_bloque_inodo(ninodo, primerBL, 1);
        char bufbloque[BLOCKSIZE];
        if (bread(nbfisico, bufbloque) < 0)
        {
            fprintf(stderr, RED "[mi_write_f]-->No se pudo leer bufbloque (caso 1)\n" RESET);
            return FALLO;
        }
        bytes_escritos += nbytes;
        memcpy(bufbloque + desp1, buf_original, nbytes); 
        if (bwrite(nbfisico, bufbloque) < 0)
        {
            fprintf(stderr, RED "[mi_write_f]-->No se pudo escribir bufbloque (caso 1)\n" RESET);
            return FALLO;
        }
    }
    else
    {
        unsigned int nbfisico = traducir_bloque_inodo(ninodo, primerBL, 1);
        char bufbloque[BLOCKSIZE];
        if (bread(nbfisico, bufbloque) < 0)
        {
            fprintf(stderr, RED "[mi_write_f]-->No se pudo leer bufbloque (caso 2)\n" RESET);
            return FALLO;
        }
        memcpy(bufbloque + desp1, buf_original, BLOCKSIZE - desp1);
        bytes_escritos += BLOCKSIZE - desp1;
        bwrite(nbfisico, bufbloque);


        if (bytes_escritos < 0)
        {
            fprintf(stderr, RED "[mi_write_f]-->No se pudo escribir bufbloque\n" RESET);
            return FALLO;
        }
        for (int bl = primerBL + 1; bl < ultimoBL; bl++)
        {
            nbfisico = traducir_bloque_inodo(ninodo, bl, 1);
            bytes_escritos += BLOCKSIZE;
            bwrite(nbfisico, buf_original + (BLOCKSIZE - desp1) + (bl - primerBL - 1) * BLOCKSIZE);
        }
        nbfisico = traducir_bloque_inodo(ninodo, ultimoBL, 1);
        if (nbfisico < 0)
        {
            return bytes_escritos;
        }
        if (bread(nbfisico, bufbloque) < 0)
        {
            fprintf(stderr, RED "[mi_write_f]-->No se pudo escribir bufbloque (caso 2, escritura de bloque parcial )\n" RESET);
            return FALLO;
        }
        memcpy(bufbloque, buf_original + (nbytes - (desp2 + 1)), desp2 + 1); 
        bytes_escritos += desp2 + 1;
        bwrite(nbfisico, bufbloque);
        if (bytes_escritos < 0)
        {
            return FALLO;
        }
    }

    if (bytes_escritos > 0)
    {
        if (leer_inodo(ninodo, &inodo) == FALLO)
        {
            fprintf(stderr, RED "[mi_write_f]-->No se pudo leer el inodo\n" RESET);
            return FALLO;
        }
        if (inodo.tamEnBytesLog < offset + bytes_escritos) 
        {
            inodo.tamEnBytesLog = offset + bytes_escritos; 
        }

        inodo.mtime = time(NULL);
        inodo.ctime = time(NULL);
        if (escribir_inodo(ninodo, &inodo) < 0)
        {
            fprintf(stderr, RED "[mi_write_f]-->No se pudo escribir el inodo\n" RESET);
            return FALLO;
        }
    }

    return bytes_escritos;
}

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes)
{
    struct inodo inodo;
    int bytes_leidos = 0;

    if (leer_inodo(ninodo, &inodo) == FALLO)
    {
        fprintf(stderr, RED "[mi_read_f]-->No se pudo leer el inodo\n" RESET);
        return FALLO;
    }

    if ((inodo.permisos & 4) != 4)
    {
        fprintf(stderr, RED "No hay permisos de lectura\n" RESET);
        return FALLO;
    }

    if (offset >= inodo.tamEnBytesLog)
    {
        return 0;
    }

    if ((offset + nbytes) > inodo.tamEnBytesLog)
    {
        nbytes = inodo.tamEnBytesLog - offset;
    }

    unsigned int primerBL = offset / BLOCKSIZE;
    unsigned int ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;
    unsigned int desp1 = offset % BLOCKSIZE;
    unsigned int desp2 = (offset + nbytes - 1) % BLOCKSIZE;
    char buf_bloque[BLOCKSIZE];

    if (primerBL == ultimoBL)
    {
        int nbfisico = traducir_bloque_inodo(ninodo, primerBL, 0);
        if (nbfisico >= 0 && bread(nbfisico, buf_bloque) >= 0)
        {

            
            memcpy(buf_original, buf_bloque + desp1, nbytes); 

            
        }
        bytes_leidos += nbytes;
    }
    else
    {
        int nbfisico = traducir_bloque_inodo(ninodo, primerBL, 0);
        if (nbfisico >= 0 && bread(nbfisico, buf_bloque) >= 0)
        {

            
            memcpy(buf_original, buf_bloque + desp1, nbytes);

            
        }

        bytes_leidos += BLOCKSIZE - desp1;

        for (int bl = primerBL + 1; bl < ultimoBL; bl++)
        {
            nbfisico = traducir_bloque_inodo(ninodo, bl, 0);
            if (nbfisico >= 0 && bread(nbfisico, buf_bloque) >= 0)
            {

                
                memcpy(buf_original + bytes_leidos, buf_bloque, BLOCKSIZE);

                
            }
            bytes_leidos += BLOCKSIZE;
        }

        nbfisico = traducir_bloque_inodo(ninodo, ultimoBL, 0);

        if (nbfisico != -1 && bread(nbfisico, buf_bloque) != FALLO)
        {

        
            memcpy(buf_original + bytes_leidos, buf_bloque, desp2 + 1);

         
        }

        bytes_leidos += desp2 + 1;
    }
    inodo.atime = time(NULL);
    if (escribir_inodo(ninodo, &inodo) < 0)
    {
        fprintf(stderr, RED "[mi_read_f]--> No se pudo actualizar atime del inodo\n" RESET);
        return FALLO;
    }
    return bytes_leidos;
}

int mi_stat_f(unsigned int ninodo, struct STAT *p_stat)
{
    struct inodo inodo;

    if (leer_inodo(ninodo, &inodo) < 0)
    {
        fprintf(stderr, RED "[mi_stat_f] --> No se pudo leer el inodo\n" RESET);
        return FALLO;
    }

    p_stat->tipo = inodo.tipo;
    p_stat->permisos = inodo.permisos;
    p_stat->atime = inodo.atime;
    p_stat->mtime = inodo.mtime;
    p_stat->ctime = inodo.ctime;
    p_stat->btime = inodo.btime;
    p_stat->nlinks = inodo.nlinks;
    p_stat->tamEnBytesLog = inodo.tamEnBytesLog;
    p_stat->numBloquesOcupados = inodo.numBloquesOcupados;
    return EXITO;
}

int mi_chmod_f(unsigned int ninodo, unsigned char permisos)
{
    struct inodo inodo;

    if (leer_inodo(ninodo, &inodo) < 0)
    {
        fprintf(stderr, RED "[mi_chmod_f] --> No se pudo leer el inodo\n" RESET);
        return FALLO;
    }

    inodo.permisos = permisos;
    inodo.ctime = time(NULL);

    if (escribir_inodo(ninodo, &inodo) < 0)
    {
        fprintf(stderr, RED "[mi_chmod_f] -> No se pudo escribir el inodo\n");
        return FALLO;
    }

    return EXITO;
}

int mi_truncar_f(unsigned int ninodo, unsigned int nbytes)
{
    struct inodo inodo;
    leer_inodo(ninodo, &inodo);
    if ((inodo.permisos & 4) != 4)
    {
        fprintf(stderr, RED "No hay permisos de lectura [leer.c]\n" RESET);
        bumount();
        return FALLO;
    }
    int primerBL = (nbytes / BLOCKSIZE) + 1;
    if (nbytes % BLOCKSIZE == 0)
    {
        primerBL = (nbytes / BLOCKSIZE);
    }
    int liberados = liberar_bloques_inodo(primerBL, &inodo);
    inodo.mtime = time(NULL);
    inodo.ctime = time(NULL);
    inodo.tamEnBytesLog = nbytes;
    inodo.numBloquesOcupados -= liberados;
    escribir_inodo(ninodo, &inodo);
    return liberados;
}