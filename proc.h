#include "param.h"
#include "date.h"
// Per-CPU state

// The SYS_CALL_NUMBERS is one more than number of system calls
#define SYS_CALL_NUMBERS 27
#define MAX_SYSTEM_CALL_NAME_SIZE 32
#define MAX_ARGUMENTS_NUMBER 3
#define MAX_SYS_CALL_NUMBERS 220
#define MAX_CHARP_SIZE 20
#define MAX_CHARPP_SIZE 3
#define SUCCESSFUL 0
#define EMPTY -1
#define FIRST 0
#define SECOND 1
#define THIRD 2
#define ZERO 0
#define ONE 1

enum argument_type {
    VOID = 0,
    INT,
    CHARP,
    POINTER
};

// It has been used to save all arguments passed to a system_call
// type of arguments is stored in argument_type
// int_value is used for storing int agruments
// charp_value is used for storing char* and strings
// all other values of type pointer is stored in pp_value.
struct argument {
     enum argument_type type;
     int int_value;
     char charp_value[MAX_CHARP_SIZE];
     uint p_value;
};

struct system_call_status {
    int syscall_number;	    // ID of system call
    int pid;	// ID of process related to this system_call
    // index of this system_call in sorted_syscalls which is sorted by time of user
    int index_in_sorted_syscalls_by_time;
    struct rtcdate time;	// system_call used time

    int number_of_arguments;
    struct argument arguments[MAX_ARGUMENTS_NUMBER];	    // system_call arguments
};

// It has been used to store array of system_call_status
// Number of stored system_call_status objects is stored in number_of_calls
// system_calls is One based.
struct system_call {
    int number_of_calls;

    // The 1-based array has been used to be compatible with number of system calls
    struct system_call_status system_calls[MAX_SYS_CALL_NUMBERS];
};

// Array of pointer to system_call_status
// It has been used to store system_call status in the order of usage time
// Number of stored system_call_status objects is stored in number_of_calls
struct array_of_syscall_pointer {
    int number_of_calls;
    struct system_call_status* items[NPROC * MAX_SYS_CALL_NUMBERS];
} sorted_syscalls;

// The 1-based array has been used to be compatible with system call number
int system_calls[SYS_CALL_NUMBERS];
struct system_call process_system_calls[NPROC];

struct cpu {
	uchar apicid;								 // Local APIC ID
	struct context *scheduler;					 // swtch() here to enter scheduler
	struct taskstate ts;						 // Used by x86 to find stack for interrupt
	struct segdesc gdt[NSEGS];					 // x86 global descriptor table
	volatile uint started;						 // Has the CPU started?
	int ncli;									 // Depth of pushcli nesting.
	int intena;								 	 // Were interrupts enabled before pushcli?
	struct proc *proc;							 // The process running on this cpu or null
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
