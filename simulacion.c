#include "simulacion.h"

static int acabados = 0;
void reaper()
{
    pid_t ended;
    signal(SIGCHLD, reaper);
    ended = waitpid(-1, NULL, WNOHANG);
    while (ended > 0)
    {
        acabados++;
        ended = waitpid(-1, NULL, WNOHANG);
    }
}

int main(int argc, char *argv[])
{
    signal(SIGCHLD, reaper);
    if (argc < 2)
    {
        fprintf(stderr, RED "ERROR, USO: ./simulacion <disco>\n" RESET);
        return FALLO;
    }

    printf("\n*** SIMULACIÓN DE %d PROCESOS REALIZANDO CADA UNA %d ESCRITURAS***\n\n", NUM_PROCESOS, NUM_ESCRITURAS);

    char *disco = argv[1];
    if (bmount(disco) < 0)
    {
        fprintf(stderr, "[simulacion.c] --> Error al montar el dispositivo \n");
        return FALLO;
    }

    time_t timer;
    struct tm *tm;
    char formateo[100];
    timer = time(NULL);
    tm = localtime(&timer);
    sprintf(formateo, "%d%02d%02d%02d%02d%02d", tm->tm_year + 1900, tm->tm_mon + 1,
            tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    char buf[120];
    strcat(buf, "/simul_");
    strcat(buf, formateo);
    strcat(buf, "/");
    if (mi_creat(buf, 6) == FALLO)
    {
        fprintf(stderr, RED "[simulacion.c] --> Error al crear el directorio \n" RESET);
        bumount();
        return FALLO;
    }

    fprintf(stderr, BLUE "[simulacion.c] --> Creado el directorio '%s'.\n" RESET, buf);

    for (int proceso = 1; proceso <= NUM_PROCESOS; proceso++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            if (bmount(disco) < 0)
            {
                fprintf(stderr, RED "[Proceso %d] --> Error al montar el dispositivo \n" RESET, proceso);
                exit(EXIT_FAILURE);
            }

            char dirProceso[200];
            sprintf(dirProceso, "%sproceso_%d/", buf, getpid());
            if (mi_creat(dirProceso, 6) < 0)
            {
                fprintf(stderr, RED "[Proceso %d] --> Error al crear el subdirectorio \n" RESET, proceso);
                bumount();
                exit(EXIT_FAILURE);
            }
            char rutaFichero[256];
            strcpy(rutaFichero, dirProceso);
            strcat(rutaFichero, "prueba.dat");
            if (mi_creat(rutaFichero, 6) < 0)
            {
                fprintf(stderr, RED "[Proceso %d] --> Error al crear prueba.dat path: %s \n" RESET, proceso,rutaFichero);
                bumount();
                exit(EXIT_FAILURE);
            }
            srand(time(NULL) + getpid());
            for (int j = 1; j <= NUM_ESCRITURAS; j++)
            {
                struct REGISTRO reg;
                reg.fecha = time(NULL);
                reg.pid = getpid();
                reg.nEscritura = j;
                reg.nRegistro = rand() % REGMAX;

                int offset = reg.nRegistro * sizeof(struct REGISTRO);
                if (mi_write(rutaFichero, &reg, offset, sizeof(struct REGISTRO)) < 0)
                {
                    fprintf(stderr, RED "[Proceso %d] --> Error al escribir en prueba.dat path: %s --\n" RESET, proceso,rutaFichero);
                    
                }
                printf("[Proceso %d]: Completadas %d escrituras en %s]\n", proceso, j, rutaFichero);
                usleep(50000); // 0.05 
            }
            if (bumount() < 0)
            {
                fprintf(stderr, RED "ERROR AL DESMONTAR EL SISTEMA DE FICHEROS \n" RESET);
                return FALLO;
            }
            exit(0);
        }
        usleep(150000); // 0.15
    }
    while (acabados < NUM_PROCESOS)
    {
        pause();
    }

    if (bumount() < 0)
    {
        fprintf(stderr, RED "ERROR AL DESMONTAR EL SISTEMA DE FICHEROS \n" RESET);
        return FALLO;
    }
    printf(GREEN "Simulación terminada. Procesos terminados: %d\n" RESET, acabados);

    return EXITO;
}
