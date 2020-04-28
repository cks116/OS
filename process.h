#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <sys/types.h>

#define CHILD_CPU 1
#define PARENT_CPU 0

#define ONE_UNIT ({volatile unsigned long i; for(i = 0; i < 1000000UL; i++);})
#define WAIT_UNIT(time)  ({for(size_t i = 0; i < time; i++) ONE_UNIT;})

struct process
{
    char name[2];
    int ready_time;
    int exec_time;
    pid_t pid;
};

int assign_proc_cpu(pid_t pid, int cpu);

int execution(struct process proc);

int block(pid_t pid);

int wakeup(pid_t pid);


#endif