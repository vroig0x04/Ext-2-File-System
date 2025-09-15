#include "verificacion.h"
int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, RED "ERROR, USO CORRECTO: verificacion <nombre_dispositivo> <directorio_simulación>\n" RESET);
        return FALLO;
    }
    char *ndisco = argv[1];
    char *directorioSim = argv[2];
    if (bmount(ndisco) < 0)
    {
        fprintf(stderr, RED "Error al montar el dispositivo\n" RESET);
        return FALLO;
    }
    struct STAT p_stat;
    unsigned int num_entradas = 0;
    if (mi_stat(directorioSim, &p_stat) < 0)
    {
        fprintf(stderr, RED "Error al obtener el estado del direcotorio\n" RESET);
        bumount();
        return FALLO;
    }

    num_entradas = p_stat.tamEnBytesLog / sizeof(struct entrada);
    if (num_entradas != NUM_PROCESOS)
    {
        fprintf(stderr, RED "Error: El número de entradas en el directorio no es correcto\n" RESET);
        bumount();
        return FALLO;
    }
    char rutaInforme[256];
    strcpy(rutaInforme, directorioSim);
    strcat(rutaInforme, "informe.txt");

    if (mi_creat(rutaInforme, 6) < 0)
    {
        fprintf(stderr, RED "Error al crear el informe.txt \n" RESET);
        bumount();
        return FALLO;
    }
    struct entrada entradas[NUM_PROCESOS];
    if (mi_read(directorioSim, entradas, 0, sizeof(struct entrada) * num_entradas) < 0)
    {
        fprintf(stderr, RED "Error al leer entradas del directorio \n" RESET);
        bumount();
        return FALLO;
    }
    int offsetTexto = 0;
    for (int i = 0; i < num_entradas; i++)
    {
        char nombreProceso[256];
        strcpy(nombreProceso, entradas[i].nombre);
        int pid = 0;
        sscanf(nombreProceso, "proceso_%d", &pid);
        char rutaPrueba[256] = "";
        strcat(rutaPrueba, directorioSim);
        strcat(rutaPrueba, nombreProceso);
        strcat(rutaPrueba, "/prueba.dat");
        struct REGISTRO bufferescrituras[NUM_ESCRITURAS];
        memset(bufferescrituras, 0, sizeof(bufferescrituras));
        unsigned int offset = 0;
        struct INFORMACION infoProceso;
        infoProceso.pid = pid;
        infoProceso.nEscrituras = 0;
        int primeraValida = 1;
        infoProceso.MenorPosicion.nEscritura = 0;
        infoProceso.UltimaEscritura.nEscritura = 0;
        infoProceso.MenorPosicion.nRegistro = 0;
        infoProceso.MayorPosicion.nRegistro = 0;
        int leidos = 0;
        leidos = mi_read(rutaPrueba, bufferescrituras, offset, sizeof(bufferescrituras));
        while (leidos > 0)
        {
            for (int j = 0; j < NUM_ESCRITURAS; j++)
            {
                struct REGISTRO reg = bufferescrituras[j];
                if (reg.pid == pid)
                {
                    if (primeraValida)
                    {
                        infoProceso.MenorPosicion = reg;
                        infoProceso.MayorPosicion = reg;
                        infoProceso.UltimaEscritura = reg;
                        infoProceso.PrimeraEscritura = reg;
                        primeraValida = 0;
                    }
                    else
                    {

                        if (reg.nRegistro < infoProceso.MenorPosicion.nRegistro)
                        {
                            infoProceso.MenorPosicion = reg;
                        }
                        if (reg.nRegistro > infoProceso.MayorPosicion.nRegistro)
                        {
                            infoProceso.MayorPosicion = reg;
                        }
                        if (reg.nEscritura < infoProceso.PrimeraEscritura.nEscritura)
                        {
                            infoProceso.PrimeraEscritura = reg;
                        }
                        if (reg.nEscritura > infoProceso.UltimaEscritura.nEscritura)
                        {
                            infoProceso.UltimaEscritura = reg;
                        }
                    }
                    infoProceso.nEscrituras++;
                }
            }
            offset += leidos;
            memset(bufferescrituras, 0, sizeof(bufferescrituras));
            leidos = mi_read(rutaPrueba, bufferescrituras, offset, sizeof(bufferescrituras));
        }
        char textoFinal[1024];
        memset(textoFinal, 0, sizeof(textoFinal));
        char texto[1024];
        char tiempo[100];
        struct tm *tm;
        sprintf(texto, "PID: %d \n Numero de escritura: %d\n", infoProceso.pid, infoProceso.nEscrituras);
        strcat(textoFinal, texto);
        tm = localtime(&infoProceso.PrimeraEscritura.fecha);
        sprintf(tiempo, "%d%02d%02d%02d%02d%02d",
                tm->tm_year + 1900,
                tm->tm_mon + 1,
                tm->tm_mday,
                tm->tm_hour,
                tm->tm_min,
                tm->tm_sec);
        sprintf(texto, "Primera escritura \t%d \t%d \t%s \n", infoProceso.PrimeraEscritura.nEscritura, infoProceso.PrimeraEscritura.nRegistro, tiempo);
        strcat(textoFinal, texto);
        tm = localtime(&infoProceso.PrimeraEscritura.fecha);
        sprintf(tiempo, "%d%02d%02d%02d%02d%02d",
                tm->tm_year + 1900,
                tm->tm_mon + 1,
                tm->tm_mday,
                tm->tm_hour,
                tm->tm_min,
                tm->tm_sec);
        sprintf(texto, "Ultima escritura \t%d \t%d \t%s \n", infoProceso.UltimaEscritura.nEscritura, infoProceso.UltimaEscritura.nRegistro, tiempo);
        strcat(textoFinal, texto);
        tm = localtime(&infoProceso.PrimeraEscritura.fecha);
        sprintf(tiempo, "%d%02d%02d%02d%02d%02d",
                tm->tm_year + 1900,
                tm->tm_mon + 1,
                tm->tm_mday,
                tm->tm_hour,
                tm->tm_min,
                tm->tm_sec);
        sprintf(texto, "Menor posicion \t%d \t%d \t%s \n", infoProceso.MenorPosicion.nEscritura, infoProceso.MenorPosicion.nRegistro, tiempo);
        strcat(textoFinal, texto);
        tm = localtime(&infoProceso.PrimeraEscritura.fecha);
        sprintf(tiempo, "%d%02d%02d%02d%02d%02d",
                tm->tm_year + 1900,
                tm->tm_mon + 1,
                tm->tm_mday,
                tm->tm_hour,
                tm->tm_min,
                tm->tm_sec);

        sprintf(texto, "Mayor posicion \t%d \t%d \t%s \n", infoProceso.MayorPosicion.nEscritura, infoProceso.MayorPosicion.nRegistro, tiempo);
        strcat(textoFinal, texto);
        fprintf(stdout, BLUE "[HILO: %d] %d ESCRITURAS VALIDADADAS EN %s\n" RESET, i, infoProceso.nEscrituras, rutaPrueba);     
        if (mi_write(rutaInforme, textoFinal, offsetTexto, strlen(textoFinal)) < 0)
        {
            fprintf(stderr, RED "Error al escribir en informe.txt\n" RESET);
        }
        offsetTexto += strlen(textoFinal);
        memset(textoFinal, 0, sizeof(texto));
    }

    if (bumount() < 0)
    {
        fprintf(stderr, RED "" RESET);
    }
    return EXITO;
}