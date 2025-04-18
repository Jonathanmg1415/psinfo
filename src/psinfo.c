#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 256

void leer_status(const char *pid)
{
    char ruta[BUFFER_SIZE];
    snprintf(ruta, sizeof(ruta), "/proc/%s/status", pid);

    FILE *archivo = fopen(ruta, "r");
    if (!archivo)
    {
        perror("Error al abrir el archivo de status");
        exit(1);
    }

    char linea[BUFFER_SIZE];
    char nombre[BUFFER_SIZE] = "";
    char estado[BUFFER_SIZE] = "";
    char vm_size[BUFFER_SIZE] = "0";
    char vm_exe[BUFFER_SIZE] = "0";
    char vm_data[BUFFER_SIZE] = "0";
    char vm_stk[BUFFER_SIZE] = "0";
    char ctxt_vol[BUFFER_SIZE] = "0";
    char ctxt_novol[BUFFER_SIZE] = "0";

    while (fgets(linea, sizeof(linea), archivo))
    {
        if (strncmp(linea, "Name:", 5) == 0)
        {
            sscanf(linea, "Name:\t%255s", nombre);
        }
        else if (strncmp(linea, "State:", 6) == 0)
        {
            sscanf(linea, "State:\t%255[^\n]", estado);
        }
        else if (strncmp(linea, "VmSize:", 7) == 0)
        {
            sscanf(linea, "VmSize:\t%255s", vm_size);
        }
        else if (strncmp(linea, "VmExe:", 6) == 0)
        {
            sscanf(linea, "VmExe:\t%255s", vm_exe);
        }
        else if (strncmp(linea, "VmData:", 7) == 0)
        {
            sscanf(linea, "VmData:\t%255s", vm_data);
        }
        else if (strncmp(linea, "VmStk:", 6) == 0)
        {
            sscanf(linea, "VmStk:\t%255s", vm_stk);
        }
        else if (strncmp(linea, "voluntary_ctxt_switches:", 25) == 0)
        {
            sscanf(linea, "voluntary_ctxt_switches:\t%255s", ctxt_vol);
        }
        else if (strncmp(linea, "nonvoluntary_ctxt_switches:", 28) == 0)
        {
            sscanf(linea, "nonvoluntary_ctxt_switches:\t%255s", ctxt_novol);
        }
    }

    fclose(archivo);

    printf("Nombre del proceso: %s\n", nombre);
    printf("Estado: %s\n", estado);
    printf("Tamaño total de la imagen de memoria: %s KB\n", vm_size);
    printf("Tamaño de la memoria TEXT: %s KB\n", vm_exe);
    printf("Tamaño de la memoria DATA: %s KB\n", vm_data);
    printf("Tamaño de la memoria STACK: %s KB\n", vm_stk);
    printf("Número de cambios de contexto (voluntarios - no voluntarios): %s - %s\n",
           ctxt_vol, ctxt_novol);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Uso: %s <PID>\n", argv[0]);
        return 1;
    }

    leer_status(argv[1]);
    return 0;
}
