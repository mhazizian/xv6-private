// rwlcok for processes

struct rwlock {
	unsigned int resource;		// It's been initialized like semaphores
	unsigned int read_count, write_count;
	unsigned int function_lock;
	unsigned int pid;		// Process holding lock
};
