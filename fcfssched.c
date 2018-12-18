#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "fcfssched.h"

struct proc_queue FCFS_queue;
struct spinlock fcfs_lock;

void
init_sched_queue(void)
{
    FCFS_queue.head = 0;
    FCFS_queue.tail = 0;
    cprintf("salam\n");
}


void
add_to_fcfs_sched(struct proc* p)
{
    acquire(&fcfs_lock);

    FCFS_queue.procs[FCFS_queue.head] = p;
    FCFS_queue.head = (FCFS_queue.head + 1) % NPROC;

    release(&fcfs_lock);
}

struct proc*
get_from_fcfs_sched(void)
{
    // acquire(&fcfs_lock);

    struct proc* p = FCFS_queue.procs[FCFS_queue.tail];
    FCFS_queue.tail = (FCFS_queue.tail + 1) % NPROC;

    // release(&fcfs_lock);

    return p;
}

int
fcfs_is_empty(void)
{
    // acquire(&fcfs_lock);

    int res = (FCFS_queue.head == FCFS_queue.tail);

    // release(&fcfs_lock);

    return res;
}



