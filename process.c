#define _GNU_SOURCE

#include <sched.h>  // schedule thing
#include <errno.h>  // error message
#include <unistd.h> // lots of func

#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>  // gettimeofday have to delete // not use 
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/sysinfo.h> // get cpu information

#include "process.h"

#define GET_TIME 334

int assign_proc_cpu(pid_t pid, int cpu)
{
    int CPU_NUM = get_nprocs();
    if (cpu > CPU_NUM)
    {
        fprintf(stderr, "Assign CPU has wrong, max : %d  min : %d", CPU_NUM, cpu);
        return -1;
    }    

    cpu_set_t cpusetp;
    CPU_ZERO(&cpusetp);
    CPU_SET(cpu, &cpusetp);


    // int sched_setaffinity(pid_t pid, size_t cpusetsize, cpu_set_t *mask);
    if (sched_setaffinity(pid, CPU_NUM, &cpusetp) < 0)
    {
        perror("SCHED_SETAFFINITY");
        exit(EXIT_FAILURE);
    }

    return 0;
}


int execution(struct process proc)
{
    // return -1, 0, < 0
    pid_t PID = fork();    


    switch(PID){
        case -1:    // error
            perror("FORK");
            exit(EXIT_FAILURE);
        case 0:     // child
        {
            unsigned long start_sec, start_nsec;
            pid_t pid = getpid();

            // block(pid);
            printf("%d\n", pid);
        
            syscall(GET_TIME, 1, pid, &start_sec, &start_nsec);

            WAIT_UNIT(proc.exec_time);

            syscall(GET_TIME, 0, pid, &start_sec, &start_nsec);
            
            exit(EXIT_SUCCESS);
        }
        default:    // parent
            assign_proc_cpu(PID, CHILD_CPU);
    }


    // Just parent process return child pid
    return PID;
}

int block(pid_t pid)
{
    struct sched_param param;
    // SCHED_OTHER, SCHED_IDLE, SCHED_BATCH
    // sched_priority is not used in scheduling decisions 
    // It MUST be specified as 0
    param.sched_priority = 0;

    // SCHED_FIFO = SCHED_RR > SCHED_OTHER ~ SCHED_BATCH > SCHED_IDLE

    // success return 0, error return -1
    int set = sched_setscheduler(pid, SCHED_IDLE, &param);

    if(set != 0) perror("BLOCK SCHED_SETSCHEDULER");

    return set;
}

int wakeup(pid_t pid)
{
    struct sched_param param;
    // param.sched_priority = 0;
    param.sched_priority = sched_get_priority_max(SCHED_FIFO);

    // wakeup but run directly
    int set = sched_setscheduler(pid, SCHED_FIFO, &param);

    if(set != 0) perror("WAKEUP SCHED_SETSCHEDULER");

    return set;
}