#ifndef _SCHEDULER_
#define _SCHEDULER_

#include "process.h"

#define FIFO 0
#define RR 1
#define SJF 2
#define PSJF 3

int next_process(struct process *proc, int num_p, int policy);

int scheduling(struct process *proc, int num_p, int policy);

#endif