// Readers-Writers locks

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
entry_pw_section(struct rwlock *lock)
{
	while(xchg(&lock->function_lock, 1) != 0)
		sleep(lock, '\0');
}

void
init_rw_pw_lock(struct rwlock *lock)
{
	lock->function_lock = 0;
	lock->resource = 1;
	lock->read_count = 0;
    lock->write_count = 0;
	lock->pid = 0;
}

void
acquire_pw_writer(struct rwlock *lock)
{
	cprintf("__W: AC: Going to Enter, pid=%d\n", myproc()->pid);
	// entry_pw_section(lock);
	// cprintf("W: AC: Enterd entry Section, pid=%d, resource=%d\n", myproc()->pid, lock->resource);
    lock->write_count++;
    
	while ((xchg(&lock->resource, 0) != 1) || (xchg(&lock->function_lock, 1) != 0))
		sleep(lock, '\0');

	cprintf("__W: AC: Enterd, pid=%d, resource=%d\n", myproc()->pid, lock->resource);
    lock->write_count--;
	lock->pid = myproc()->pid;
	lock->function_lock = 0;
	cprintf("Write Entered.\n");
}

void
release_pw_writer(struct rwlock *lock)
{
	cprintf("__W: RE: Going to entry Section, pid=%d\n", myproc()->pid);
	entry_pw_section(lock);
	cprintf("__W: RE: Enterd entry Section, pid=%d, resource=%d\n", myproc()->pid, lock->resource);

	lock->pid = 0;
	lock->resource = 1;
    lock->function_lock = 0;

	wakeup(lock);

	cprintf("Write Released.\n");
}

void
acquire_pw_reader(struct rwlock *lock)
{
    while(xchg(&lock->function_lock, 1) != 0 || lock->write_count > 0)
		sleep(lock, '\0');

	if (++lock->read_count == 1)
	{
		while (!lock->resource)
			sleep(lock, '\0');
		lock->resource = 0;
		lock->pid = myproc()->pid;
	}

	lock->function_lock = 0;
}

void
release_pw_reader(struct rwlock *lock)
{
	entry_pw_section(lock);

	if (--lock->read_count == 0)
	{
		// while (!lock->resource)
		// 	sleep(lock, '\0');
		// lock->resource = 0;
		lock->resource = 1;
		lock->pid = myproc()->pid;
	}

	lock->function_lock = 0;
}
