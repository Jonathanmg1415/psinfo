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

void print_process_info(ProcessInfo *p, FILE *out)
{
    fprintf(out, "Pid: %d\n", p->pid);
    fprintf(out, "Nombre del proceso: %s\n", p->name);
    fprintf(out, "Estado: %s\n", p->state);
    fprintf(out, "Tamaño total de la imagen de memoria: %s KB\n", p->vmSize);
    fprintf(out, "Tamaño de la memoria TEXT: %s KB\n", p->vmExe);
    fprintf(out, "Tamaño de la memoria DATA: %s KB\n", p->vmData);
    fprintf(out, "Tamaño de la memoria STACK: %s KB\n", p->vmStk);
    fprintf(out, "Número de cambios de contexto (voluntarios - no voluntarios): %s - %s\n\n",
            p->voluntary_ctxt_switches,
            p->nonvoluntary_ctxt_switches);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Uso: %s [-l|-r] <pid1> [pid2] [...]\n", argv[0]);
        return 1;
    }

    int is_list = strcmp(argv[1], "-l") == 0;
    int is_report = strcmp(argv[1], "-r") == 0;
    int start = 2;

    if (!is_list && !is_report)
    {
        start = 1;
        is_list = 1; // por defecto modo lista simple
    }

    int count = argc - start;
    if (count <= 0)
    {
        fprintf(stderr, "Debe proporcionar al menos un PID.\n");
        return 1;
    }

    ProcessInfo processes[count];

    for (int i = 0; i < count; i++)
    {
        pid_t pid = atoi(argv[start + i]);
        processes[i].pid = pid;

        if (read_process_info(pid, &processes[i]) != 0)
        {
            fprintf(stderr, "No se pudo leer info del PID %d\n", pid);
        }
    }

    if (is_report)
    {
        // Generar nombre del archivo
        char filename[256] = "psinfo-report";
        for (int i = 0; i < count; i++)
        {
            char pid_str[16];
            sprintf(pid_str, "-%d", processes[i].pid);
            strcat(filename, pid_str);
        }
        strcat(filename, ".info");

        FILE *out = fopen(filename, "w");
        if (!out)
        {
            perror("Error al crear el archivo de salida");
            return 1;
        }

        for (int i = 0; i < count; i++)
        {
            print_process_info(&processes[i], out); // escribir en archivo
        }

        fclose(out);
        printf("Archivo de salida generado: %s\n", filename);
    }
    else
    {
        // Mostrar en pantalla
        printf("-- Información recolectada!!! --\n");
        for (int i = 0; i < count; i++)
        {
            print_process_info(&processes[i], stdout);
        }
    }

    return 0;
}
