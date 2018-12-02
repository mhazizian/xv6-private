// Mutual exclusion lock.
struct myspinlock {
	uint locked;			 // Is the lock held?

	// For debugging:
	char *name;				// Name of lock.
	struct cpu *cpu;	 // The cpu holding the lock.
	uint pcs[10];			// The call stack (an array of program counters)
	                                                                         // that locked the lock.
};

// Ticketlock for processes
struct ticketlock {
	uint locked;			 // Is the lock held?
	struct myspinlock lk; // spinlock protecting this ticket lock
	int pid;					 // Process holding lock
	int next_ticket;
	int now_serving_ticket;

	// For debugging:
//	int init;
};
