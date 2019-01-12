#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "syscall.h"
//#include "sharedm.h"

void
set_int_argument(int value, int argument_number, int system_call_number)
{
    struct proc* curproc = myproc();
    int pid = curproc->pid;
    cprintf("*** %d\n", pid);

    struct system_call* system_call_struct = &process_system_calls[pid][system_call_number];
    int number_of_calls = (*system_call_struct).number_of_calls;

	(*system_call_struct).system_calls[number_of_calls].arguments[argument_number].type = INT;
	(*system_call_struct).system_calls[number_of_calls].arguments[argument_number].int_value = value;
}

void
set_void_argument(int argument_number, int system_call_number)
{
    struct proc* curproc = myproc();
    int pid = curproc->pid;

    struct system_call* system_call_struct = &process_system_calls[pid][system_call_number];
    int number_of_calls = (*system_call_struct).number_of_calls;

    (*system_call_struct).system_calls[number_of_calls].arguments[argument_number].type = VOID;
}

int
sys_fork(void)
{
        set_void_argument(FIRST, SYS_fork);
	return fork();
}

int
sys_exit(void)
{
        set_void_argument(FIRST, SYS_exit);
	exit();
	return 0;	// not reached
}

int
sys_wait(void)
{
        set_void_argument(FIRST, SYS_wait);
		return wait();
}


int
sys_kill(void)
{
    int pid;

    if(argint(0, &pid) < 0)
	    return -1;

    set_int_argument(pid, FIRST, SYS_kill);
    return kill(pid);
}

int
sys_getpid(void)
{
        set_void_argument(FIRST, SYS_getpid);
	return myproc()->pid;
}

int
sys_sbrk(void)
{
        int addr;
	int n;

	if(argint(0, &n) < 0)
		return -1;
	addr = myproc()->sz;
	if(growproc(n) < 0)
		return -1;

	set_int_argument(n, FIRST, SYS_sbrk);
	return addr;
}

int
sys_sleep(void)
{
	int n;
	uint ticks0;

	if(argint(0, &n) < 0)
		return -1;
	acquire(&tickslock);
	ticks0 = ticks;
	while(ticks - ticks0 < n){
		if(myproc()->killed){
			release(&tickslock);
			return -1;
		}
		sleep(&ticks, &tickslock);
	}
	release(&tickslock);

	return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
        set_void_argument(FIRST, SYS_uptime);
	uint xticks;

	acquire(&tickslock);
	xticks = ticks;
	release(&tickslock);
	return xticks;
}


int
sys_invoked_syscalls(void)
{
    int pid;
    if(argint(0, &pid) < 0)
	    return -1;

    set_int_argument(pid, FIRST, SYS_invoked_syscalls);
    return invoked_syscalls(pid);
}


int sys_shm_open(void)
{
	int id, page_count;
    int  flag;

	if(argint(0, &id) < 0)
		return -1;

    if(argint(1, &page_count) < 0)
        return -1;

    if(argint(2, &flag) < 0)
        return -1;

    shm_open(id, page_count, flag);
	return 0;
}
int sys_shm_attach(void)
{
	int id;

	if(argint(0, &id) < 0)
		return -1;

	shm_attach(id);
	return 0;
}
int sys_shm_close(void)
{
	int id;

	if(argint(0, &id) < 0)
		return -1;

	shm_close(id);
	return 0;
}
