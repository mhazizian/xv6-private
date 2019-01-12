#include "param.h"
enum shm_flag { ONLY_OWNER_WRITE = 0, ONLY_CHILD_CAN_ATTACH = 1 };
int shm_open(int, int, int);
void * shm_attach(int);
int shm_close(int);

struct shm {
    uint id;
    uint owner_process;
    enum shm_flag flags;
    uint ref_count;
    uint size;
    char* shared_page_physical_addresses[15];
};

struct shm shm_table[100];
int shm_table_size;
