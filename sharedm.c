//mappager: virtual physical 1
//kalloc:   returns physical addresss
//p2v
//size field in proc struct first assign then increament
//


#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "elf.h"

struct shm {
    uint id;
    uint owner_process;
    uint flags;
    uint ref_count;
    uint size;
    enum { ONLY_OWNER_WRITE, ONLY_CHILD_CAN_ATTACH } shared_page_physical_addresses;
};

struct shm shm_table[100];
