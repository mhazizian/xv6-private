// #include "proc.h"
// struct proc;
// struct spinlock;

struct proc_queue
{
	struct proc* procs[NPROC];
	uint head;
	uint tail;
};
