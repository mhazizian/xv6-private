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
    acquire(&FCFS_queue.lock);

    FCFS_queue.procs[FCFS_queue.head] = p;
    // @TODO user rem with NPROC
    FCFS_queue.head++;

    release(&FCFS_queue.lock);
}

struct proc*
get_from_fcfs_sched(void)
{
    acquire(&FCFS_queue.lock);

    struct proc* p = FCFS_queue.procs[FCFS_queue.tail];
    // @TODO user rem with NPROC
    FCFS_queue.tail++;

    release(&FCFS_queue.lock);
    return p;
}

int
fcfs_is_empty(void)
{
    return (FCFS_queue.head == FCFS_queue.tail);
}



