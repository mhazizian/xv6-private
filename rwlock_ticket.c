// Readers-Writers locks with privilage to readers.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "rwlock_ticket.h"

void
init_rw_ticket_lock(struct rwlock_ticket *lock)
{
    initticketlock(&lock->mutex);
    initticketlock(&lock->wrt);

    lock->reader_count = 0;
}

void
acquire_ticket_writer(struct rwlock_ticket *lock)
{
	cprintf("** W: AC: Going_AC, pid=%d\n", myproc()->pid);

    acquireticket(&lock->wrt);

	cprintf("** W: AC: AC_Done, pid=%d\n", myproc()->pid);
}

void
release_ticket_writer(struct rwlock_ticket *lock)
{
	cprintf("** W: RE: Going to Release, pid=%d.\n", myproc()->pid);

    releaseticket(&lock->wrt);

	cprintf("** W: RE: Released, pid=%d\n", myproc()->pid);    
}

void
acquire_ticket_reader(struct rwlock_ticket *lock)
{
	cprintf("__ R: AC: Going to AC, pid=%d\n", myproc()->pid);

    acquireticket(&lock->mutex);

    if (++lock->reader_count == 1)
        acquireticket(&lock->wrt);

    releaseticket(&lock->mutex);
    
	cprintf("__ R: AC: AC_Done, pid=%d\n", myproc()->pid);

}

void
release_ticket_reader(struct rwlock_ticket *lock)
{
	cprintf("__ R: RE: Going to Release, pid=%d\n", myproc()->pid);
    
    acquireticket(&lock->mutex);

    if (--lock->reader_count == 0)
        releaseticket(&lock->wrt);

    releaseticket(&lock->mutex);

	cprintf("__ R: RE: Released, pid=%d\n", myproc()->pid);
}
