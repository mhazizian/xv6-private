#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"
#include "syscall.h"
#include "sharedm.h"

//mappager: virtual physical 1
//kalloc:   returns physical addresss
//p2v
//size field in proc struct first assign then increament
//

//KMEM : AN ARRAY OF FREE FRAMSE


//KALLOC : GET A VIRTUAL ADDRESS FROM KMEM

//MAPPAGES: MAP VIRTUAL TO PHYSICAL




int shm_open(int id, int page_count, int flag)
{
    for (int i = 0; i < SHMEMTABSIZE; i++)
    {
        if (shm_table[i].id == id)
        {
            cprintf("Error: pid is duplicate");
            return 0;
        }
    }
    struct proc* curproc = myproc();

    initsharedmem(curproc->pgdir, curproc->sz);
    curproc->sz += PGSIZE;


    shm_table[shm_table_size].id = id;
    shm_table[shm_table_size].owner_process = curproc->pid;
    shm_table[shm_table_size].flags = flag;
    shm_table[shm_table_size].ref_count++;
    shm_table[shm_table_size].size = page_count;




    shm_table_size++;
}
void * shm_attach(int id)
{
}
int shm_close(int id)
{

}
