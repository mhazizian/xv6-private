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

unsigned int temp = 0;

void
acquireticket(struct ticketlock *lk)
{
	// int temp;
	int my_ticket = fetch_and_add(&lk->next_ticket, 1);
//	cprintf("going to acuqire, my_ticket: %d\n", my_ticket);

	while(lk->now_serving_ticket != my_ticket)
//		 temp = temp * lk->next_ticket - lk->pid;
		cprintf("____now serving: %d vs. my ticket: %d\n", lk->now_serving_ticket, my_ticket);

//	cprintf("acquire: ticket: %d, next_ticket: %d, pointer: %p\n", my_ticket, lk->next_ticket, &(*lk));
	
//	while (lk->locked) {
//		sleep(lk, &lk->lk);
//	}

	lk->pid = myproc()->pid;

}

void
releaseticket(struct ticketlock *lk)
{

	if (lk->pid == myproc()->pid)
	{
//		cprintf("release: ticket: %d\n", lk->now_serving_ticket);
		lk->pid = 0;
		// fetch_and_add(&lk->now_serving_ticket, 1);
		lk->now_serving_ticket++;
//		cprintf("release done: now serving is: %d\n", lk->now_serving_ticket);
//		wakeup(lk);
	}
	else
		cprintf("Failed to release the lock!\n");

}
