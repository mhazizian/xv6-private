// rwlcok for processes

struct rwlock {
	// It's been initialized like semaphores
	// - allow only one writer to enter critical section.
	unsigned int resource;		

	// forbid readers to enter critical section when a writer is writing.
	unsigned int write_lock;

	// for allowing multi reader to enter critical section
	unsigned int read_count;

	// count number of writer waiting to enter critical section
	// (for when writers have more priority than readers.)
	// it has been used for writer privileged implementation.
	unsigned int write_count;

	// for atomic acquire and release functions
	unsigned int function_lock; 

	// Process holding lock
	unsigned int pid;			
};
