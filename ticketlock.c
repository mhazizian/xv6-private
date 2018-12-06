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
	// initlock((struct spinlock*)&ticket->lk, "ticket lock");
	ticket->locked = 0;
	ticket->pid = 0;
	ticket->next_ticket = 0;
	ticket->now_serving_ticket = 0;
}

//unsigned int temp = 0;

void
acquireticket(struct ticketlock *lk)
{

//	int my_ticket;
//	int val = 1;
	// acquire((struct spinlock*)&lk->lk);
	
//	cprintf("before adding temp: %d\n", temp);
//	fetch_and_add(&temp, 1);
//	cprintf("after adding temp: %d\n", temp);

	int my_ticket = fetch_and_add(&lk->next_ticket, 1);
//	my_ticket = fetch_and_add(&lk->next_ticket, val);
//	int my_ticket = (lk->next_ticket)++;
//	my_ticket = (lk->next_ticket)++;

//	cprintf("lk->next_ticket: %d\n", lk->next_ticket);


	while(lk->now_serving_ticket != my_ticket);
//	cprintf("acquire: lk->next_ticket:%d\n", lk->next_ticket);

	// cprintf("acquire: next_ticket:%d\n", lk->next_ticket);
	
//	while (lk->locked) {
//		sleep(lk, &lk->lk);
//	}
	lk->locked = 1;
	lk->pid = myproc()->pid;
	// release((struct spinlock*)&lk->lk);
}

void
releaseticket(struct ticketlock *lk)
{
	// acquire((struct spinlock*)&lk->lk);
	if (lk->pid == myproc()->pid)
	{
//		cprintf("release: my_ticket:%d\n", lk->now_serving_ticket);
		lk->locked = 0;
		lk->pid = 0;
		fetch_and_add(&lk->now_serving_ticket, 1);
//		cprintf("release: now_serving:%d\n", lk->now_serving_ticket);
//		lk->now_serving_ticket++;
//		wakeup(lk);
	}
	// release((struct spinlock*)&lk->lk);
}
