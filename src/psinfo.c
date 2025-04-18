#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/psinfo.h"

int read_process_info(int pid, ProcessInfo *pinfo)
{
    char path[64];
    FILE *file;

    sprintf(path, "/proc/%d/status", pid);
    file = fopen(path, "r");
    if (!file)
    {
        perror("Error al abrir el archivo /proc/[pid]/status");
        return -1;
    }

    pinfo->pid = pid;
    char line[512];

    while (fgets(line, sizeof(line), file))
    {
        if (strncmp(line, "Name:", 5) == 0)
            sscanf(line, "Name:\t%255s", pinfo->name);
        else if (strncmp(line, "State:", 6) == 0)
            sscanf(line, "State:\t%31[^\n]", pinfo->state);
        else if (strncmp(line, "VmSize:", 7) == 0)
            sscanf(line, "VmSize:\t%d", &pinfo->vm_size);
        else if (strncmp(line, "VmExe:", 6) == 0)
            sscanf(line, "VmExe:\t%d", &pinfo->vm_exe);
        else if (strncmp(line, "VmData:", 7) == 0)
            sscanf(line, "VmData:\t%d", &pinfo->vm_data);
        else if (strncmp(line, "VmStk:", 6) == 0)
            sscanf(line, "VmStk:\t%d", &pinfo->vm_stk);
        else if (strncmp(line, "voluntary_ctxt_switches", 24) == 0)
            sscanf(line, "voluntary_ctxt_switches:\t%d", &pinfo->voluntary_ctxt_switches);
        else if (strncmp(line, "nonvoluntary_ctxt_switches", 27) == 0)
            sscanf(line, "nonvoluntary_ctxt_switches:\t%d", &pinfo->nonvoluntary_ctxt_switches);
    }

    fclose(file);
    return 0;
}

void print_process_info(const ProcessInfo *pinfo)
{
    printf("Pid: %d\n", pinfo->pid);
    printf("Nombre del proceso: %s\n", pinfo->name);
    printf("Estado: %s\n", pinfo->state);
    printf("Tamaño total de la imagen de memoria: %d KB\n", pinfo->vm_size);
    printf("Tamaño de la memoria TEXT: %d KB\n", pinfo->vm_exe);
    printf("Tamaño de la memoria DATA: %d KB\n", pinfo->vm_data);
    printf("Tamaño de la memoria STACK: %d KB\n", pinfo->vm_stk);
    printf("Número de cambios de contexto (voluntarios - no voluntarios): %d - %d\n",
           pinfo->voluntary_ctxt_switches, pinfo->nonvoluntary_ctxt_switches);
    printf("\n");
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Uso: %s [-l] <pid1> [pid2] [...]\n", argv[0]);
        return 1;
    }

    int is_list = strcmp(argv[1], "-l") == 0;
    int start = is_list ? 2 : 1;
    int count = argc - start;

    if (count <= 0)
    {
        fprintf(stderr, "Debe proporcionar al menos un PID.\n");
        return 1;
    }

    ProcessInfo *infos = malloc(count * sizeof(ProcessInfo));
    if (!infos)
    {
        perror("Error de memoria");
        return 1;
    }

    printf("-- Información recolectada!!!\n");

    for (int i = 0; i < count; ++i)
    {
        int pid = atoi(argv[start + i]);
        if (read_process_info(pid, &infos[i]) == 0)
        {
            print_process_info(&infos[i]);
        }
        else
        {
            fprintf(stderr, "No se pudo leer información del PID %d\n", pid);
        }
    }

    free(infos);
    return 0;
}
