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
            cprintf("Error: id is duplicate");
            return -1;
        }
    }
    struct proc* curproc = myproc();
    char * physical_address;

    for (int i = 0; i < page_count; i++)
    {
        physical_address = initsharedmem(curproc->pgdir, curproc->sz);
        curproc->sz += PGSIZE;
        shm_table[shm_table_size].shared_page_physical_addresses[i] = physical_address;
    }


    shm_table[shm_table_size].id = id;
    shm_table[shm_table_size].owner_process = curproc->pid;
    shm_table[shm_table_size].flags = flag;
    shm_table[shm_table_size].ref_count++;
    shm_table[shm_table_size].size = page_count;

    shm_table_size++;

    return 0;
}
void * shm_attach(int id)
{
    struct proc* curproc = myproc();
    int segment_id = -1;
    for (int i = 0; i < SHMEMTABSIZE; i++)
    {
        if (shm_table[i].id == id)
        {
            segment_id = i;
        }
    }
    if (segment_id == -1)
        return 0;

    shm_table[segment_id].ref_count++;

    for (int i = 0; i < shm_table[segment_id].size; i++)
    {
        uint phys_add = shm_table[segment_id].shared_page_physical_addresses[i];
        mappagesinsharedmem(curproc->pgdir, curproc->sz, phys_add);
        curproc->sz+= PGSIZE;
    }
    return 0;
    // should return virtual address of start of shared segment


}
int shm_close(int id)
{
    int row;
    for (row = 0; row < shm_table_size; row++)
    {
        if (shm_table[row].id == id)
        {
            shm_table[row].ref_count--;
            break;
        }
    }

    shm_table[row].id = shm_table[shm_table_size - 1].id;
    shm_table[row].owner_process = shm_table[shm_table_size - 1].owner_process;
    shm_table[row].flags = shm_table[shm_table_size - 1].flags;
    shm_table[row].ref_count = shm_table[shm_table_size - 1].ref_count;
    shm_table[row].size = shm_table[shm_table_size - 1].size;
    for (int i = 0; i < shm_table[row].size; i++)
        shm_table[row].shared_page_physical_addresses[i] =
                shm_table[shm_table_size - 1].shared_page_physical_addresses[i];

    shm_table_size--;
}
