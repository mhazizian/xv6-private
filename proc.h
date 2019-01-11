#include "param.h"
#include "date.h"
// Per-CPU state

#define SYS_CALL_NUMBERS 23 /// @todo move this to appropriate place
#define MAX_ARGUMENTS_NUMBER 3
#define MAX_SYS_CALL_NUMBERS 16
#define MAX_CHARP_SIZE 16
#define MAX_INTP_SIZE 16
#define FIRST 0
#define SECOND 1
#define THIRD 2

enum argument_type {
    VOID,
    INT,
    CHARP
};

struct argumnet
{
     enum argument_type type;
     int int_value;
     char charp_value[MAX_CHARP_SIZE];
};

struct system_call_status {
    struct rtcdate time;
    struct argumnet arguments[MAX_ARGUMENTS_NUMBER];
};

struct system_call {
    int number_of_calls;
    struct system_call_status system_calls[MAX_SYS_CALL_NUMBERS];
};

struct system_call system_calls[SYS_CALL_NUMBERS];
struct system_call process_system_calls[NPROC][SYS_CALL_NUMBERS];

struct cpu {
	uchar apicid;								// Local APIC ID
	struct context *scheduler;	 // swtch() here to enter scheduler
	struct taskstate ts;				 // Used by x86 to find stack for interrupt
	struct segdesc gdt[NSEGS];	 // x86 global descriptor table
	volatile uint started;			 // Has the CPU started?
	int ncli;										// Depth of pushcli nesting.
	int intena;									// Were interrupts enabled before pushcli?
	struct proc *proc;					 // The process running on this cpu or null
};

extern struct cpu cpus[NCPU];
extern int ncpu;

//PAGEBREAK: 17
// Saved registers for kernel context switches.
// Don't need to save all the segment registers (%cs, etc),
// because they are constant across kernel contexts.
// Don't need to save %eax, %ecx, %edx, because the
// x86 convention is that the caller has saved them.
// Contexts are stored at the bottom of the stack they
// describe; the stack pointer is the address of the context.
// The layout of the context matches the layout of the stack in swtch.S
// at the "Switch stacks" comment. Switch doesn't save eip explicitly,
// but it is on the stack and allocproc() manipulates it.
struct context {
	uint edi;
	uint esi;
	uint ebx;
	uint ebp;
	uint eip;
};

enum procstate { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };
// This struct is for kernel not process!!!!
// Per-process state
struct proc {
	int last_system_call;
	uint sz;										 // Size of process memory (bytes)
	pde_t* pgdir;								// Page table
	char *kstack;								// Bottom of kernel stack for this process
	enum procstate state;				// Process state
	int pid;										 // Process ID
	struct proc *parent;				 // Parent process
	struct trapframe *tf;				// Trap frame for current syscall
	struct context *context;		 // swtch() here to run process
	void *chan;									// If non-zero, sleeping on chan
	int killed;									// If non-zero, have been killed
	struct file *ofile[NOFILE];	// Open files
	struct inode *cwd;					 // Current directory
	char name[16];							 // Process name (debugging)
};

// Process memory is laid out contiguously, low addresses first:
//	 text
//	 original data and bss
//	 fixed-size stack
//	 expandable heap
