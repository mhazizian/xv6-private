// rwlcok for processes
#include "ticketlock.h"

struct rwlock_ticket {
	struct ticketlock mutex;
    struct ticketlock wrt;

    int reader_count;

	// Process holding lock, debug purpose
	// unsigned int pid;			
};
