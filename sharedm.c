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
//    for (int i = 0; i < SHMEMTABSIZE; i++)
//        cprintf("%d\n", shm_table[i].id);
    for (int i = 0; i < shm_table_size; i++)
    {
        if (shm_table[i].id == id)
        {
            cprintf("Error: id is duplicate %d, %d\n", shm_table[i].id, id);
            return -1;
        }
    }

    struct proc* curproc = myproc();
    uint physical_address;

    for (int i = 0; i < page_count; i++)
    {
        physical_address = initsharedmem(curproc->pgdir, curproc->sz);
        if (physical_address == -1)
        {
            cprintf("Error: allocation failed");
            return -2;
        }
        curproc->sz += PGSIZE;
        shm_table[shm_table_size].shared_page_physical_addresses[i] = physical_address;
    }


    shm_table[shm_table_size].id = id;
    shm_table[shm_table_size].owner_process = curproc->pid;
    shm_table[shm_table_size].flags = flag;//TODO
    shm_table[shm_table_size].ref_count++;
    shm_table[shm_table_size].size = page_count;

    shm_table_size++;

    int sharedm_va = curproc->sz - PGSIZE * page_count;

    curproc->sharedm_ids[curproc->sharedm_count] = id;
    curproc->sharedm_virtual_addresses[curproc->sharedm_count] = sharedm_va;
    curproc->sharedm_count++;
    
    // should return virtual address of start of shared segment
    pte_t *pte = global_walkpgdir(curproc->pgdir, (void *) sharedm_va, 0);
    cprintf("## Open: va to physical: %d to %d, pid = %d\n", sharedm_va, PTE_ADDR(*pte), curproc->pid);
    return sharedm_va;
}
void * shm_attach(int id)
{
    int permission;
    struct proc* curproc = myproc();
    int segment_id = -1;
    for (int i = 0; i < shm_table_size; i++)
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
        switch(shm_table[segment_id].flags)
        {
            case NONE:
                permission = PTE_W | PTE_U | PTE_P;
                break;
            case ONLY_CHILD_CAN_ATTACH:
                if (curproc->parent != shm_table[segment_id].owner_process)
                {
                    cprintf("Only child can attach\n");
                    return -3;
                    break;
                }
            case ONLY_OWNER_WRITE:
                permission = PTE_U | PTE_P;
                break;
            case ONLY_OWNER_WRITE_ONLY_CHILD_ATTACH:
                if (curproc->parent != shm_table[segment_id].owner_process)
                {
                    cprintf("Only child can attach\n");
                    return -3;
                    break;
                }
                permission = PTE_U | PTE_P;
        }
        if (mappagesinsharedmem(curproc->pgdir, curproc->sz, phys_add, permission) < 0)
        {
            cprintf("Error: memory map failed");
            return (void*)-2;
        }
        curproc->sz+= PGSIZE;
    }
    int sharedm_va = curproc->sz - PGSIZE * shm_table[segment_id].size;

    curproc->sharedm_ids[curproc->sharedm_count] = segment_id;
    curproc->sharedm_virtual_addresses[curproc->sharedm_count] = sharedm_va;
    curproc->sharedm_count++;

    // should return virtual address of start of shared segment
    pte_t *pte = global_walkpgdir(curproc->pgdir, (void *) sharedm_va, 0);
    cprintf("## Attach: va to physical: %d to %d, pid = %d\n", sharedm_va, PTE_ADDR(*pte), curproc->pid);
    return sharedm_va;
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

    shm_table_size--;

    shm_table[row].id = shm_table[shm_table_size].id;
    shm_table[row].owner_process = shm_table[shm_table_size].owner_process;
    shm_table[row].flags = shm_table[shm_table_size].flags;
    shm_table[row].ref_count = shm_table[shm_table_size].ref_count;
    shm_table[row].size = shm_table[shm_table_size].size;
    for (int i = 0; i < shm_table[row].size; i++) {
        shm_table[row].shared_page_physical_addresses[i] =
                    shm_table[shm_table_size].shared_page_physical_addresses[i];
    }
    return 0;
}
