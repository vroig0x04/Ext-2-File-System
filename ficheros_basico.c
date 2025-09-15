#include "ficheros_basico.h"
#include <limits.h>
#include <time.h>
int tamMB(unsigned int nbloques)
{
    int tam = (nbloques / 8) / BLOCKSIZE;
    if ((nbloques / 8) % BLOCKSIZE != 0)
    {
        return tam + 1;
    }
    return tam;
}

int tamAI(unsigned int ninodos)
{
    int tam = (ninodos * INODOSIZE) / BLOCKSIZE;
    if ((ninodos * INODOSIZE) % BLOCKSIZE != 0)
    {
        return tam + 1;
    }
    return tam;
}

int initSB(unsigned int nbloques, unsigned int ninodos)
{
    struct superbloque SB;
    SB.posPrimerBloqueMB = POSSB + TAMSB;
    SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + tamMB(nbloques) - 1;
    SB.posPrimerBloqueAI = SB.posUltimoBloqueMB + 1;
    SB.posUltimoBloqueAI = SB.posPrimerBloqueAI + tamAI(ninodos) - 1;
    SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI + 1;
    SB.posUltimoBloqueDatos = nbloques - 1;
    SB.posInodoRaiz = 0;
    SB.posPrimerInodoLibre = 0;
    SB.cantBloquesLibres = nbloques;
    SB.cantInodosLibres = ninodos;
    SB.totBloques = nbloques;
    SB.totInodos = ninodos;

    if (bwrite(POSSB, &SB) < 0)
    {
        return FALLO;
    }

    return EXITO;
}
int initMB()
{
    struct superbloque SB;
    if (bread(POSSB, &SB) < 0)
    {
        fprintf(stderr, RED "ERROR AL LEER EL SUPERBLOQUE [initMB()]\n" RESET);
        return FALLO;
    }
    int metadatos = TAMSB + tamMB(SB.totBloques) + tamAI(SB.totInodos);
    int bloquesEnteros = (metadatos / 8) / BLOCKSIZE;
    int bytesEnteros = (metadatos / 8) % BLOCKSIZE;
    int bitsSobrantes = metadatos % 8;

    char bufferMB[BLOCKSIZE];
    if (bloquesEnteros > 0)
    {
        memset(bufferMB, 255, BLOCKSIZE);
    }

    for (int j = 0; j < bloquesEnteros; j++)
    {
        if (bwrite(SB.posPrimerBloqueMB + j, bufferMB) < 0)
        {
            fprintf(stderr, RED "ERROR AL ESCRIBIR LOS BLOQUES ENTEROS [initMB()]\n" RESET);
            return FALLO;
        }
    }
    char buffBytes[BLOCKSIZE];
    memset(buffBytes, 0, BLOCKSIZE);

    int ultimoByteEscrito = 0;
    if (bytesEnteros > 0)
    {
        for (int i = 0; i < bytesEnteros; i++)
        {
            buffBytes[i] = (char)255;
            ultimoByteEscrito = i;
        }
    }
    if (bitsSobrantes > 0)
    {
        unsigned int numEq = 0;
        for (int i = 0; i < bitsSobrantes; i++)
        {
            numEq += (unsigned int)pow((double)2, (double)(7 - i));
        }
        buffBytes[ultimoByteEscrito + 1] = (char)numEq;
    }
    if (bytesEnteros > 0 || bitsSobrantes > 0)
    {
        if (bwrite(SB.posPrimerBloqueMB + bloquesEnteros, buffBytes) < 0)
        {
            fprintf(stderr, RED "ERROR AL ESCRIBIR LOS BYTES Y/O BITS [initMB()]\n" RESET);
            return FALLO;
        }
    }

    SB.cantBloquesLibres -= metadatos;

    if (bwrite(POSSB, &SB) < 0)
    {
        fprintf(stderr, RED "ERROR AL ESCRIBIR EL SUPERBLOQUE [initMB()]\n" RESET);
        return FALLO;
    }
    return EXITO;
}

int initAI()
{
    struct inodo inodos[BLOCKSIZE / INODOSIZE];
    struct superbloque SB;
    if (bread(POSSB, &SB) < 0)
    {
        fprintf(stderr, RED "ERROR AL LEER EL SUPERBLOQUE [initAI()]\n" RESET);
        return FALLO;
    }
    unsigned int contInodos = SB.posPrimerInodoLibre + 1;
    for (int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++)
    {

        int r = bread(i, inodos);
        if (r < 0)
        {
            fprintf(stderr, RED "ERROR AL LEER INODO\n" RESET);
        }
        if (r < 0)
            return FALLO;
        for (int j = 0; j < BLOCKSIZE / INODOSIZE; j++)
        {
            inodos[j].tipo = 'l';
            if (contInodos < SB.totInodos)
            {
                inodos[j].punterosDirectos[0] = contInodos;
                contInodos++;
            }
            else
            {
                inodos[j].punterosDirectos[0] = UINT_MAX;
            }
        }
        if (bwrite(i, inodos) < 0)
        {
            fprintf(stderr, RED "ERROR AL ESCRIBIR INODOS [initAI()]\n" RESET);
            return FALLO;
        }
    }
    return EXITO;
}

int escribir_bit(unsigned int nbloque, unsigned int bit)
{
    struct superbloque SB;
    if (bread(POSSB, &SB) < 0)
    {
        struct superbloque SB;
        if (bread(POSSB, &SB) < 0)
        {
            fprintf(stderr, RED "ERROR AL LEER EL SUPERBLOQUE [escribir_bit]" RESET);
            return FALLO;
        }
        int bloque = nbloque / (BLOCKSIZE * 8);
        int posbyte = (nbloque % (BLOCKSIZE * 8)) / 8;
        int posbit = (nbloque % (BLOCKSIZE * 8)) % 8;
        int nbloqueabs = SB.posPrimerBloqueMB + bloque;
        unsigned char bufferMB[BLOCKSIZE];
        if (bread(nbloqueabs, bufferMB) < 0)
        {
            fprintf(stderr, RED "ERROR AL LEER EN MB [escribir_bit()] \n" RESET);
            return FALLO;
        }
        unsigned char mascara = 128;
        mascara >>= posbit;
        if (bit == 1)
        {
            bufferMB[posbyte] |= mascara;
        }
        else
        {
            bufferMB[posbyte] &= ~mascara;
        }

        if (bwrite(nbloqueabs, bufferMB) < 0)
        {
            fprintf(stderr, RED "ERROR AL ESCRIBIR UN BIT [escribir_bit()] \n" RESET);
            return FALLO;
        }
        return EXITO;
    }
    // return FALLO;
    int bloque = nbloque / (BLOCKSIZE * 8);
    int posbyte = (nbloque % (BLOCKSIZE * 8)) / 8;
    int posbit = (nbloque % (BLOCKSIZE * 8)) % 8;
    int nbloqueabs = SB.posPrimerBloqueMB + bloque;
    unsigned char bufferMB[BLOCKSIZE];
    if (bread(nbloqueabs, bufferMB) < 0)
    {
        fprintf(stderr, RED "ERROR AL LEER EN MB [escribir_bit()] \n" RESET);
        return FALLO;
    }
    unsigned char mascara = 128;
    mascara >>= posbit;
    if (bit == 1)
    {
        bufferMB[posbyte] |= mascara;
    }
    else
    {
        bufferMB[posbyte] &= ~mascara;
    }

    if (bwrite(nbloqueabs, bufferMB) < 0)
    {
        fprintf(stderr, RED "ERROR AL ESCRIBIR UN BIT [escribir_bit()] \n" RESET);
        return FALLO;
    }
    return EXITO;
}

char leer_bit(unsigned int nbloque)
{
    struct superbloque SB;
    if (bread(POSSB, &SB) < 0)
    {
        fprintf(stderr, RED "ERROR AL LEER EL SUPERBLOQUE [leer_bit]" RESET);
        return FALLO;
    }
    int posbyte = ((nbloque) % (BLOCKSIZE * 8)) / 8;
    int posbit = (nbloque % (BLOCKSIZE * 8)) % 8;
    int bloqueMB = nbloque / (BLOCKSIZE * 8);
    int bloqueABS = SB.posPrimerBloqueMB + bloqueMB;
    unsigned char bufferMB[BLOCKSIZE];
    unsigned char mascara = 128;

    if (bread(bloqueABS, bufferMB) < 0)
    {
        fprintf(stderr, RED "Error al leer el bloque especificado" RESET);
        return FALLO;
    }

    mascara >>= posbit;
    mascara &= bufferMB[posbyte];
    mascara >>= (7 - posbit);

    return mascara;
}

int reservar_bloque()
{
    struct superbloque SB;
    if (bread(POSSB, &SB) < 0)
    {
        fprintf(stderr, RED "ERROR AL LEER EL SUPERBLOQUE [reservar_bloque]\n" RESET);
        return FALLO;
    }

    if (SB.cantBloquesLibres == 0)
    {
        fprintf(stderr, RED "NO QUEDA ESPACIO PARA RESERVAR BLOQUE [reservar_bloque()]\n" RESET);
        return FALLO;
    }

    unsigned char bufferAux[BLOCKSIZE];
    unsigned char bufferMB[BLOCKSIZE];
    memset(bufferAux, 255, BLOCKSIZE); // Buffer lleno de 1s (todos los bloques ocupados)

    int posAbsBloque = -1;

    for (int i = SB.posPrimerBloqueMB; i <= SB.posUltimoBloqueMB && posAbsBloque < 0; i++)
    {
        if (bread(i, bufferMB) < 0)
        {
            fprintf(stderr, RED "ERROR AL LEER EN EL BUFFER [reservar_bloque()]\n" RESET);
            return FALLO;
        }

        if (memcmp(bufferAux, bufferMB, BLOCKSIZE) != 0) // Hay al menos un bloque libre
        {
            for (int j = 0; j < BLOCKSIZE; j++)
            {
                if (bufferMB[j] != 255) // Hay un byte con al menos un bit libre
                {
                    unsigned char mascaraBit = 128; // 10000000
                    for (int k = 0; k < 8; k++)
                    {
                        if (!(bufferMB[j] & mascaraBit)) // Bit libre encontrado
                        {
                            bufferMB[j] |= mascaraBit; // Reservar el bit
                            int posBloque = (i - SB.posPrimerBloqueMB) * 8 * BLOCKSIZE + j * 8 + k;

                            if (bwrite(i, bufferMB) < 0)
                            {
                                fprintf(stderr, RED "ERROR AL ESCRIBIR EN EL MAPA DE BITS [reservar_bloque()]\n" RESET);
                                return FALLO;
                            }

                            SB.cantBloquesLibres--;
                            if (bwrite(POSSB, &SB) < 0)
                            {
                                fprintf(stderr, RED "ERROR AL ESCRIBIR EL SUPERBLOQUE [reservar_bloque()]\n" RESET);
                                return FALLO;
                            }

                            unsigned char buffBloq[BLOCKSIZE];
                            memset(buffBloq, 0, BLOCKSIZE); // Inicializar el bloque reservado a 0
                            if (bwrite(posBloque, buffBloq) < 0)
                            {
                                fprintf(stderr, RED "ERROR AL INICIALIZAR EL BLOQUE RESERVADO [reservar_bloque()]\n" RESET);
                                return FALLO;
                            }

                            posAbsBloque = posBloque;
                            break;
                        }
                        mascaraBit >>= 1; // Desplazar la máscara al siguiente bit
                    }
                    if (posAbsBloque >= 0)
                        break;
                }
            }
        }
    }

    if (posAbsBloque == -1)
    {
        fprintf(stderr, RED "NO SE ENCONTRÓ UN BLOQUE LIBRE [reservar_bloque()]\n" RESET);
        return FALLO;
    }

    return posAbsBloque;
}

int liberar_bloque(unsigned int nbloque)
{
    struct superbloque SB;
    if (bread(POSSB, &SB) < 0)
    {
        fprintf(stderr, RED "ERROR AL LEER EL SUPERBLOQUE [liberar_bloque]\n" RESET);
        return FALLO;
    }
    if (escribir_bit(nbloque, 0) < 0)
    {
        fprintf(stderr, RED "ERROR AL ESCRIBIR EL BIT A 0 [liberar_bloques]\n" RESET);
        return FALLO;
    }
    SB.cantBloquesLibres++;
    if (bwrite(POSSB, &SB) < 0)
    {
        fprintf(stderr, RED "ERROR AL ESCRIBIR EL SUPERBLOQUE [liberar_bloque]\n" RESET);
        return FALLO;
    }
    return nbloque;
}

int escribir_inodo(unsigned int ninodo, struct inodo *inodo)
{
    struct superbloque SB;
    struct inodo inodos[BLOCKSIZE / INODOSIZE];
    if (bread(POSSB, &SB) < 0)
    {
        fprintf(stderr, RED "ERROR AL LEER EL SUPERBLOQUE [escribir_inodo]\n" RESET);
        return FALLO;
    }
    int nbloqueAI = (ninodo * INODOSIZE) / BLOCKSIZE;
    int nbloquesABS = nbloqueAI + SB.posPrimerBloqueAI;

    if (bread(nbloquesABS, inodos) < 0)
    {
        fprintf(stderr, RED "ERROR AL LEER EL INODO\n" RESET);
        return FALLO;
    }
    int posinodo = ninodo % (BLOCKSIZE / INODOSIZE);
    inodos[posinodo] = *inodo;
    if (bwrite(nbloquesABS, inodos) < 0)
    {
        fprintf(stderr, RED "ERROR AL ESCRIBIR EL INODO EN El ARRAY DE INODOS\n" RESET);
        return FALLO;
    }
    return EXITO;
}
int leer_inodo(unsigned int ninodo, struct inodo *inodo)
{
    struct superbloque SB;
    if (bread(POSSB, &SB) < 0)
    {
        fprintf(stderr, RED "ERROR AL LEER EL SUPERBLOQUE [leer_inodo]" RESET);
        return FALLO;
    }
    int nbloqueAI = (ninodo * INODOSIZE / BLOCKSIZE) + SB.posPrimerBloqueAI;
    struct inodo inodos[BLOCKSIZE / INODOSIZE];
    int posinodo = ninodo % (BLOCKSIZE / INODOSIZE);
    if (bread(nbloqueAI, inodos) < 0)
    {
        fprintf(stderr, RED "ERROR AL LEER EL BLOQUE DE INODOS [leer_inodo]" RESET);
        return FALLO;
    }
    *inodo = inodos[posinodo];
    return posinodo;
}

int reservar_inodo(unsigned char tipo, unsigned char permisos)
{
    struct superbloque SB;
    struct inodo inodo;
    unsigned int posInodo;

    if (bread(POSSB, &SB) < 0)
    {
        fprintf(stderr, RED "ERROR AL LEER EL SUPERBLOQUE [reservar_inodo]" RESET);
        return FALLO;
    }
    if (SB.cantInodosLibres <= 0)
    {
        fprintf(stderr, RED "NO HAY MÁS INODOS LIBRE [reservar_inodo]" RESET);
        return FALLO;
    }
    if (leer_inodo(SB.posPrimerInodoLibre, &inodo) < 0)
    {
        fprintf(stderr, RED "ERROR AL LEER EL PRIMER INODO LIBRE [reservar_inodo]" RESET);
        return FALLO;
    }
    posInodo = SB.posPrimerInodoLibre;
    SB.posPrimerInodoLibre = inodo.punterosDirectos[0];
    for (int i = 0; i < DIRECTOS; i++)
    {
        inodo.punterosDirectos[i] = 0;
    }
    for (int i = 0; i < 3; i++)
    {
        inodo.punterosIndirectos[i] = 0;
    }
    SB.cantInodosLibres--;
    inodo.permisos = permisos;
    inodo.tipo = tipo;
    inodo.nlinks = 1;
    inodo.tamEnBytesLog = 0;
    inodo.atime = time(NULL);
    inodo.btime = time(NULL);
    inodo.ctime = time(NULL);
    inodo.mtime = time(NULL);
    inodo.numBloquesOcupados = 0;

    if (escribir_inodo(posInodo, &inodo) < 0)
    {
        fprintf(stderr, RED "ERROR AL ESCRIBIR EL INODO [reservar_inodo]" RESET);
        return FALLO;
    }
    if (bwrite(POSSB, &SB) < 0)
    {
        fprintf(stderr, RED "ERROR AL ESCRIBIR EL SUPERBLOQUE [reservar_inodo]" RESET);
        return FALLO;
    }
    return posInodo;
}

int obtener_nRangoBL(struct inodo *inodo, unsigned int nblogico, unsigned int *ptr)
{
    if (nblogico < DIRECTOS)
    {
        *ptr = inodo->punterosDirectos[nblogico];
        return 0;
    }
    else if (nblogico < INDIRECTOS0)
    {
        *ptr = inodo->punterosIndirectos[0];
        return 1;
    }
    else if (nblogico < INDIRECTOS1)
    {
        *ptr = inodo->punterosIndirectos[1];
        return 2;
    }
    else if (nblogico < INDIRECTOS2)
    {
        *ptr = inodo->punterosIndirectos[2];
        return 3;
    }
    else
    {
        *ptr = 0;
        fprintf(stderr, RED "valor fuera de rango en <obtener_nRangoBL>" RESET);
        return FALLO;
    }
}

int obtener_indice(unsigned int nblogico, int nivel_punteros)
{
    if (nblogico < DIRECTOS)
    {
        return nblogico;
    }
    else if (nblogico < INDIRECTOS0)
    {
        return nblogico - DIRECTOS;
    }
    else if (nblogico < INDIRECTOS1)
    {
        if (nivel_punteros == 2)
        {
            return (nblogico - INDIRECTOS0) / NPUNTEROS;
        }
        else if (nivel_punteros == 1)
        {
            return (nblogico - INDIRECTOS0) % NPUNTEROS;
        }
    }
    else if (nblogico < INDIRECTOS2)
    {
        if (nivel_punteros == 3)
        {
            return (nblogico - INDIRECTOS1) / (NPUNTEROS * NPUNTEROS);
        }
        else if (nivel_punteros == 2)
        {
            return (nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS) / NPUNTEROS;
        }
        else if (nivel_punteros == 1)
        {
            return (nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS) % NPUNTEROS;
        }
    }
    fprintf(stderr, RED "ERROR: no se pudo obtener el índice <obtener_indice()>" RESET);
    return FALLO;
}

// técnicamente esta función está arreglada :D
int traducir_bloque_inodo(unsigned int ninodo, unsigned int nblogico, unsigned char reservar)
{
    unsigned int ptr = 0, ptr_ant = 0, salvar_inodo = 0;
    int nRangoBL, nivel_punteros, indice;
    unsigned int buffer[NPUNTEROS];
    struct inodo inodo;

    if (leer_inodo(ninodo, &inodo) < 0)
    {
        return FALLO;
    }

    nRangoBL = obtener_nRangoBL(&inodo, nblogico, &ptr);
    nivel_punteros = nRangoBL;

    while (nivel_punteros > 0)
    {
        if (ptr == 0)
        {
            if (reservar == 0)
            {
                return FALLO;
            }

            ptr = reservar_bloque();
            if (ptr == FALLO)
            {
                // error
                return FALLO;
            }
            inodo.numBloquesOcupados = inodo.numBloquesOcupados + 1;

            inodo.ctime = time(NULL);
            salvar_inodo = 1;

            if (nivel_punteros == nRangoBL)
            {
                inodo.punterosIndirectos[nRangoBL - 1] = ptr;
#if DEBUGN4
                fprintf(stdout, "[traducir_bloque_inodo()--> inodo.punterosIndirectos[%d]=%d (reservado BF %d para punteros_nivel %d)]\n", nivel_punteros, ptr, ptr, nivel_punteros);
#endif
            }
            else
            {
                buffer[indice] = ptr;
                if (bwrite(ptr_ant, buffer) < 0)
                {
                    fprintf(stderr, RED "Error al escribir el bloque de punteros [traducir bloque inodo]\n" RESET);
                    return FALLO;
                }
#if DEBUGN4
                if (nivel_punteros > 0)
                {
                    fprintf(stdout, "[traducir_bloque_inodo() --> punteros_nivel%d [%d]=%d (reservado BG %d para punteros nivel_%d)]\n", nivel_punteros, indice, ptr, ptr, nivel_punteros - 1);
                }
#endif
            }
            memset(buffer, 0, BLOCKSIZE);
        }
        else
        {
            if (bread(ptr, buffer) < 0)
            {
                return FALLO;
            }
        }

        indice = obtener_indice(nblogico, nivel_punteros);

        if (indice == FALLO)
        {
            return FALLO;
        }

        ptr_ant = ptr;
        ptr = buffer[indice];
        nivel_punteros--;
    }

    if (ptr == 0)
    {
        if (reservar == 0)
        {
            return FALLO;
        }

        ptr = reservar_bloque();
        if (ptr < 0)
        {
            // error
            return FALLO;
        }
        inodo.numBloquesOcupados = inodo.numBloquesOcupados + 1;
        inodo.ctime = time(NULL);
        salvar_inodo = 1;

        if (nRangoBL == 0)
        {
#if DEBUGN4
            fprintf(stdout, "[traducir_bloque_inodo()--> inodo.punterosDirectos[%d]=%d (reservado BF %d para BL %d)]\n", nblogico, ptr, ptr, nblogico);
#endif
            inodo.punterosDirectos[nblogico] = ptr;
        }
        else
        {
            buffer[indice] = ptr;
            bwrite(ptr_ant, buffer);
#if DEBUGN4
            fprintf(stdout, "[traducir_bloque_inodo() --> punteros_nivel%i [%d]=%d (reservado BF %d para BL %d)]\n", nivel_punteros, indice, ptr, ptr, nblogico);
#endif
        }
    }
    if (salvar_inodo)
    {
        escribir_inodo(ninodo, &inodo);
    }
    return ptr;
}
int liberar_inodo(unsigned int ninodo)
{
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo) < 0)
    {
        fprintf(stderr, RED "[liberar_inodo()]-->ERROR AL LEER INODO" RESET);
        return FALLO;
    }
    int bloques_liberados = liberar_bloques_inodo(0, &inodo);
    inodo.numBloquesOcupados -= bloques_liberados;

    inodo.tipo = 'l';
    inodo.tamEnBytesLog = 0;
    struct superbloque SB;
    bread(POSSB, &SB);
    int pos_siguiente_inodo = SB.posPrimerInodoLibre;
    SB.posPrimerInodoLibre = ninodo;
    inodo.punterosDirectos[0] = pos_siguiente_inodo;
    SB.cantInodosLibres++;
    if (bwrite(POSSB, &SB) < 0)
    {
        fprintf(stderr, RED "[liberar_inodo()]-->ERROR AL ESCRIBIR EL SUPERBLOQUE" RESET);
    }
    inodo.ctime = time(NULL);
    if (escribir_inodo(ninodo, &inodo) < 0)
    {
        fprintf(stderr, RED "[liberar_inodo()]-->ERROR AL ESCRIBIR EL INODO EN LA POSICIÓN %d" RESET, ninodo);
    }
    return ninodo;
}
// FORMA ITERATIVA
int liberar_bloques_inodo(unsigned int primerBL, struct inodo *inodo)
{
    unsigned int nivel_punteros = 0;
    unsigned int indice = 0;
    unsigned int ptr = 0;
    unsigned int nBL;
    unsigned int ultimoBL;
    int nRangoBL;
    unsigned int bloques_punteros[3][NPUNTEROS];
    unsigned int bufAux_punteros[3][NPUNTEROS] = {0};
    int ptr_nivel[3];
    int indices[3];
    int liberados = 0;
    int total_breads = 0;
    int total_bwrites = 0;

    if (inodo->tamEnBytesLog == 0)
    {
        return liberados;
    }

    if ((inodo->tamEnBytesLog % BLOCKSIZE) == 0)
    {
        ultimoBL = inodo->tamEnBytesLog / BLOCKSIZE - 1;
    }
    else
    {
        ultimoBL = inodo->tamEnBytesLog / BLOCKSIZE;
    }
#if DEBUGN6
    fprintf(stderr, "[liberar_bloques_inodo()] primerBL = %d, ultimoBL = %d\n", primerBL, ultimoBL);
#endif
    for (nBL = primerBL; nBL <= ultimoBL; nBL++) // no se si < o <=?
    {
        nRangoBL = obtener_nRangoBL(inodo, nBL, &ptr);
        if (nRangoBL < 0)
        {
            return FALLO;
        }
        nivel_punteros = nRangoBL;

        while (ptr > 0 && nivel_punteros > 0)
        {
            indice = obtener_indice(nBL, nivel_punteros);
            if (indice == 0 || nBL == primerBL)
            {
                bread(ptr, bloques_punteros[nivel_punteros - 1]);
                total_breads++;
            }
            ptr_nivel[nivel_punteros - 1] = ptr;
            indices[nivel_punteros - 1] = indice;
            ptr = bloques_punteros[nivel_punteros - 1][indice];
            nivel_punteros--;
        }

        if (ptr > 0)
        {
#if DEBUGN6
            fprintf(stderr, "[liberar_bloques_inodo()] liberando bloque fisico %d (BL %d)\n", ptr, nBL);
#endif
            liberar_bloque(ptr);
            liberados++;
            if (nRangoBL == 0)
            {
                inodo->punterosDirectos[nBL] = 0;
            }
            else
            {
                nivel_punteros = 1;
                while (nivel_punteros <= nRangoBL)
                {
                    indice = indices[nivel_punteros - 1];
                    bloques_punteros[nivel_punteros - 1][indice] = 0;
                    ptr = ptr_nivel[nivel_punteros - 1];
                    if (memcmp(bloques_punteros[nivel_punteros - 1], bufAux_punteros, BLOCKSIZE) == 0)
                    {
#if DEBUGN6
                        fprintf(stderr, "[liberar_bloques_inodo()] liberando puntero %d de nivel %d\n", ptr, nivel_punteros);
#endif
                        liberar_bloque(ptr);
                        liberados++;
                        if (nivel_punteros == nRangoBL)
                        {
                            inodo->punterosIndirectos[nRangoBL - 1] = 0;
                        }
                        nivel_punteros++;
                    }
                    else
                    {
#if DEBUGN6
                        fprintf(stderr, "[liberar_bloques_inodo()] liberando punteros de nivel %d\n", nivel_punteros);
#endif
                        bwrite(ptr, bloques_punteros[nivel_punteros - 1]);
                        total_bwrites++;
                        nivel_punteros = nRangoBL + 1;
                    }
                }
            }
        }
    }
#if DEBUGN6
    fprintf(stderr, "[liberar_bloques_inodo()] total bloques liberados: %d, total breads: %d, total bwrites %d\n", liberados, total_breads, total_bwrites);
#endif

    return liberados;
}
void ver_inodo(struct inodo *inodo)
{
    printf("número de bloques ocupados inodo: %d\n", inodo->numBloquesOcupados);
    printf("permisos del inodo: %d\n", (int)inodo->permisos);
    printf("tamaño en bytes del inodo: %d\n", inodo->tamEnBytesLog);
    printf("tipo del inodo: %c\n", inodo->tipo);
    printf("nlinks del inodo: %d\n", inodo->nlinks);
    printf("punteros directos del inodo: %ls\n", inodo->punterosDirectos);
    printf("punteros indirectos del inodo: %ls\n", inodo->punterosIndirectos);
}