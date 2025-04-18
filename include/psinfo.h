#ifndef PSINFO_H
#define PSINFO_H

typedef struct
{
    int pid;
    char name[256];
    char state[32];
    int vm_size;
    int vm_exe;
    int vm_data;
    int vm_stk;
    int voluntary_ctxt_switches;
    int nonvoluntary_ctxt_switches;
} ProcessInfo;

int read_process_info(int pid, ProcessInfo *pinfo);
void print_process_info(const ProcessInfo *pinfo);

#endif
