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
init_rw_pw_lock(struct rwlock *lock)
{
	lock->function_lock = 0;
	lock->resource = 1;
	lock->read_count = 0;
	lock->write_count = 0;
	lock->write_lock = 0;
	lock->pid = 0;
}

void
acquire_pw_writer(struct rwlock *lock)
{
	cprintf("** W: AC: Going_AC, pid=%d\n", myproc()->pid);

	// atomic incrementer
	fetch_and_add(&lock->write_count, 1);

	while (xchg(&lock->resource, 0) != 1) {
		sleep(lock, '\0');
		 cprintf("@@@@ W: AC: Retry Going_AC_resource, pid=%d\n", myproc()->pid);
	}

	while(xchg(&lock->function_lock, 1) != 0) {
		sleep(&lock->function_lock, '\0');
		 cprintf("@@@@ W: AC: Retry Going_AC_func_lock, pid=%d\n", myproc()->pid);
	}

	lock->pid = myproc()->pid;
	lock->write_lock = 1;
	lock->write_count--;

	cprintf("** W: AC: AC_Done, pid=%d, resource=%d\n", myproc()->pid, lock->resource);
	lock->function_lock = 0;

	wakeup(lock);
	wakeup(&lock->function_lock);
}

void
release_pw_writer(struct rwlock *lock)
{
	cprintf("** W: RE: Going to Release, pid=%d.\n", myproc()->pid);

	while(xchg(&(lock->function_lock), 1) != 0) {
		sleep(&lock->function_lock, '\0');
		 cprintf("@@@@ W: RE: Retry: Going to Release, pid=%d\n", myproc()->pid);
	}

	lock->pid = 0;

	lock->resource = 1;
	lock->write_lock = 0;

	cprintf("** W: RE: Released, pid=%d, resource=%d\n", myproc()->pid, lock->resource);
	lock->function_lock = 0;
	wakeup(lock);
	wakeup(&lock->function_lock);
}

void
acquire_pw_reader(struct rwlock *lock)
{
	cprintf("__ R: AC: Going to AC, pid=%d\n", myproc()->pid);

	while(lock->write_lock || (lock->write_count > 0))
	{
		sleep(lock, '\0');
		 cprintf("@@@@ R: AC: Retry: Going to AC, pid=%d\n", myproc()->pid);
	}

	while(xchg(&lock->function_lock, 1) != 0) {
		sleep(&lock->function_lock, '\0');
		 cprintf("@@@@ R: AC: Retry: Going to AC, pid=%d\n", myproc()->pid);
	}


	lock->read_count++;
	lock->resource = 0;
	lock->pid = myproc()->pid;

	cprintf("__ R: AC: AC_Done, pid=%d\n", myproc()->pid);

	lock->function_lock = 0;
	wakeup(lock);
	wakeup(&lock->function_lock);
}

void
release_pw_reader(struct rwlock *lock)
{
	cprintf("__ R: RE: Going to Release, pid=%d, reader_count=%d\n", myproc()->pid, lock->read_count);

	while(xchg(&lock->function_lock, 1) != 0) {
		sleep(&lock->function_lock, '\0');
		 cprintf("@@@@ R: RE: Retry: Going to Release, pid=%d\n", myproc()->pid);
	}


	if (--lock->read_count == 0)
	{
		lock->resource = 1;
		lock->pid = myproc()->pid;
	}
	
	cprintf("__ R: RE: Released, pid=%d, reader_count=%d\n", myproc()->pid, lock->read_count);
	lock->function_lock = 0;
	wakeup(lock);
	wakeup(&lock->function_lock);
}
