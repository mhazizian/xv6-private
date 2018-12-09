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
entry_section(struct rwlock *lock)
{
	while(xchg(&lock->function_lock, 1) != 0)
		sleep(lock, '\0');
	// while(lock->function_lock == 1)
	// 	sleep(lock, '\0');
	// lock->function_lock = 1;
}

void
init_rw_lock(struct rwlock *lock)
{
	lock->function_lock = 0;
	lock->resource = 1;
	lock->read_count = 0;
	lock->pid = 0;
}

void
acquire_writer(struct rwlock *lock)
{
	entry_section(lock);

	while (!lock->resource)
		sleep(lock, '\0');

	lock->resource = 0;
	lock->pid = myproc()->pid;

	lock->function_lock = 0;
}

void
release_writer(struct rwlock *lock)
{
	entry_section(lock);

	if (lock->pid == myproc()->pid)
	{
		lock->pid = 0;
		lock->resource = 1;
		wakeup(lock);
	}
	else
		cprintf("Failed to release the lock!\n");

	lock->function_lock = 0;
}

void
acquire_reader(struct rwlock *lock)
{
	entry_section(lock);

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
release_reader(struct rwlock *lock)
{
	entry_section(lock);

	if (lock->pid == myproc()->pid)
	{
		if (--lock->read_count == 0)
		{
			// while (!lock->resource)
			// 	sleep(lock, '\0');
			// lock->resource = 0;
			lock->resource = 1;
			lock->pid = myproc()->pid;
		}
	}
	else
		cprintf("Failed to release the lock!\n");

	lock->function_lock = 0;
}
