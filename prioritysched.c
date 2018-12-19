#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "procqueue.h"

struct proc_queue fcfs_queue;
struct spinlock fcfs_lock;

void
init_sched_queue(void)
{
    fcfs_queue.head = 0;
    fcfs_queue.tail = 0;
}


void
add_to_fcfs_sched(struct proc* p)
{
    acquire(&fcfs_lock);

    fcfs_queue.procs[fcfs_queue.head] = p;
    fcfs_queue.head = (fcfs_queue.head + 1) % NPROC;

    release(&fcfs_lock);
}

struct proc*
get_from_fcfs_sched(void)
{
    struct proc* p = fcfs_queue.procs[fcfs_queue.tail];
    fcfs_queue.tail = (fcfs_queue.tail + 1) % NPROC;

    return p;
}

int
fcfs_is_empty(void)
{
    int res = (fcfs_queue.head == fcfs_queue.tail);
    return res;
}