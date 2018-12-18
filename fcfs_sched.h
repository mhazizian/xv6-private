// #include "proc.h"
struct proc;

struct proc_queue
{
	struct proc* procs[NPROC];
	unsigned int head;
	unsigned int tail;
};