// Ticketlock for processes
struct ticketlock {
	uint locked;			 // Is the lock held?
	struct spinlock lk; // spinlock protecting this sleep lock
	int pid;					 // Process holding lock
	int next_ticket;
	int now_serving_ticket;
};
