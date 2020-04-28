#define _GNU_SOURCE

#include <sched.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#include <stdio.h>
#include <stdlib.h>

#include <sys/wait.h>

#include "process.h"
#include "scheduler.h"

#define TRUE 1


// evey child use common var
static int r_time = 0;      // use to count rr time

static int n_time = 0;

static int running = -1;

static int finish_cnt = 0;



int compare(const void *a, const void *b)
{
    return ((struct process*)a)->ready_time - ((struct process*)b)->ready_time;
}


// have to modify
int next_process(struct process *proc, int num_p, int policy)
{
    // Non-preemptive
    if(running != -1 && (policy == SJF || policy == FIFO))
        return running;

    int num = -1;

    switch(policy)
    {
        case SJF:
        case PSJF:
            for(int i = 0; i < num_p; i++)
            {
                if((proc + i)->pid == -1 || (proc + i)->exec_time == 0)
                    continue;
                if(num == -1 || (proc + i)->exec_time < (proc + num)->exec_time)
                    num = i;
            }
            break;
        case FIFO:
            for(int i = 0; i < num_p; i++)
            {
                if((proc + i)->pid == -1 || (proc + i)->exec_time == 0)
                    continue;
                if(num == -1 || (proc + num)->ready_time < (proc + num)->ready_time)
                {
                    num = i;
                    break;
                } 
            }
            break;
        case RR:
            if(running == -1)
            {
                for(int i = 0; i < num_p; i++)
                {
                    if((proc + i)->pid != -1 && (proc + i)->exec_time > 0)
                    {
                        num = i; 
                        break;
                    }
                }
            }
            else if ((n_time - r_time) % 500 == 0)
            {
                num = (running + 1) % num_p;
                while((proc + num)->pid == -1 || (proc + num)->exec_time == 0)
                    num = (num + 1) % num_p;
            }
            else
                num = running;
    }

    return num;

}


int scheduling(struct process *proc, int num_p, int policy)
{
    // Initial pid = -1 imply not ready
    for(int i = 0; i < num_p; i++)
        (proc + i)->pid = -1;

    // let parent process use own CPU
    assign_proc_cpu(getpid(), PARENT_CPU);

    // let process can be execution SCHED_OTHER
    struct sched_param param;
    param.sched_priority = sched_get_priority_max(SCHED_FIFO);
    sched_setscheduler(getpid(), SCHED_FIFO, &param);
    
    // wakeup(getpid());

    while(TRUE)
    {
        if(running != -1 && (proc + running)->exec_time == 0)
        {
            printf("%s %d\n", (proc + running)->name, (proc + running)->pid);
            
            running = -1;
            finish_cnt++;

            if(finish_cnt == num_p) break;
        }


        // check if process ready and fork
        for(int i = 0; i < num_p; i++)
        {
            if((proc + i)->ready_time == n_time)
            {
                (proc + i)->pid = execution(*(proc + i));
                block((proc + i)->pid);
            }
        }

        // select next running process
        // have to implement
        int next = next_process(proc, num_p, policy);

        if(next != -1 && running != next)
        {
            wakeup((proc + next)->pid);
            block((proc + running)->pid);
            running = next;
            r_time = n_time;
        }

        ONE_UNIT;
        if(running != -1)
            (proc + running)->exec_time--;

        n_time++;
    }

    return 0;
}   