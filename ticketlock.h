#ifndef _TICKET_LOCK_H_
#define _TICKET_LOCK_H_

// Ticketlock for processes
struct ticketlock {
	unsigned int pid;					 // Process holding lock
	unsigned int next_ticket;
	unsigned int now_serving_ticket;
};

#endif
