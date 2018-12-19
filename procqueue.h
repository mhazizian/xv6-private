struct proc_queue
{
	struct proc* procs[NPROC];
	uint head;
	uint tail;
};
