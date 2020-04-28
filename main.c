#define _GNU_SOURCE

#include <sched.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "process.h"
#include "scheduler.h"


int main(int argc, char **argv)
{
    char sched_policy[4];
    int policy;
    int num_p;
    struct process *proc;

    scanf("%s", sched_policy);
    scanf("%d", &num_p);

    proc = (struct process *)malloc(num_p * sizeof(struct process));

    for(int i = 0; i < num_p; i++) 
    {
        scanf("%s%d%d", (proc + i)->name,
            &(proc + i)->ready_time, &(proc + i)->exec_time);
    }

    switch(sched_policy[0])
    {
        case 'F':
            policy = FIFO;
            break;
        case 'R':
            policy = RR;
            break;
        case 'S':
            policy = SJF;
            break;
        case 'P':
            policy = PSJF;
            break;
        default:
            fprintf(stderr, "Invalid policy : %s", sched_policy);
            exit(EXIT_FAILURE);
    }

    scheduling(proc, num_p, policy);

    exit(EXIT_SUCCESS);
}