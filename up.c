#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"


int main(int argc, char *argv[])
{
    shm_open(1, 10, 0);

    if (fork() == 0)
    {
        char* virt_addr = shm_open(2, 10, 0);
        printf(1, "***********this is return of shm_open: %d\n ", shm_open(3, 10, 0));
        virt_addr = shm_open(4, 10, 0);
        *virt_addr = 'c';
        printf(1, "write done  %p\n", virt_addr);
        exit();
    }
    else
        {
            sleep(100);
            printf(1, "read character\n");
            char * virt_addr = shm_attach(2);
            printf(1, "virtual address in parent: %p\n", virt_addr);
            printf(1, "read character in parent: %c\n", *virt_addr);
    }


    sleep(500);
    exit();
    return 0;
}
