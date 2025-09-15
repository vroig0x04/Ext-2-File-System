#include "directorios.h"
#define TAM_CACHE 3
#define DEBUGN10 0
typedef struct Cache
{
    char nombre[TAMNOMBRE * PROFMAXIMA];
    int ninodo;
} Cache;
static Cache cache[TAM_CACHE];
int indexFIFO = TAM_CACHE - 1;

int extraer_camino(const char *camino, char *inicial, char *final, char *tipo)
{

    if (*camino != '/')
    {
        fprintf(stderr, RED "[extraer_camino] --> El camino introducido no empieza por '/': %s \n" RESET, camino);
        return FALLO;
    }
    int i = 1;
    int j = 0;
    while (*(camino + i) != '/' && *(camino + i) != '\0')
    {
        *(inicial + j) = *(camino + i);
        i++;
        j++;
    }
    *(inicial + j) = '\0';

    if (*(camino + i) == '/')
    {
        j = 0;
        *tipo = 'd';
        while (*(camino + i) != '\0')
        {
            *(final + j) = *(camino + i);
            i++;
            j++;
        }
        *(final + j) = '\0';
    }
    else
    {
        *tipo = 'f';
        *final = '\0';
    }
#if DEBUGN7
    fprintf(stdout, BLUE "Camino: %s  \nInicial: %s \nFinal: %s \nTipo: %c\n" RESET, camino, inicial, final, *tipo);
#endif
    return EXITO;
}

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir,
                   unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos)
{
    // Variables locales
    struct entrada buffer_entradas[BLOCKSIZE / sizeof(struct entrada)];
    struct entrada entrada;
    struct inodo inodo_dir;
    struct superbloque SB;
    char inicial[TAMNOMBRE];
    char final[TAMNOMBRE];
    char tipo;
    int cant_entradas_inodo, num_entrada_inodo;

    // Leer el superbloque
    if (bread(POSSB, &SB) < 0)
    {
        fprintf(stderr, RED "[BUSCAR_ENTRADA()--> ERROR AL LEER EL SUPERBLOQUE]\n" RESET);
        return FALLO;
    }

    // Si el camino es la raíz
    if (strcmp(camino_parcial, "/") == 0)
    {
        *p_inodo = SB.posInodoRaiz;
        *p_entrada = 0;
        return EXITO;
    }

    // Extraer el primer componente del camino
    if (extraer_camino(camino_parcial, inicial, final, &tipo))
    {
        return ERROR_CAMINO_INCORRECTO;
    }

    // Leer el inodo del directorio actual
    if (leer_inodo(*p_inodo_dir, &inodo_dir) < 0)
    {
        return ERROR_PERMISO_LECTURA;
    }

    // Verificar permisos de lectura
    if ((inodo_dir.permisos & 4) != 4)
    {
        fprintf(stderr, RED "[buscar_entrada] --> El inodo %d no tiene permisos de lectura: %s \n" RESET, *p_inodo_dir, camino_parcial);
        return ERROR_PERMISO_LECTURA;
    }

    // Inicializar variables
    memset(buffer_entradas, 0, BLOCKSIZE);

    cant_entradas_inodo = inodo_dir.tamEnBytesLog / sizeof(struct entrada);
    num_entrada_inodo = 0;
    int ent_encontrada = 0;
    int desplazamiento = 0;

    struct entrada entrada_encontrada;

    // Buscar la entrada en el directorio
    if (cant_entradas_inodo > 0)
    {

        while ((desplazamiento < cant_entradas_inodo * sizeof(struct entrada)) && !ent_encontrada)
        {

            if (mi_read_f(*p_inodo_dir, buffer_entradas, desplazamiento, BLOCKSIZE) < 0)
            {
                fprintf(stderr, RED "[buscar_entrada] --> Error al leer el buffer buscando la entrada del directorio \n" RESET);
                return FALLO;
            }

            for (int i = 0; (i < BLOCKSIZE / sizeof(struct entrada)) && !ent_encontrada; i++)
            {
                if (strcmp(inicial, buffer_entradas[i].nombre) == 0)
                {

                    num_entrada_inodo = (desplazamiento / sizeof(struct entrada)) + i;
                    entrada_encontrada = buffer_entradas[i];
                    ent_encontrada = 1;
                }
            }
            desplazamiento += BLOCKSIZE;
        }
    }
    // Si no se encuentra la entrada

    if (!ent_encontrada)
    {
        if (reservar == 0)
        {
            return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
        }

        // Crear una nueva entrada
        if (inodo_dir.tipo == 'f')
        {
            return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
        }

        if ((inodo_dir.permisos & 2) != 2)
        {
            return ERROR_PERMISO_ESCRITURA;
        }

        // Reservar un nuevo inodo
        strcpy(entrada.nombre, inicial);
        int nuevo_inodo = reservar_inodo(tipo, permisos);
        if (nuevo_inodo < 0)
        {
            return FALLO;
        }
        entrada.ninodo = nuevo_inodo;
        *p_inodo = nuevo_inodo;

        if (mi_write_f(*p_inodo_dir, &entrada, cant_entradas_inodo * sizeof(struct entrada), sizeof(struct entrada)) < 0)
        {
            liberar_inodo(entrada.ninodo);
            fprintf(stderr, RED "[buscar_entrada]-->NO SE HA PODIDO CREAR EL DIRECTORIO\n" RESET);
            return FALLO;
        }
        entrada_encontrada = entrada;
        num_entrada_inodo = cant_entradas_inodo;
        ent_encontrada = 1;
    }

    // Si el camino no ha terminado, continuar recursivamente
    if ((strcmp(final, "") != 0) && (strcmp(final, "/") != 0))
    {
        *p_inodo_dir = entrada_encontrada.ninodo;

        *p_inodo = 0;
        *p_entrada = 0;
        return buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);
    }
    else
    {

        if (ent_encontrada == 1)
        {
            *p_inodo = entrada_encontrada.ninodo;
        }

        *p_entrada = num_entrada_inodo;
    }
    return EXITO;
}

void mostrar_error_buscar_entrada(int error)
{
    switch (error)
    {
    case -2:
        fprintf(stderr, RED "Error: Camino incorrecto.\n" RESET);
        break;
    case -3:
        fprintf(stderr, RED "Error: Permiso denegado de lectura.\n" RESET);
        break;
    case -4:
        fprintf(stderr, RED "Error: No existe el archivo o el directorio.\n" RESET);
        break;
    case -5:
        fprintf(stderr, RED "Error: No existe algún directorio intermedio.\n" RESET);
        break;
    case -6:
        fprintf(stderr, RED "Error: Permiso denegado de escritura.\n" RESET);
        break;
    case -7:
        fprintf(stderr, RED "Error: El archivo ya existe.\n" RESET);
        break;
    case -8:
        fprintf(stderr, RED "Error: No es un directorio.\n" RESET);
        break;
    }
}

int mi_creat(const char *camino, unsigned char permisos)
{
    mi_waitSem();
    struct superbloque SB;
    if (bread(POSSB, &SB) < 0)
    {
        fprintf(stderr, RED "[mi_creat]-->ERROR AL LEER EL SUPERBLOQUE\n" RESET);
    }
    unsigned int posInodo_raiz = SB.posInodoRaiz;
    unsigned int posInodo;
    unsigned int posEntrada;
    int error = buscar_entrada(camino, &posInodo_raiz, &posInodo, &posEntrada, 1, permisos);
    if (error < 0)
    {
        mostrar_error_buscar_entrada(error);
        mi_signalSem();
        return FALLO;
    }
    else
    {
        mi_signalSem();
        return EXITO;
    }
}

int mi_dir(const char *camino, char *buffer, char flag)
{
    struct entrada buffer_entradas[BLOCKSIZE / sizeof(struct entrada)];
    struct inodo inodo;
    struct superbloque SB;
    int num_entradas;

    mi_waitSem();
    if (bread(POSSB, &SB) < 0)
    {
        fprintf(stderr, RED "[mi_dir] --> ERROR AL LEER EL SUPERBLOQUE]\n" RESET);
        mi_signalSem();
        return FALLO;
    }
    unsigned int pinodoRaiz = SB.posInodoRaiz, p_inodo, p_entrada;

    if (buscar_entrada(camino, &pinodoRaiz /* Me da miedo */, &p_inodo, &p_entrada, 0, 0) < 0)
    {
        fprintf(stderr, RED "[mi_dir] --> ERROR AL BUSCAR ENTRADA\n" RESET);
        mi_signalSem();
        return FALLO;
    }

    if (leer_inodo(p_inodo, &inodo) < 0)
    {
        fprintf(stderr, RED "[mi_dir] --> ERROR AL LEER EL INODO\n" RESET);
        mi_signalSem();
        return FALLO;
    }

    if (inodo.tipo != 'd')
    {
        fprintf(stderr, RED "[mi_dir] --> ERROR: No es un directorio\n" RESET);
        mi_signalSem();
        return FALLO;
    }

    if ((inodo.permisos & 4) != 4)
    {
        fprintf(stderr, RED "[mi_dir] --> ERROR: No hay permisos de lectura\n" RESET);
        mi_signalSem();
        return ERROR_PERMISO_LECTURA;
    }

    num_entradas = inodo.tamEnBytesLog / sizeof(struct entrada);
    
    for (int i = 0; i < num_entradas; i++)
    {

        int modulo = i % (BLOCKSIZE / sizeof(struct entrada));
        if (modulo == 0)
        {
            memset(buffer_entradas, 0, BLOCKSIZE);
            if (mi_read_f(p_inodo, buffer_entradas, i * sizeof(struct entrada), BLOCKSIZE) < 0)
            {
                fprintf(stderr, RED "[mi_dir] --> ERROR AL LEER ENTRADA\n" RESET);
                mi_signalSem();
                return FALLO;
            }
        }
        strcat(buffer, buffer_entradas[modulo].nombre);

        fflush(stderr);
        strcat(buffer, "\t");
        if (flag == 'l')
        {
            struct inodo inodo_entrada;
            if (leer_inodo(buffer_entradas[modulo].ninodo, &inodo_entrada) < 0)
            {
                fprintf(stderr, RED "[mi_dir] --> ERROR AL LEER EL INODO DE LA ENTRADA\n" RESET);
                mi_signalSem();
                return FALLO;
            }

            struct tm *tm;
            char formateo[100];
            tm = localtime(&inodo_entrada.mtime);
            sprintf(formateo, "%d-%02d-%02d %02d:%02d:%02d ", tm->tm_year + 1900, tm->tm_mon + 1,
                    tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
            strcat(buffer, formateo);

            if (inodo_entrada.permisos & 4)
                strcat(buffer, "r");
            else
                strcat(buffer, "-");
            if (inodo_entrada.permisos & 2)
                strcat(buffer, "w");
            else
                strcat(buffer, "-");
            if (inodo_entrada.permisos & 1)
                strcat(buffer, "x");
            else
                strcat(buffer, "-");

            char tam[TAMNOMBRE];

            sprintf(tam, "\t%d\n", inodo_entrada.tamEnBytesLog);
            strcat(buffer, tam);
        }
    }
    mi_signalSem();
    return num_entradas;
}

int mi_chmod(const char *camino, unsigned char permisos)
{
    struct superbloque SB;
    if (bread(POSSB, &SB) < 0)
    {
        fprintf(stderr, RED "[mi_creat]-->ERROR AL LEER EL SUPERBLOQUE\n" RESET);
    }
    unsigned int posInodo_raiz = SB.posInodoRaiz;
    unsigned int posInodo;
    unsigned int posEntrada;
    int error = buscar_entrada(camino, &posInodo_raiz, &posInodo, &posEntrada, 0, permisos);
    if (error < 0)
    {
        mostrar_error_buscar_entrada(error);
    }
#if DEBUGN8
    struct inodo inodo;
    if (leer_inodo(posInodo, &inodo) < 0)
    {
        fprintf(stderr, RED "[mi_chmod] error al leer el inodo: %d\n" RESET, posInodo);
        return FALLO;
    }

    fprintf(stderr, "Permisos del inodo %d antes del cambio: %d\n", posInodo, inodo.permisos);
#endif
    mi_waitSem();
    if (mi_chmod_f(posInodo, permisos) == FALLO)
    {
        mi_signalSem();
        return FALLO;
    }
    mi_signalSem();
#if DEBUGN8
    struct inodo inodo;
    leer_inodo(posInodo, &inodo);
    fprintf(stderr, "Permisos del inodo %d despues del cambio: %d\n", posInodo, inodo.permisos);
#endif
    return EXITO;
}

int mi_stat(const char *camino, struct STAT *p_stat)
{
    struct superbloque SB;
    
    if (bread(POSSB, &SB) < 0)
    {
        fprintf(stderr, RED "[mi_stat] --> ERROR AL LEER EL SUPERBLOQUE\n" RESET);
        return FALLO;
    }
    unsigned int pindoRaiz = SB.posInodoRaiz, p_inodo_dir, p_entrada;

    int error = buscar_entrada(camino, &pindoRaiz, &p_inodo_dir, &p_entrada, 0, 0);
    if (error < 0)
    {
        mostrar_error_buscar_entrada(error);
        return error;
    }
    int resultado = mi_stat_f(p_inodo_dir, p_stat);
    if (resultado < 0)
    {
        fprintf(stderr, RED "[mi_stat] --> ERROR AL LEER EL INODO\n" RESET);
        return FALLO;
    }
    return p_inodo_dir;
}

int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes)
{
    struct superbloque SB;
    if (bread(POSSB, &SB) < 0)
    {
        fprintf(stderr, RED "[mi_write] --> ERROR AL LEER EL SUPERBLOQUE\n" RESET);
        return FALLO;
    }
    unsigned int pinodoRaiz = SB.posInodoRaiz, p_inodo_dir, p_entrada;

    int encontrado = 0;
    for (int i = 0; i < TAM_CACHE && !encontrado; i++)
    {
        if (strcmp(cache[i].nombre, camino) == 0)
        {
            p_inodo_dir = cache[i].ninodo;
            encontrado = 1;
#if DEBUGN9
            fprintf(stderr, BLUE "[mi_write() -> Utilizamos cache[%d]: %s] \n" RESET, i, cache[i].nombre);
#endif
        }
    }
    if (!encontrado)
    {
        int error = buscar_entrada(camino, &pinodoRaiz, &p_inodo_dir, &p_entrada, 0, 0);
        if (error < 0)
        {
            mostrar_error_buscar_entrada(error);
            return FALLO;
        }
        indexFIFO = (indexFIFO + 1) % TAM_CACHE;
#if DEBUGN9
        fprintf(stderr, ORANGE "[mi_write() -> Remplazamos cache[%d]: %s] \n" RESET, indexFIFO, camino);
#endif
        strncpy(cache[indexFIFO].nombre, camino, sizeof(cache[indexFIFO].nombre));
        cache[indexFIFO].ninodo = p_inodo_dir;
    }
    mi_waitSem();
    int e=mi_write_f(p_inodo_dir, buf, offset, nbytes);
    mi_signalSem();
    return e;
}

int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes)
{
    struct superbloque SB;
    int resultado;
    if (bread(POSSB, &SB) < 0)
    {
        fprintf(stderr, RED "[mi_read] --> ERROR AL LEER EL SUPERBLOQUE\n" RESET);
        return FALLO;
    }
    unsigned int pinodoRaiz = SB.posInodoRaiz, p_inodo_dir, p_entrada;
    for (int i = 0; i < TAM_CACHE; i++)
    {
        if (strcmp(camino, cache[i].nombre) == 0)
        {
            p_inodo_dir = cache[i].ninodo;
            mi_waitSem();
            resultado = mi_read_f(p_inodo_dir, buf, offset, nbytes);
            mi_signalSem();
            return resultado;
        }
    }
    
    int error = buscar_entrada(camino, &pinodoRaiz, &p_inodo_dir, &p_entrada, 0, 0);
    if (error < 0)
    {
        mostrar_error_buscar_entrada(error);
        return FALLO;
    }

    indexFIFO = (indexFIFO + 1) % TAM_CACHE;
    strncpy(cache[indexFIFO].nombre, camino, sizeof(cache[indexFIFO].nombre));
    cache[indexFIFO].ninodo = p_inodo_dir;
    mi_waitSem();
    resultado = mi_read_f(p_inodo_dir, buf, offset, nbytes);
    mi_signalSem();
    return resultado;
}

int mi_link(const char *camino_original, const char *camino_destino)
{
    struct superbloque SB;
    if (bread(POSSB, &SB) < 0)
    {
        fprintf(stderr, RED "[mi_link] --> Error al leer el superbloque\n" RESET);
        return FALLO;
    }

    unsigned int posInodo_raiz1 = SB.posInodoRaiz;
    unsigned int posInodo_raiz2 = SB.posInodoRaiz;
    unsigned int p_inodo1, p_entrada1;
    unsigned int p_inodo2, p_entrada2;

    // Buscar la entrada del archivo original
    mi_waitSem();
    int error = buscar_entrada(camino_original, &posInodo_raiz1, &p_inodo1, &p_entrada1, 0, 6);
#if DEBUGN10
    fprintf(stdout, YELLOW "DESPUES DE BUSCAR LA ENTRADA DEL CAMINO ORIGINAL:\n" RESET);
    fprintf(stdout, BLUE "camino_original: %s \n" RESET, camino_original);
    fprintf(stdout, BLUE "Posicion Inodo Raiz 1: %i\n" RESET, posInodo_raiz1);
    fprintf(stdout, BLUE "Posicion Inodo 1: %i\n" RESET, p_inodo1);
    fprintf(stdout, BLUE "Posicion entrada 1: %i \n" RESET, p_entrada1);
#endif
    if (error < 0)
    {
        mostrar_error_buscar_entrada(error);
        mi_signalSem();
        return FALLO;
    }
    struct inodo inodo_original;
    if (leer_inodo(p_inodo1, &inodo_original) < 0)
    {
        fprintf(stderr, RED "[mi_link] --> Error al leer el inodo del archivo original\n" RESET);
        mi_signalSem();
        return FALLO;
    }
    // Verificar que el archivo original es un fichero
    if (inodo_original.tipo != 'f')
    {
        fprintf(stderr, RED "[mi_link] --> El archivo original no es un fichero\n" RESET);
        mi_signalSem();
        return FALLO;
    }
    if ((inodo_original.permisos & 4) != 4)
    {
        fprintf(stderr, RED "[mi_link] --> No hay permisos de lectura en el archivo original\n" RESET);
        mi_signalSem();
        return FALLO;
    }
    // Buscar la entrada del destino
    error = buscar_entrada(camino_destino, &posInodo_raiz2, &p_inodo2, &p_entrada2, 0, 6);
    if (!(error < 0))
    {
        fprintf(stderr, RED "ENTRADA YA EXISTE\n" RESET);
        mi_signalSem();
        return FALLO;
    }
    posInodo_raiz2 = SB.posInodoRaiz;
    p_inodo2 = 0;
    p_entrada2 = 0;
    error = buscar_entrada(camino_destino, &posInodo_raiz2, &p_inodo2, &p_entrada2, 1, 6);

#if DEBUGN10
    fprintf(stdout, YELLOW "DESPUES DE BUSCAR LA ENTRADA DEL CAMINO DESTINO:\n" RESET);
    fprintf(stdout, BLUE "camino_destino: %s \n" RESET, camino_destino);
    fprintf(stdout, BLUE "Posicion Inodo Raiz 2: %i\n" RESET, posInodo_raiz2);
    fprintf(stdout, BLUE "Posicion Inodo 2: %i\n" RESET, p_inodo2);
    fprintf(stdout, BLUE "Posicion entrada 1: %i \n" RESET, p_entrada2);
#endif
    struct entrada nueva_entrada;

    if (mi_read_f(posInodo_raiz2, &nueva_entrada, (p_entrada2) * sizeof(struct entrada), sizeof(struct entrada)) < 0)
    {
        fprintf(stderr, RED "[mi_link] --> Error al leer la entrada destino \n" RESET);
        mi_signalSem();
        return FALLO;
    }
    p_inodo2 = nueva_entrada.ninodo;
#if DEBUGN10
    fprintf(stdout, YELLOW "ANTES DE ASIGNAR:\n" RESET);
    fprintf(stdout, BLUE "posInodoRaiz2: %i \n" RESET, posInodo_raiz2);
    fprintf(stdout, BLUE "Nombre entrada: %s\n" RESET, nueva_entrada.nombre);
    fprintf(stdout, BLUE "Ninodoentrada antes de asignar: %d\n" RESET, nueva_entrada.ninodo);
#endif
    nueva_entrada.ninodo = p_inodo1;
    if (liberar_inodo(p_inodo2) < 0)
    {
        fprintf(stderr, RED "[mi_link] --> Error al liberar el inodo \n" RESET);
        mi_signalSem();
        return FALLO;
    }
    if (mi_write_f(posInodo_raiz2, &nueva_entrada, (p_entrada2) * sizeof(struct entrada), sizeof(struct entrada)) < 0)
    {
        fprintf(stderr, RED "[mi_link] --> Error al escribir la entrada destino \n" RESET);
        mi_signalSem();
        return FALLO;
    }
#if DEBUGN10
    fprintf(stdout, YELLOW "DESPUES DE ASIGNAR:\n" RESET);
    fprintf(stdout, BLUE "posInodoRaiz2: %i \n" RESET, posInodo_raiz2);
    fprintf(stdout, BLUE "Nombre entrada: %s \n" RESET, nueva_entrada.nombre);
    fprintf(stdout, BLUE "Ninodoentrada despues de asignar: %d\n" RESET, nueva_entrada.ninodo);
#endif

    inodo_original.nlinks++;
    inodo_original.ctime = time(NULL);
    if (escribir_inodo(p_inodo1, &inodo_original) < 0)
    {
        fprintf(stderr, RED "[mi_link] --> Error al escribir inodo \n" RESET);
        mi_signalSem();
        return FALLO;
    }
    mi_signalSem();
    return EXITO;
}

int mi_unlink(const char *camino)
{
    struct superbloque SB;
    mi_waitSem();
    if (bread(POSSB, &SB) < 0)
    {
        fprintf(stderr, RED "[mi_unlink] --> Error al leer el superbloque\n" RESET);
        return FALLO;
    }
    unsigned int p_inodo_dir = SB.posInodoRaiz;
    unsigned p_inodo = 0, p_entrada = 0;
    int error;
  
    // buscar entrada y leer inodo
    error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0);
    if (error < 0)
    {
        mostrar_error_buscar_entrada(error);
        mi_signalSem();
        return FALLO;
    }

    struct inodo inodo;

    if (leer_inodo(p_inodo, &inodo) < 0)
    {
        fprintf(stderr, RED "[mi_unlink] -> Error al leer inodo \n" RESET);
        mi_signalSem();
        return FALLO;
    }

    if ((inodo.tipo == 'd') && (inodo.tamEnBytesLog > 0))
    {
        fprintf(stderr, RED "[mi_unlink] -> El directorio contiene entradas \n" RESET);
        mi_signalSem();
        return FALLO;
    }

    struct inodo inodo_dir;
    if (leer_inodo(p_inodo_dir, &inodo_dir) < 0)
    {
        fprintf(stderr, RED "[mi_unlink] -> Error al leer inodo raiz \n" RESET);
        mi_signalSem();
        return FALLO;
    }

    int n_entradas = inodo_dir.tamEnBytesLog / sizeof(struct entrada);
    struct entrada entrada_lectura;
    if (p_entrada != n_entradas - 1)
    {
        if (mi_read_f(p_inodo_dir, &entrada_lectura, ((n_entradas)-1) * sizeof(struct entrada), sizeof(struct entrada)) < 0)
        {
            fprintf(stderr, RED "[mi_unlink] -> Error al leer posible ultima entrada \n" RESET);
            mi_signalSem();
            return FALLO;
        }

        if (mi_write_f(p_inodo_dir, &entrada_lectura, p_entrada * sizeof(struct entrada), sizeof(struct entrada)) < 0)
        {
            fprintf(stderr, RED "[mi_unlink] -> Error al sobrescribir lo leido \n" RESET);
            mi_signalSem();
            return FALLO;
        }
    }

    if (mi_truncar_f(p_inodo_dir, inodo_dir.tamEnBytesLog - sizeof(struct entrada)) < 0)
    {
        fprintf(stderr, RED "[mi_unlink] -> Error al truncar el inodo \n" RESET);
        mi_signalSem();
        return FALLO;
    }
    inodo.nlinks--;
    if (inodo.nlinks == 0)
    {
        if (liberar_inodo(p_inodo) < 0)
        {
            fprintf(stderr, RED "[mi_unlink] -> Error al liberar el inodo \n" RESET);
            mi_signalSem();
            return FALLO;
        }
    }
    else
    {
        inodo.ctime = time(NULL);
        if (escribir_inodo(p_inodo, &inodo) < 0)
        {
            fprintf(stderr, RED "[mi_unlink] -> Error al escribir inodo \n" RESET);
            mi_signalSem();
            return FALLO;
        }
    }

    mi_signalSem();
    return EXITO;
}
