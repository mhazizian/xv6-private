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
	initlock((struct spinlock*)&ticket->lk, "ticket lock");
	ticket->locked = 0;
	ticket->pid = 0;
	ticket->next_ticket = 0;
	ticket->now_serving_ticket = 0;
}

void
acquireticket(struct ticketlock *lk)
{
	acquire((struct spinlock*)&lk->lk);
	int my_ticket = ++(lk->next_ticket);

	while(lk->now_serving_ticket != my_ticket);
//	while (lk->locked) {
//		sleep(lk, &lk->lk);
//	}
	lk->locked = 1;
	lk->pid = myproc()->pid;
	release((struct spinlock*)&lk->lk);
}

void
releaseticket(struct ticketlock *lk)
{
	acquire((struct spinlock*)&lk->lk);
	if (lk->pid == myproc()->pid)
	{
		lk->locked = 0;
		lk->pid = 0;
		lk->now_serving_ticket++;
//		wakeup(lk);
	}
	release((struct spinlock*)&lk->lk);
}
