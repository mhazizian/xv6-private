#include "param.h"
enum shm_flag { ONLY_OWNER_WRITE = 0, ONLY_CHILD_CAN_ATTACH = 1, ONLY_OWNER_WRITE_ONLY_CHILD_ATTACH = 2, NONE = 3 };
int shm_open(int, int, int);
void * shm_attach(int);
int shm_close(int);

struct shm {
    uint id;
    uint owner_process;
    enum shm_flag flags;
    uint ref_count;
    // number of tables allocated to this shared memory(page_count)
    uint size;
    uint shared_page_physical_addresses[15];
};

struct shm shm_table[SHMEMTABSIZE];
int shm_table_size;
