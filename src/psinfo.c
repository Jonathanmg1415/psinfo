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
    fprintf(out, "PID: %d\n", p->pid);
    fprintf(out, "Nombre: %s\n", p->name);
    fprintf(out, "Estado: %s\n", p->state);
    fprintf(out, "Tamaño total de la imagen de memoria (VmSize): %d KB\n", p->vm_size);
    fprintf(out, "Tamaño de la memoria de código (VmExe): %d KB\n", p->vm_exe);
    fprintf(out, "Tamaño de la memoria de datos (VmData): %d KB\n", p->vm_data);
    fprintf(out, "Tamaño de la pila (VmStk): %d KB\n", p->vm_stk);
    fprintf(out, "Cambios de contexto: voluntarios = %d, no voluntarios = %d\n\n",
            p->voluntary_ctxt_switches,
            p->nonvoluntary_ctxt_switches);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Uso: %s [-l|-r] [-o archivo] <pid1> [pid2] [...]\n", argv[0]);
        return 1;
    }

    int is_list = 0, is_report = 0;
    char *output_file = NULL;

    // Parseo de opciones
    int i = 1;
    for (; i < argc; i++)
    {
        if (strcmp(argv[i], "-l") == 0)
            is_list = 1;
        else if (strcmp(argv[i], "-r") == 0)
            is_report = 1;
        else if (strcmp(argv[i], "-o") == 0)
        {
            if (i + 1 < argc)
            {
                output_file = argv[++i];
            }
            else
            {
                fprintf(stderr, "Falta el nombre del archivo después de -o\n");
                return 1;
            }
        }
        else
        {
            break; // primer PID encontrado
        }
    }

    if (!is_list && !is_report)
        is_list = 1; // por defecto

    int count = argc - i;
    if (count <= 0)
    {
        fprintf(stderr, "Debe proporcionar al menos un PID.\n");
        return 1;
    }

    ProcessInfo processes[count];
    for (int j = 0; j < count; j++)
    {
        pid_t pid = atoi(argv[i + j]);
        processes[j].pid = pid;

        if (read_process_info(pid, &processes[j]) != 0)
        {
            fprintf(stderr, "No se pudo leer info del PID %d\n", pid);
        }
    }

    // Salida a archivo personalizado con -o
    if (output_file)
    {
        FILE *out = fopen(output_file, "w");
        if (!out)
        {
            perror("Error al abrir archivo de salida");
            return 1;
        }

        for (int j = 0; j < count; j++)
        {
            print_process_info(&processes[j], out);
        }

        fclose(out);
        printf("Archivo de salida generado: %s\n", output_file);
    }
    // Salida automática en archivo (modo -r)
    else if (is_report)
    {
        char filename[256] = "psinfo-report";
        for (int j = 0; j < count; j++)
        {
            char pid_str[16];
            sprintf(pid_str, "-%d", processes[j].pid);
            strcat(filename, pid_str);
        }
        strcat(filename, ".info");

        FILE *out = fopen(filename, "w");
        if (!out)
        {
            perror("Error al crear el archivo de salida");
            return 1;
        }

        for (int j = 0; j < count; j++)
        {
            print_process_info(&processes[j], out);
        }

        fclose(out);
        printf("Archivo de salida generado: %s\n", filename);
    }
    // Salida por pantalla
    else
    {
        printf("-- Información recolectada --\n");
        for (int j = 0; j < count; j++)
        {
            print_process_info(&processes[j], stdout);
        }
    }

    return 0;
}
