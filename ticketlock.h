// Ticketlock for processes
struct ticketlock {
	unsigned int pid;					 // Process holding lock
	unsigned int next_ticket;
	unsigned int now_serving_ticket;
};
