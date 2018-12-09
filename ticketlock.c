// Ticket locks

#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "ticketlock.h"

void
initticketlock(struct ticketlock *ticket)
{
	ticket->pid = 0;
	ticket->next_ticket = 0;
	ticket->now_serving_ticket = 0;
}

void
acquireticket(struct ticketlock *lk)
{
	int my_ticket = fetch_and_add(&lk->next_ticket, 1);
	while (lk->now_serving_ticket < my_ticket) {
		sleep(lk, '\0');
	}
	lk->pid = myproc()->pid;
}

void
releaseticket(struct ticketlock *lk)
{
	// TODO : check this later!
	if (lk->pid == myproc()->pid)
	{
		lk->pid = 0;
		lk->now_serving_ticket++;
		wakeup(lk);
	}
	else
		cprintf("Failed to release the lock!\n");
}
