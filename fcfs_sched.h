// #include "proc.h"
// struct proc;
// struct spinlock;

struct proc_queue
{
	struct proc* procs[NPROC];
	struct spinlock lock;
	unsigned int head;
	unsigned int tail;
};