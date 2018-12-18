#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "fcfs_sched.h"

struct proc_queue FCFS_queue;

void
init_sched_queue(void)
{
    FCFS_queue.head = 0;
    FCFS_queue.tail = 0;
}

void
add_to_fcfs_sched(struct proc* p)
{
	// complete thid func.
    // is lock required?
    FCFS_queue.procs[FCFS_queue.head] = p;
    FCFS_queue.head++;
}

struct proc*
get_from_fcfs_sched(void)
{
    // is lock required?
    return FCFS_queue.procs[FCFS_queue.tail++];
}

