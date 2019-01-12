
enum shm_flag { ONLY_OWNER_WRITE, ONLY_CHILD_CAN_ATTACH };

struct shm {
    uint id;
    uint owner_process;
    uint flags;
    uint ref_count;
    uint size;
    enum shm_flag shared_page_physical_addresses;
};

struct shm shm_table[100];