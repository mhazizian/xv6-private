// Readers-Writers locks with privilage to readers.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "rwlock.h"

void
init_rw_lock(struct rwlock *lock)
{
	lock->function_lock = 0;
	lock->resource = 1;
	lock->read_count = 0;
	lock->write_count = 0;
	lock->write_lock = 0;
	lock->pid = 0;
}

void
acquire_writer(struct rwlock *lock)
{
	cprintf("__W: AC: Going to Enter, pid=%d\n", myproc()->pid);

	while ((xchg(&lock->resource, 0) != 1) || (xchg(&lock->function_lock, 1) != 0))
		sleep(lock, '\0');

	cprintf("__W: AC: Enterd, pid=%d, resource=%d\n", myproc()->pid, lock->resource);

	lock->pid = myproc()->pid;
	lock->write_lock = 1;
	
	lock->function_lock = 0;

	cprintf("__W: AC: Write Entered.\n");
}

void
release_writer(struct rwlock *lock)
{
	cprintf("__W: RE: Going to entry Section, pid=%d\n", myproc()->pid);
	while(xchg(&lock->function_lock, 1) != 0)
		sleep(lock, '\0');

	cprintf("__W: RE: Enterd entry Section, pid=%d, resource=%d\n", myproc()->pid, lock->resource);

	lock->pid = 0;

	
	lock->resource = 1;
	lock->write_lock = 0;

	lock->function_lock = 0;
	wakeup(lock);
	
	cprintf("__W: RE: Write Released.\n");
}

void
acquire_reader(struct rwlock *lock)
{
	cprintf("__R: AC: Going to entry Section, pid=%d\n", myproc()->pid);

	while(xchg(&lock->function_lock, 1) != 0 || (lock->write_lock))
		sleep(lock, '\0');

	cprintf("__R: AC: Enterd, pid=%d\n", myproc()->pid);

	lock->read_count++;
	lock->resource = 0;
	lock->pid = myproc()->pid;

	lock->function_lock = 0;
}

void
release_reader(struct rwlock *lock)
{
	cprintf("__R: RE: Going to entry Section, pid=%d\n", myproc()->pid);

	while(xchg(&lock->function_lock, 1) != 0)
		sleep(lock, '\0');

	cprintf("__R: RE: Enterd, pid=%d\n", myproc()->pid);

	if (--lock->read_count == 0)
	{
		lock->resource = 1;
		lock->pid = myproc()->pid;
	}

	lock->function_lock = 0;
}
